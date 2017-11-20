#include "RenderFrameManager.h"
#include <unistd.h>
#include <algorithm>
#include "../Base/MojingTimer.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif


namespace Baofeng
{
	namespace Mojing
	{
		RenderFrameManager::RenderFrameManager():
			m_pRenderFrames(NULL),
			m_ui64NextFrameID(0),
			m_iDistortionSkip(0),
			m_bExitState(false)
		{
			// m_pEnableNextModelRender = new Event;
			m_bEnableNextModelRender = false;
			//m_pEnableDistortoinRender = new Event;
			m_bEnableDistortoinRender = false;
			m_pModleLocker = new Mutex;
			m_pDistortionLocker = new Mutex;
			m_pLastDisplayLocker = new Mutex;
			RenderFrame::InitDrawTime();
		}


		RenderFrameManager::~RenderFrameManager()
		{
			Release();
			// delete m_pEnableNextModelRender;
			m_bEnableNextModelRender = false;

			//delete m_pEnableDistortoinRender;
			m_bEnableDistortoinRender = false;

			delete m_pModleLocker;
			delete m_pDistortionLocker;
			delete m_pLastDisplayLocker;
		}

		void RenderFrameManager::InitFrame()
		{
			bool bExitState = m_bExitState;
			Release();
			
			//m_pEnableNextModelRender->ResetEvent();
			//m_bEnableNextModelRender = true;
			// m_pEnableDistortoinRender->ResetEvent();
			
			

			m_bExitState = bExitState;
			m_pRenderFrames = new RenderFrame[FRAME_COUNT];
			for (int i = 0; i < FRAME_COUNT; i++)
			{
				m_ModelIndex.push_back(i);
			}

			SetMaxFrameCount(FRAME_COUNT);
			// MOJING_TRACE(g_APIlogger, "Start Distion Thread , and Active ModleRender" << m_ModelIndex.size() << " / " << m_DistortionIndex.size() << " / " << m_LastDisplayIndex.size());
			//m_pEnableNextModelRender->PulseEvent();
			m_bEnableNextModelRender = true;
			m_bEnableDistortoinRender = false;
		}


		void RenderFrameManager::Release()
		{
			// 释放所有的对象锁，让他们能够跑下去
			m_bExitState = true;
			
			//m_pEnableNextModelRender->SetEvent();
			m_bEnableNextModelRender = true;
			//m_pEnableDistortoinRender->SetEvent();
			m_bEnableDistortoinRender = true;

			m_pModleLocker->Unlock();
			m_pDistortionLocker->Unlock();
			m_pLastDisplayLocker->Unlock();
			SetMaxFrameCount(0);
			// 注意：这是一个对象数组，所以要用 [] delete
			delete[] m_pRenderFrames;
			m_pRenderFrames = NULL;

			if (m_ModelIndex.size())
				m_ModelIndex.erase(m_ModelIndex.begin(), m_ModelIndex.end());
			if (m_DistortionIndex.size())
				m_DistortionIndex.erase(m_DistortionIndex.begin(), m_DistortionIndex.end());
			if (m_LastDisplayIndex.size())
				m_LastDisplayIndex.erase(m_LastDisplayIndex.begin(), m_LastDisplayIndex.end());
		}

		// 返回当前用于绘制模型的Frame
		RenderFrame* RenderFrameManager::GetCurrentModleFrame()
		{
			RenderFrame* pRet = NULL;
			while (m_ModelIndex.size() < 1)
			{
				if (m_bExitState)
					return NULL;
				usleep(200);
			}
			//for (int iTry = 0; iTry < (int)m_ModelIndex.size(); iTry++)
			int iTry = 0;
			int iModelIndex = m_ModelIndex[iTry];
			pRet = m_pRenderFrames + iModelIndex;
			
			if (RenderFrame::GetMultiThread())
			{
				pRet = m_pRenderFrames + iModelIndex;
				FrameWorkFlowState CURRENT_STATE = pRet->GetFrameWorkFlowState();
				if (CURRENT_STATE == DISPLAY_END ||
					CURRENT_STATE == UNINIT ||
					CURRENT_STATE == INIT)
				{
					{// 为了处理双眼分开多次绘制的问题，将等待时机放到这个地方
						//while (!m_pEnableNextModelRender->Wait(1))
						while (!m_bEnableNextModelRender)
						{
							if (m_bExitState)
								return NULL;
							//MOJING_TRACE(g_APIlogger, "Sleep to wait m_bEnableNextModelRender signal");
							usleep(200);
						}
						// 单线程模式下不要锁
						m_bEnableNextModelRender = false;
					}
				}
			}
			
			FrameWorkFlowState CURRENT_STATE = pRet->GetFrameWorkFlowState();
			if (CURRENT_STATE != RENDER_MODLE&&
				CURRENT_STATE != RENDER_MODLE_LEFT &&
				CURRENT_STATE != RENDER_MODLE_RIGHT &&
				CURRENT_STATE != RENDER_MODLE_BOTH)
			{
				if (m_ui64NextFrameID == 0)
				{
					pRet->BeforModle(0);
				}
				else
				{
					int iStep = 0;
					while (!pRet->BeforModle(m_ui64NextFrameID + iStep))
					{
						iStep++;
					}
				}
				m_ui64NextFrameID = pRet->GetFrameIndex() + 1;
			}

			// 程序执行到这里表示工作流程出了问题
			return pRet;
		}
		
		RenderFrame* RenderFrameManager::GetCurrentDistortionFrame(const UInt64 ui64DistortionFrameIndex, const double dEndTime)
		{
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, ui64DistortionFrameIndex << ", " << CURRENT_RENDER_TIME() << " Start Distortion. End time: " << Timer::FormatDoubleTime(dEndTime));
#endif
			RenderFrame* pRet = NULL;
			if (!RenderFrame::GetMultiThread())
			{// 单线程直接给出待畸变帧
				while (m_DistortionIndex.size())
				{
					m_LastDisplayIndex.push_back(m_DistortionIndex.front());
					m_DistortionIndex.erase(m_DistortionIndex.begin());
				}
				pRet = m_pRenderFrames + m_LastDisplayIndex.back();
				return pRet;
			}
			// 如果在指定时间之内不能得到要绘制的帧，那么用历史帧
			double dWaitForRender = dEndTime;
			// 1首先尝试得到最近画完模型的那一帧
//			int iWaitTime = fmax( 0 , (dEndTime - Timer::GetSeconds()) * 1000);
// 			if (!m_pEnableDistortoinRender->Wait(iWaitTime))
// 				return NULL;
			while (!m_bEnableDistortoinRender && dEndTime > Timer::GetSeconds())
			{
				if (m_bExitState )
				{// Time out
					// MOJING_TRACE(g_APIlogger, "GetCurrentDistortionFrame : EXIT " << " Frames :" << m_ModelIndex.size() << " / " << m_DistortionIndex.size() << " / " << m_LastDisplayIndex.size());
					return pRet;
				}
				usleep(100);
			}
			if (m_DistortionIndex.size() < 1)
			{// 超时或者没有可以畸变的东西
				MOJING_ERROR(g_APIlogger, "GetCurrentDistortionFrame : " << ui64DistortionFrameIndex  << "TIME OUT " << " Frames :" << m_ModelIndex.size() << " / " << m_DistortionIndex.size() << " / " << m_LastDisplayIndex.size());
				/*程序执行到这里，意味着畸变将被迫使用历史帧。为了保持住同步，这里把帧号+1*/
				if (m_ui64NextFrameID != 0 &&
					m_LastDisplayIndex.size() > 1 // 为了防止第一帧模型绘制完成之前出现帧号被重复++的问题，这里判断一下没有历史帧则不累计帧号
					)
				{// 疑似是生产满了，给个信号让模型干活
					MOJING_TRACE(g_APIlogger, "Active Modle Render (Skip)");
					//m_pEnableNextModelRender->PulseEvent();
					m_bEnableNextModelRender = true;
					m_ui64NextFrameID++;
				}
				return pRet;
			}
			m_bEnableDistortoinRender = false;
			
			/*如果畸变队列里面有多帧的话，把那些已经不适用的帧移出去*/
			while (m_DistortionIndex.size() > 1)
			{
				int iIndex = m_DistortionIndex.front();
				pRet = m_pRenderFrames + iIndex;
				if (pRet->GetFrameIndex() < ui64DistortionFrameIndex)
				{// 把过旧的帧扔掉
					MOJING_TRACE(g_APIlogger, "Skip Frame " << pRet->GetFrameIndex() << " / " << ui64DistortionFrameIndex << "， Too SLOW");
					m_LastDisplayIndex.push_back(m_DistortionIndex.front());

					m_pDistortionLocker->DoLock();
					m_DistortionIndex.erase(m_DistortionIndex.begin());
					m_pDistortionLocker->Unlock();
				}
				else
				{
					break;
				}
			}
			

			int iDistortion = m_DistortionIndex.front();
			pRet = m_pRenderFrames + iDistortion;
			if (pRet->GetFrameIndex() > ui64DistortionFrameIndex && ui64DistortionFrameIndex != 0)
			{
				if (m_DistortionIndex.size() == 1)
				{
					MOJING_TRACE(g_APIlogger, "Model Render is a little faster, skip it and active Model Render");
					m_bEnableNextModelRender = true;
				}
				else
				{
					MOJING_TRACE(g_APIlogger, "Model Render is too fast, skip it and do not Active Modle Render , Start Next Distortion ");

					m_bEnableDistortoinRender = true;
				}
				return NULL;
			}
			
			//MOJING_TRACE(g_APIlogger, "Active Model Render");
			m_bEnableNextModelRender = true;
			
			if (!pRet->BeforDistortion(dWaitForRender))
			{// 没有办法在指定时间内等到模型绘制完成
#ifdef _DEBUG
				// MOJING_TRACE(g_APIlogger, "Move Frame " << m_pRenderFrames[iDistortion].GetFrameIndex() << " Distortion ---> Last Display (Skip)");
				char PD[20] , Now[20];
				strcpy(PD, Timer::FormatDoubleTime(pRet->GetPredictDisplayTime()));
				strcpy(Now, Timer::FormatDoubleTime(Timer::GetSeconds()));

				MOJING_ERROR(g_APIlogger, "GetCurrentDistortionFrame : Wait Model Finish..." << m_pRenderFrames[iDistortion].GetFrameIndex() << " Predict " << PD << " / Now " << Now << " , Frames :" << m_ModelIndex.size() << " / " << m_DistortionIndex.size() << " / " << m_LastDisplayIndex.size());
				
// Todo 追帧应该放到外面去
				if (m_ui64NextFrameID <= ui64DistortionFrameIndex + 1)
				{
					MOJING_ERROR(g_APIlogger, "Make Model Render Faster , Next Model Frame Will be " << ++m_ui64NextFrameID);
				}
#endif				
				m_LastDisplayIndex.push_back(m_DistortionIndex.front());
				m_pDistortionLocker->DoLock();
				m_DistortionIndex.erase(m_DistortionIndex.begin());
				m_pDistortionLocker->Unlock();
				// 这里会出现跳帧
				pRet = NULL;
			}
			else
			{
				pRet = m_pRenderFrames + iDistortion;
				pRet->SetFrameWorkFlowState(DISTION_BEGIN);
				// MOJING_TRACE(g_APIlogger, "GetCurrentDistortionFrame : " << pRet->GetFrameIndex() << " Frames :" << m_ModelIndex.size() << " / " << m_DistortionIndex.size() << " / " << m_LastDisplayIndex.size());
				
			}

			return pRet;
		}

		RenderFrame* RenderFrameManager::GetLastDisplayFrame()
		{
			RenderFrame* pRet = NULL;
			//m_pLastDisplayLocker->DoLock();
			if (m_LastDisplayIndex.size() >= 1)
			{
				for (int i = m_LastDisplayIndex.size() - 1; i >= 0 ; i--)
				{	// 这一帧只需要判定是否完成了模型绘制就可以了
					if (m_pRenderFrames[m_LastDisplayIndex[i]].TestModleFinish())
					{
						pRet = m_pRenderFrames + m_LastDisplayIndex[i];
						break;
					}
				}
			}
			//m_pLastDisplayLocker->Unlock();
			return pRet;
		}
		// 设置当前帧的绘制模型完成
		void RenderFrameManager::SetEndOfRenderModle(RenderFrame* pFR, FrameWorkFlowState EyeMode)
		{
			// Model -> Distortion
			int iFrameIndex = pFR - m_pRenderFrames;
			if (iFrameIndex < 0 || iFrameIndex > GetMaxFrameCount())
				return;

			FrameWorkFlowState CurrentState = pFR->GetFrameWorkFlowState();

			if ((EyeMode == RENDER_MODLE_LEFT && CurrentState == RENDER_MODLE_RIGHT) ||
				(EyeMode == RENDER_MODLE_RIGHT && CurrentState == RENDER_MODLE_LEFT))
			{// 完成了双眼绘制场景下两眼的绘制
				pFR->SetFrameWorkFlowState(RENDER_MODLE_BOTH);
			}
			else
			{
				pFR->SetFrameWorkFlowState(EyeMode);
			}

			// 根据新的状态执行后续操作
			CurrentState = pFR->GetFrameWorkFlowState();
			if (CurrentState == RENDER_MODLE_LEFT || CurrentState == RENDER_MODLE_RIGHT)
			{// 只完成了一只眼睛的绘制，激活等待绘制的信号
// 				// MOJING_TRACE(g_APIlogger, "Active Model , Wait next Eye");
// 				m_pEnableNextModleRender->PulseEvent();
			}else if (CurrentState == RENDER_SUCCEED)
			{// 完成了模型绘制
				//MOJING_TRACE(g_APIlogger, CURRENT_RENDER_TIME() << "Call AfterModle: " << pFR->GetFrameIndex());
				pFR->AfterModle();

				m_pModleLocker->DoLock();
				std::vector<int>::iterator it = std::find(m_ModelIndex.begin(), m_ModelIndex.end(), iFrameIndex);
				if (it != m_ModelIndex.end())
				{
					// MOJING_TRACE(g_APIlogger, "Move Frame " << pFR->GetFrameIndex() << " Model Render ---> ");
					m_ModelIndex.erase(it);
				}
				m_pModleLocker->Unlock();

				m_pDistortionLocker->DoLock();
				//it = std::find(m_DistortionIndex.begin(), m_DistortionIndex.end(), iFrameIndex);
				//if (it == m_DistortionIndex.end())
				//{
				//	// MOJING_TRACE(g_APIlogger, "Move Frame " << pFR->GetFrameIndex() << "---> Distortion");
					m_DistortionIndex.push_back(iFrameIndex);
				//}
				m_pDistortionLocker->Unlock();
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, pFR->GetFrameIndex() << ", " << CURRENT_RENDER_TIME() << " Move to Distortion: ");
#endif
				// MOJING_TRACE(g_APIlogger, "Active Distion , " << m_ModelIndex.size() << " / " << m_DistortionIndex.size() << " / " << m_LastDisplayIndex.size());
				// m_pEnableDistortoinRender->PulseEvent();
				m_bEnableDistortoinRender = true;
			}
		}
		// 设置畸变帧的畸变完成
		void RenderFrameManager::SetEndOfDistortion(RenderFrame* pFR)
		{
			// Model -> Distortion
			int iFrameIndex = pFR - m_pRenderFrames;
			if (iFrameIndex < 0 || iFrameIndex > GetMaxFrameCount())
				return;

			if (m_bExitState)
			{
				return;
			}

			m_pDistortionLocker->DoLock();
			std::vector<int>::iterator it = std::find(m_DistortionIndex.begin(), m_DistortionIndex.end(), iFrameIndex);
			if (it != m_DistortionIndex.end())
			{
				// MOJING_TRACE(g_APIlogger, "Move Frame " << pFR->GetFrameIndex() << " Distortion --->");
				m_DistortionIndex.erase(it);
			}
			m_pDistortionLocker->Unlock();
			if (m_bExitState)
			{
				return;
			}

			m_pLastDisplayLocker->DoLock();
			it = std::find(m_LastDisplayIndex.begin(), m_LastDisplayIndex.end(), iFrameIndex);
			if (it == m_LastDisplayIndex.end())
			{
				// MOJING_TRACE(g_APIlogger, "Move Frame " << pFR->GetFrameIndex() << " ---> Last Display");
				m_LastDisplayIndex.push_back(iFrameIndex);
			}
			m_pLastDisplayLocker->Unlock();
			if (m_bExitState)
			{
				return;
			}
			// 设置为等待图层渲染
			pFR->AfterDistortion();
			
		}
		// 设置帧的显示完成
		void RenderFrameManager::SetEndOfDisplay()
		{	// 这个函数的作用是把所有已经完成显示的（最后一帧除外），都移动到模型队列里面
			vector<int> IndexAppendToModel;
			bool bMoveToAppend = false;
			//m_pLastDisplayLocker->DoLock();
			if (m_LastDisplayIndex.size() > 1)
			{
				for (int i = m_LastDisplayIndex.size() - 1; i >= 0; i--)
				{	// 这一帧只需要判定是否完成了模型绘制就可以了
					if (!bMoveToAppend)
					{
						bMoveToAppend = m_pRenderFrames[m_LastDisplayIndex[i]].TestModleFinish();
					}
					else
					{
						IndexAppendToModel.push_back(m_LastDisplayIndex[i]);
					}// end of append to remove vector
				}// end of scan m_LastDisplayIndex 
				// 程序执行到这里，所有需要从显示完成队列中移除的帧下标已经记录在IndexAppendToModel之中
				//////////////////////////////////////////////////////////////////////////////
				for (int i = 0; i < (int)IndexAppendToModel.size(); i++)
				{// remov...
					std::vector<int>::iterator itEarse = std::find(m_LastDisplayIndex.begin(), m_LastDisplayIndex.end(), IndexAppendToModel[i]);
					if (itEarse != m_LastDisplayIndex.end())
					{
						// MOJING_TRACE(g_APIlogger, "Move Frame " << m_pRenderFrames[IndexAppendToModel[i]].GetFrameIndex() << "Last Display ---> ");
						m_LastDisplayIndex.erase(itEarse);
					}
				}
			}// m_LastDisplayIndex.size
			//m_pLastDisplayLocker->Unlock();			

			// Append to model vector
			m_pModleLocker->DoLock();
			// 注意：因为IndexAppendToModel是按照时间倒叙的，所以这里还要反着插回模型队列中
			for (int i = (int)IndexAppendToModel.size() - 1; i >= 0; i--)
			{
				std::vector<int>::iterator itAppend = std::find(m_ModelIndex.begin(), m_ModelIndex.end(), IndexAppendToModel[i]);
				if (itAppend == m_ModelIndex.end())
				{
					// MOJING_TRACE(g_APIlogger, "Move Frame " << m_pRenderFrames[IndexAppendToModel[i]].GetFrameIndex() << " ---> Model Render");
					m_ModelIndex.push_back(IndexAppendToModel[i]);
					m_pRenderFrames[IndexAppendToModel[i]].SetFrameWorkFlowState(DISPLAY_END);
				}
			}
			m_pModleLocker->Unlock();
		}


	}
}
