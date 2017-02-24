#include "RenderFrameCirular.h"
#include "../Base/MojingTimer.h"
#include <unistd.h>


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
#define CIRULAR_FRAME_COUNT  3// 注意：这里只允许有3帧
		RenderFrameCirular::RenderFrameCirular():
			m_pRenderFrames(NULL),
			m_ui64NextFrameID(0),
			m_iLastDistortionFrameInCirular(-1),
			m_iCurRenderFrameInCirular(0),
			m_ui64EnableRenderTxtureID(0),
			m_bExitState(false)
		{
		}


		RenderFrameCirular::~RenderFrameCirular()
		{
		}


		void RenderFrameCirular::InitFrame()
		{
			bool bExitState = m_bExitState;
			Release();

			m_bExitState = bExitState;
			m_pRenderFrames = new RenderFrame[CIRULAR_FRAME_COUNT];
			SetMaxFrameCount(CIRULAR_FRAME_COUNT);
			m_iLastDistortionFrameInCirular = -1;
			m_iCurRenderFrameInCirular = 0;
		}

		void RenderFrameCirular::Release()
		{
			// 释放所有的对象锁，让他们能够跑下去
			m_bExitState = true;
			SetMaxFrameCount(0);
			// 注意：这是一个对象数组，所以要用 [] delete
			delete[] m_pRenderFrames;
			m_pRenderFrames = NULL;

			m_iLastDistortionFrameInCirular = -1;
			m_iCurRenderFrameInCirular = 0;

		}

		// 返回当前用于绘制模型的Frame
		RenderFrame* RenderFrameCirular::GetCurrentModleFrame()
		{
			if (m_iCurRenderFrameInCirular == m_iLastDistortionFrameInCirular)
				return NULL;// 模型太快

			RenderFrame* pRet = m_pRenderFrames + m_iCurRenderFrameInCirular;
			FrameWorkFlowState CURRENT_STATE = pRet->GetFrameWorkFlowState();
			if (CURRENT_STATE == DISTION_BEGIN || CURRENT_STATE == DISTION_END)
				return NULL;// 状态不对

			if (CURRENT_STATE == DISPLAY_END ||
				CURRENT_STATE == UNINIT ||
				CURRENT_STATE == INIT)
			{// 这是第一次请求--显示完成的或者刚刚初始化的
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
				if (m_ui64EnableRenderTxtureID != 0 && pRet->GetFrameIndex() > m_ui64EnableRenderTxtureID)
				{// 模型太快
					pRet->SetFrameWorkFlowState(DISPLAY_END);
					usleep(100);
					return NULL;
				}
				m_ui64NextFrameID = pRet->GetFrameIndex() + 1;
			}
			return pRet;
		}
		
		RenderFrame* RenderFrameCirular::GetCurrentDistortionFrame(const UInt64 ui64DistortionFrameIndex, const double dEndTime)
		{
			int iRet = -1;
			int iTest = NextInCirular(m_iLastDistortionFrameInCirular);
			/*这个假说是先进入渲染流程的帧应该也会先完成渲染*/
			while (!m_bExitState
				&&(iTest != m_iCurRenderFrameInCirular))
			{
				if (m_pRenderFrames[iTest].BeforDistortion(dEndTime))
				{// 这一帧的模型已经绘制完成了
					iRet = iTest;
					if (m_pRenderFrames[iTest].GetFrameIndex() == ui64DistortionFrameIndex || ui64DistortionFrameIndex == 0)
					{	// 这一帧就是我们需要的帧，不再等待和查找了
						break;
					}
					// 继续检查下一帧
					iTest = NextInCirular(iTest);
				}
				else if (dEndTime < 0 || Timer::GetSeconds() < dEndTime)
				{
					usleep(100);
				}
				else
				{
					break;
				}
			}

			if (iRet == -1 || m_bExitState)
				return NULL;

			m_iLastDistortionFrameInCirular = iRet;
			return m_pRenderFrames + m_iLastDistortionFrameInCirular;
		}

		RenderFrame* RenderFrameCirular::GetLastDisplayFrame()
		{
			if (!m_bExitState &&
				m_iLastDistortionFrameInCirular >= 0 && 
				m_iLastDistortionFrameInCirular != m_iCurRenderFrameInCirular &&
				m_pRenderFrames[m_iLastDistortionFrameInCirular].TestModleFinish())
				return m_pRenderFrames+m_iLastDistortionFrameInCirular;
			return NULL;
		}

		// 设置当前帧的绘制模型完成
		void RenderFrameCirular::SetEndOfRenderModle(RenderFrame* pFR, FrameWorkFlowState EyeMode)
		{
// 			MOJING_FUNC_TRACE(g_APIlogger);
// 			MOJING_TRACE(g_APIlogger , "SetEndOfRenderModle Frame ID = " << pFR->GetFrameIndex());
			// Model -> Distortion
			int iFrameIndex = pFR - m_pRenderFrames;
			if (m_bExitState || iFrameIndex < 0 || iFrameIndex > GetMaxFrameCount())
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
			}
			else if (CurrentState == RENDER_SUCCEED)// 多线程的时候这个标记由 MojingRenderMultiThread::WarpToScreen()函数触发
			{// 完成了模型绘制
				//MOJING_TRACE(g_APIlogger, CURRENT_RENDER_TIME() << "Call AfterModle: " << pFR->GetFrameIndex());
				pFR->AfterModle();
				int iTest = NextInCirular(m_iCurRenderFrameInCirular);
				/*MOJING_TRACE(g_APIlogger , "##Wait next render frame....1");*/
				while (iTest == m_iLastDistortionFrameInCirular)
				{// 模型过快保护代码
					usleep(100);
				}
				/*MOJING_TRACE(g_APIlogger, "##Wait next render frame....2");*/
				iTest = m_iCurRenderFrameInCirular;
				m_iCurRenderFrameInCirular = NextInCirular(m_iCurRenderFrameInCirular);
				// 等待刚刚完成的模型进入畸变流程
				/*MOJING_TRACE(g_APIlogger, "##Wait next render frame....3");*/
				while (iTest != m_iLastDistortionFrameInCirular)
				{// 防止模型过快
					usleep(100);
				}
				/*MOJING_TRACE(g_APIlogger, "##Wait next render frame....4");*/
			}
		}
		// 设置畸变帧的畸变完成
		void RenderFrameCirular::SetEndOfDistortion(RenderFrame* pFR)
		{
// 			MOJING_FUNC_TRACE(g_APIlogger);
// 			MOJING_TRACE(g_APIlogger, "SetEndOfDistortion Frame ID = " << pFR->GetFrameIndex());
			// 设置为等待图层渲染
			pFR->AfterDistortion();
		}
		// 设置帧的显示完成
		void RenderFrameCirular::SetEndOfDisplay()
		{
			int iCheck = m_iLastDistortionFrameInCirular;
			if (iCheck < 0)
				return;
			/* 这里会不会有多线程竞争的问题？*/
			do
			{
				if (m_pRenderFrames[iCheck].GetFrameWorkFlowState() != UNINIT)
					m_pRenderFrames[iCheck].SetFrameWorkFlowState(DISPLAY_END);
				iCheck = PrevInCirular(iCheck);
			} while (iCheck != m_iCurRenderFrameInCirular);
		}

	}
}

