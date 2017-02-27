#include "RenderFrame.h"
#include <unistd.h>
#include "../MojingManager.h"
#include "../Tracker/MojingTracker.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#include "Vsync.h"

// 在没有启用FBR的系统里面，安卓系统自身有3帧的缓存，且需要考虑到屏幕刷新还需要额外的10ms左右
#define SWAP_SHOW_DELAY	(0 *  DISPLY_FRAME_TIME_IN_SEC)

namespace Baofeng
{
	namespace Mojing
	{
		// 静态变量
		int    RenderFrame::m_uiCurrentDrawTimeIndex = 0;
		float  RenderFrame::m_fModelDrawTime[5] = { DISPLY_FRAME_TIME_IN_SEC, DISPLY_FRAME_TIME_IN_SEC, DISPLY_FRAME_TIME_IN_SEC, DISPLY_FRAME_TIME_IN_SEC, DISPLY_FRAME_TIME_IN_SEC };
		
		void RenderFrame::SetDrawTime(float time) 
		{
			m_fModelDrawTime[m_uiCurrentDrawTimeIndex] = time;
			m_uiCurrentDrawTimeIndex++; 
			if (m_uiCurrentDrawTimeIndex > 4)
			{
				m_uiCurrentDrawTimeIndex = 0;
			}
		}
		
		float RenderFrame::GetDrawTime()
		{
			return (m_fModelDrawTime[0] + m_fModelDrawTime[1] + m_fModelDrawTime[2] + m_fModelDrawTime[3] + m_fModelDrawTime[4]) * 12;
		}

		void RenderFrame::InitDrawTime()
		{
			for (int i = 0; i < 5; i++)
			{
				m_fModelDrawTime[i] = 0.010f;
			}
			m_uiCurrentDrawTimeIndex = 0;
		}
	
		UInt16 RenderFrame::m_uiNextFrameKey = 0;
		UInt64 RenderFrame::m_ui64CurrentFrameIndex = 0;
		bool RenderFrame::m_bMultiThread = false;


		RenderFrame::RenderFrame():
#ifdef MJ_OS_ANDROID
			m_ModleDisplay(0),
			m_ModleGpuSync(0),
			m_DistortionDisplay(0),
			m_DistortionGpuSync(0),
#endif
			m_fStartDrawTime(0),
			m_FrameWorkFlowState(UNINIT),
			m_dPredictDisplayTime(-1),
			m_PredictView(0,0,0,1)
		{
			SetFrameKey(++m_uiNextFrameKey);
		}
		
		RenderFrame::~RenderFrame()
		{
		}
		
		bool RenderFrame::BeforModle(UInt64 ui64FrameIndex)
		{
#ifndef MJ_OS_MAC
			static UInt64 ui64LastFrameIndex = 0;
			double VsyncFrame = GetFractionalVsync();
			if (VsyncFrame > 1)
			{// 有同步对象
				// 1 计算预计显示帧序号
				
				//if (ui64FrameIndex == 0 || ui64FrameIndex < VsyncFrame - 1)
//				{	// 第一帧,或者已经严重偏差（滞后了2帧）的时候，重新计算正确的帧序号
					// 为了防止显示时间不足，在返回的同步时钟已经大于0.2帧的时候，直接用下2帧的帧号
					float drawn = GetDrawTime();
					ui64FrameIndex = floor(GetFractionalVsync() + drawn /* + 0.5f*/);
					
					// MOJING_TRACE(g_APIlogger, "Reset Frame Index " << ui64FrameIndex);
//				}
				
				// 2 计算预计显示帧时间
				//while (ui64FrameIndex < VsyncFrame + 0.3)
				//{
				//	ui64FrameIndex++;
				//}
				if (ui64LastFrameIndex < ui64FrameIndex)
					ui64LastFrameIndex = ui64FrameIndex;
				else
					ui64LastFrameIndex++;
				m_ui64FrameIndex = ui64LastFrameIndex;
				m_dPredictDisplayTime = FramePointTimeInSeconds(m_ui64FrameIndex);
#ifdef FRAME_DEBUG
				MOJING_TRACE(g_APIlogger, m_ui64FrameIndex << ", " << CURRENT_RENDER_TIME() << " Next frame Model start.  Ave Model frames = " << drawn);
//				static char szNow[20];
//				static char szPredict[20];
//				strcpy(szNow, Timer::FormatDoubleTime(Timer::GetSeconds()));
//				strcpy(szPredict, Timer::FormatDoubleTime(m_dPredictDisplayTime));
//				MOJING_TRACE(g_APIlogger, "Next Frame ID :" << m_ui64FrameIndex << " , DisplayTime = " << szPredict << " , Now = " << szNow);
#endif
			}
			else
#endif
			{// 没有同步对象
				m_ui64FrameIndex = ui64FrameIndex;
				m_dPredictDisplayTime = 0;
			}
			// 3 根据预计显示时间获取预计陀螺仪位置
			UpdatePredictView();

			// TODO 如果算出来的时间小于当前时间怎么办？
			m_fStartDrawTime = Timer::GetSeconds();
			SetFrameWorkFlowState(RENDER_MODLE);

			bool bRet = m_dPredictDisplayTime < 1 ||  m_dPredictDisplayTime > (Timer::GetSeconds() + 0.014f); 
			if (!bRet)
			{
				MOJING_WARN(g_APIlogger, m_ui64FrameIndex << ", " << CURRENT_RENDER_TIME() << " Time is not enough to show this frame");
			}
			return bRet;
		}
		void RenderFrame::UpdatePredictView()
		{
			m_PredictView.x = m_PredictView.y = m_PredictView.z = 0;
			m_PredictView.w = 1;
			Manager* pManager = Manager::GetMojingManager();
			if (pManager)
			{
				Tracker* pTracker = pManager->GetTracker();
				if (pTracker)
				{
					m_PredictView = pTracker->getPredictionPosition(m_dPredictDisplayTime + SWAP_SHOW_DELAY);
					//MOJING_WARN(g_APIlogger, "RenderFrame::UpdatePredictView PredictView: x = " << m_PredictView.x << ", y = " << m_PredictView.y << ", z = " << m_PredictView.z << ", w = " << m_PredictView.w);
				}
			}
		}
		Matrix4f RenderFrame::CalculateTimeWarpMatrix(double dDisplayTime , double * pdOutSensotTime /*= NULL*/)const
		{
			Matrix4f PredictViewMatrix2;// 初始值是预测的四元数
			Quatf Position;
			// 根据预计显示时间获取预计陀螺仪位置
			Manager* pManager = Manager::GetMojingManager();
			if (pManager)
			{
				Tracker* pTracker = pManager->GetTracker();
				if (pTracker)
				{
					Position = pTracker->getPredictionPosition(dDisplayTime, pdOutSensotTime);
					if (Position == Quatf(0, 0, 0, 1))
					{
						Position = pTracker->getPredictionPosition(0, pdOutSensotTime);
					}
					
					if (Position != Quatf(0, 0, 0 , 1))
					{
						PredictViewMatrix2 = Tracker::CalculateTimeWarpMatrix2(m_PredictView , Position);
					}
					else
					{
#ifndef MJ_OS_WIN32
						MOJING_ERROR(g_APIlogger , "Unsing current postion.....");
#else
						// Windows 不用考虑TW
#endif
						// 程序执行到这里表示没有办法得到预期的东西
						Position = pTracker->getPredictionPosition(0);
						PredictViewMatrix2 = Tracker::CalculateTimeWarpMatrix2(m_PredictView , Position);
					}
				}
			}
			return PredictViewMatrix2;
		}
		bool RenderFrame::AfterModle()
		{
#ifdef MJ_OS_ANDROID
			if (m_ModleGpuSync != EGL_NO_SYNC_KHR && m_ModleDisplay != EGL_NO_DISPLAY && eglDestroySyncKHR_)
			{
				eglDestroySyncKHR_(m_ModleDisplay, m_ModleGpuSync);
			}
			if (m_ModleDisplay == EGL_NO_DISPLAY)
				m_ModleDisplay = eglGetCurrentDisplay();

			if (m_ModleDisplay != EGL_NO_DISPLAY && eglCreateSyncKHR_)
			{
				m_ModleGpuSync = eglCreateSyncKHR_(m_ModleDisplay , EGL_SYNC_FENCE_KHR, NULL);
			}
			glFlush(); 
			//MOJING_TRACE(g_APIlogger, CURRENT_RENDER_TIME() << "Call glFlush: " << GetFrameIndex());
#endif
			SetFrameWorkFlowState(RENDER_LAYERS);
			//SetDrawTime(Timer::GetSeconds() - m_fStartDrawTime);
			
			return true;
		}
		bool RenderFrame::BeforDistortion(double dWaitEndTime)
		{
			bool bRet = TestModleFinish();
			while (!bRet && Timer::GetSeconds() < dWaitEndTime)
			{
				bRet = TestModleFinish();
				if (!bRet)
					usleep(100);// 0.1ms;
			}

			if (m_fStartDrawTime > 0)
			{
				if (bRet)
				{
					SetDrawTime(Timer::GetSeconds() - m_fStartDrawTime);
				}
				else
				{
					// Not finished yet, add more time
					SetDrawTime(Timer::GetSeconds() - m_fStartDrawTime + 0.004f);
				}
				m_fStartDrawTime = -1;// 确保只被设置一次
			}
			return bRet;
		}
		bool RenderFrame::AfterDistortion()
		{
			// 将绘制指令提交给GPU
#ifdef MJ_OS_ANDROID
			if (m_bMultiThread)
			{
				if (m_DistortionGpuSync != EGL_NO_SYNC_KHR && m_DistortionDisplay != EGL_NO_DISPLAY && eglDestroySyncKHR_)
				{
					eglDestroySyncKHR_(m_DistortionDisplay, m_DistortionGpuSync);
				}
				if (m_DistortionDisplay == EGL_NO_DISPLAY)
					m_DistortionDisplay = eglGetCurrentDisplay();
				if (m_DistortionDisplay != EGL_NO_DISPLAY && eglCreateSyncKHR_)
				{
					m_DistortionGpuSync = eglCreateSyncKHR_(m_DistortionDisplay, EGL_SYNC_FENCE_KHR, NULL);
				}
				glFlush();
			}
#endif
			SetFrameWorkFlowState(DISTION_END);
			return true;
		}
		void RenderFrame::WaitForDistortionFinish()
		{
#ifdef MJ_OS_ANDROID
			if (m_bMultiThread)
			{
				if (m_FrameWorkFlowState != DISPLAY_END // 可能是被跳过的帧，这里不判定显示状态了
					&& m_DistortionGpuSync != EGL_NO_SYNC_KHR && m_DistortionDisplay != EGL_NO_DISPLAY && eglClientWaitSyncKHR_)
				{
					while (EGL_TIMEOUT_EXPIRED_KHR == eglClientWaitSyncKHR_(m_DistortionDisplay, m_DistortionGpuSync,
						EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, 0))
					{
						MOJING_TRACE(g_APIlogger, "Waiting For Distion Finish.... , Frame = " << GetFrameIndex() << " , WorkFlowState = " << m_FrameWorkFlowState);
						usleep(100);// 0.1ms
					}
					eglDestroySyncKHR_(m_DistortionDisplay, m_DistortionGpuSync);
					m_DistortionGpuSync = EGL_NO_SYNC_KHR;
				}
			}
			else
			{
				m_FrameWorkFlowState = DISTION_END;
			}

			if (m_FrameWorkFlowState == DISTION_END)
				SetFrameWorkFlowState(DISPLAY_END);
#endif
		}

// #ifdef _DEBUG
// 		void RenderFrame::SetFrameWorkFlowState(FrameWorkFlowState V)
// 		{
// 			MOJING_TRACE(g_APIlogger, "Frame " << GetFrameIndex() << " Change WorkFlowState " << m_FrameWorkFlowState << " --> " << V);
// 			m_FrameWorkFlowState = V;
// 		}
// #endif

		bool RenderFrame::TestModleFinish()
		{
			bool bRet = true;
#ifdef MJ_OS_ANDROID
			if ( m_ModleGpuSync != EGL_NO_SYNC_KHR && m_ModleDisplay != EGL_NO_DISPLAY && eglClientWaitSyncKHR_)
			{
				bRet = (EGL_TIMEOUT_EXPIRED_KHR != eglClientWaitSyncKHR_(m_ModleDisplay, m_ModleGpuSync,
					EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, 0));
				if (bRet)
				{
					eglDestroySyncKHR_(m_ModleDisplay, m_ModleGpuSync);
					m_ModleGpuSync = EGL_NO_SYNC_KHR;
				}
			}
#endif
			if (bRet)
			{
				
				SetFrameWorkFlowState(RENDER_SUCCEED);
			}
			return bRet;
		}

		void RenderFrame::SetMultiThread(bool bMultiThread)
		{
			m_bMultiThread = bMultiThread;
		}
		bool RenderFrame::GetMultiThread()
		{
			return m_bMultiThread;
		}
	}
}
