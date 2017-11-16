#include <unistd.h>
#include <ctime>
#include "Vsync.h"
#include "MojingRenderMultithread.h"
#include "MojingRenderBase.h"
#include "../MojingAPI.h"
#include "../MojingManager.h"
#include "../Distortion/MojingDistortion.h"
#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDisplayParameters.h"
#include "../Base/MojingTypes.h"
#include "../Base/BuildType.h"
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
	{// 主线程
#if defined(MJ_OS_MAC)|| defined(MJ_OS_WIN32)
		extern int gettid(void);
#endif
		int MojingRenderMultiThread::Run()
		{
			SetDistortionThreadID(gettid());
			SetThreadName("Distortion Thd");

			m_ui64NextDisplayFrameIndex = 0;// 记录帧序号
			m_dNextFrameDisplayTime = 0;// 记录累计帧序号
			
//			m_pStartModleRender->Wait();
//			m_pStartModleRender->PulseEvent();
			// 初始化GL环境
			m_bInitThreadSucceeded = m_GLParam.InitDistortionParam() && THREAD_InitGLThread();
			m_pInitThread->SetEvent();// 设置初始化完成
			if (m_bInitThreadSucceeded)
			{
				m_pCurrentThread = this;
				// 启动帧管理器
				m_FrameManager.InitFrame();
				m_bExitThread = m_FrameManager.GetExitState();
				MOJING_TRACE(g_APIlogger, "Start Distion Loop.... , TID = " << gettid());
				// while ((!m_FrameManager.m_bEnableDistortoinRender) && (!m_bExitThread))
				{
					// Wait until first Model is Drawn.
					// MOJING_TRACE(g_APIlogger, "Wait for first Model...");
					usleep(1000);
				}
				while (!m_bExitThread)
				{
					// 渲染到屏幕的函数
					THREAD_DoDistortion();
					m_bExitThread = m_bExitThread && m_FrameManager.GetExitState();
				}
				MOJING_TRACE(g_APIlogger, "Exit Distion Thread");
			}
			else
			{
				MOJING_ERROR(g_APIlogger, "Exit Distion Thread , Init GL/EGL FAILD");
			}

			THREAD_ReleaseGLThread();
			return 0;

		}

		bool IsContextPriorityExtensionPresent()
		{
			EGLint currentPriorityLevel = -1;
			if (!eglQueryContext(eglGetCurrentDisplay(), eglGetCurrentContext(), EGL_CONTEXT_PRIORITY_LEVEL_IMG, &currentPriorityLevel)
				|| currentPriorityLevel == -1)
			{
				// If we can't report the priority, assume the extension isn't there
				return false;
			}
			return true;
		}

		// 初始GL化环境  CreateWarpContext
		bool MojingRenderMultiThread::THREAD_InitGLThread()
		{
			/*MOJING_FUNC_TRACE(g_APIlogger);
			EGLint contextAttribs[] =
			{
				EGL_CONTEXT_CLIENT_VERSION, m_GLParam.m_eglClientVersion,
				EGL_NONE, EGL_NONE,
				EGL_NONE
			};

			if (IsContextPriorityExtensionPresent())
			{
				contextAttribs[2] = EGL_CONTEXT_PRIORITY_LEVEL_IMG;
				contextAttribs[3] = EGL_CONTEXT_PRIORITY_HIGH_IMG;
			}
			m_GLParam.m_eglWarpContext = eglCreateContext(m_GLParam.m_eglDisplay,
				m_GLParam.m_eglConfig, 
				m_GLParam.m_eglShareContext, 
				contextAttribs);

			if (m_GLParam.m_eglWarpContext == EGL_NO_CONTEXT)
			{
				MOJING_ERROR(g_APIlogger, "eglCreateContext failed:" << EglErrorString());
				return false;
			}
			// 使环境绑定到当前线程
 			if (eglMakeCurrent(m_GLParam.m_eglDisplay, m_GLParam.m_eglWindowSurface,
 				m_GLParam.m_eglWindowSurface, m_GLParam.m_eglWarpContext) == EGL_FALSE)
			{
				eglDestroyContext(m_GLParam.m_eglDisplay, m_GLParam.m_eglWarpContext);
				MOJING_ERROR(g_APIlogger, "eglMakeCurrent failed:" << EglErrorString());
				return false;
			}
			*/
			return true;
		}
		void MojingRenderMultiThread::OnModifyed()
		{
			MojingRenderBase::OnModifyed();
			
// 			eglMakeCurrent(m_GLParam.m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
// 
// 			if (eglMakeCurrent(m_GLParam.m_eglDisplay, m_GLParam.m_eglWindowSurface,
// 				m_GLParam.m_eglWindowSurface, m_GLParam.m_eglWarpContext) == EGL_FALSE)
// 			{
// 				eglDestroyContext(m_GLParam.m_eglDisplay, m_GLParam.m_eglWarpContext);
// 				MOJING_ERROR(g_APIlogger, "eglMakeCurrent failed:" << EglErrorString());
// 				return ;
// 			}
		}
		// 释放GL环境
		bool MojingRenderMultiThread::THREAD_ReleaseGLThread()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			MOJING_TRACE(g_APIlogger, "Exit MojingRenderMultiThread " << GetDistortionThreadID());
			
			m_pInitThread->ResetEvent();
			m_GLParam.ReleaseDistortionParam();


			SetDistortionThreadID(0);

			// 切记这句话要放到最后
			if (m_pCurrentThread == this)
				m_pCurrentThread = NULL;
			return true;
		}
		// 按时间交换屏幕缓冲
		void MojingRenderMultiThread::THREAD_TimeSwap(RenderFrame * pDisplayFrame)
		{	
			//MOJING_FUNC_TRACE(g_APIlogger);
			//MOJING_TRACE(g_APIlogger, CURRENT_RENDER_TIME() << "THREAD_TimeSwap");

			// 预留5ms做swap
			static double dDisplayTime = 0;
			// glFinish();
			dDisplayTime = fmax(m_dNextFrameDisplayTime, pDisplayFrame->GetPredictDisplayTime()); 
			SleepUntilTimePoint(dDisplayTime - 0.005, true);
			
#ifdef TIMESWAP_DEBUG
			static double dCurrentTime = 0;
			static double dLastTime = 0;
			dCurrentTime = Timer::GetSeconds();
#endif
			
			if (!eglSwapBuffers(m_GLParam.m_eglDisplay, m_GLParam.m_eglWindowSurface))
			{
				MOJING_ERROR(g_APIlogger, " Call eglSwapBuffers Faild : " << EglErrorString() << " , Exit Distortion...." );
// 				m_FrameManager.SetExitState(true);
// 				m_bExitThread = true;
//				return ;
			}
#ifdef TIMESWAP_DEBUG
			static char TimeNow[20];
			static char PredictFrameTime[20];
			static char PredictDisplayTime[20];
			static char TimeOffset[20] = "??";

			
			strcpy(TimeNow, Timer::FormatDoubleTime(dCurrentTime));
			strcpy(PredictFrameTime, Timer::FormatDoubleTime(pDisplayFrame->GetPredictDisplayTime()));
			strcpy(PredictDisplayTime, Timer::FormatDoubleTime(m_dNextFrameDisplayTime));

			if (dLastTime > 1)
				strcpy(TimeOffset, Timer::FormatDoubleTime(dCurrentTime - dLastTime));

			static char szLog[512];
			if (pDisplayFrame->GetFrameIndex() != m_ui64NextDisplayFrameIndex)
				sprintf(szLog, "%lld/%lld, %s ==eglSwapBuffers==, Model Time = %s , Show Time = %s , Offset = %s",
					pDisplayFrame->GetFrameIndex(), m_ui64NextDisplayFrameIndex,
					TimeNow, PredictFrameTime, PredictDisplayTime, TimeOffset);
			else
				sprintf(szLog, "%lld, %s ==eglSwapBuffers==, Model Time = %s , Show Time = %s , Offset = %s",
					m_ui64NextDisplayFrameIndex,
					TimeNow, PredictFrameTime, PredictDisplayTime, TimeOffset);

			static UInt64 ui64FrameCount = 0;
			static double dStartDrawTime = Timer::GetSeconds();
			ui64FrameCount++;
			if (ui64FrameCount % 100 == 0)
			{
				double dDrawTime = Timer::GetSeconds();
				if (dStartDrawTime > 1)
				{
					char szInfo[128];
					sprintf(szInfo, "### FPS = %2.2f", 1.0 / ((dDrawTime - dStartDrawTime) / 100.0f));
					strcat(szLog , szInfo);
				}
				dStartDrawTime = dDrawTime;
			}



			MOJING_TRACE(g_APIlogger, szLog);
			dLastTime = dCurrentTime;
#endif
			AfterDisplay(pDisplayFrame);
		}

		// 由线程来调用的渲染到屏幕的函数
		bool MojingRenderMultiThread::THREAD_DoDistortion()
		{
			RenderFrame * pDistortionFrame = SelectDistortionFrame();
			if (!pDistortionFrame)
			{
				//MOJING_TRACE(g_APIlogger, "No DistortionFrame....");
				return false;
			}

			SleepUntilTimePoint(m_dNextFrameDistortionTime, true);
			if (m_bEnableTimeWarp)// 如果不启用预测的话，就不启用了吧 
			{
				m_DistortionFixMatrix.SetIdentity();
				// 考虑到swap的时间，再放大一帧的修正
				m_DistortionFixMatrix = pDistortionFrame->CalculateTimeWarpMatrix(m_dNextFrameDisplayTime + DISPLY_FRAME_TIME_AFTER_DRAW);
			}

			DrawDistortionRange(); 
			DrawDistortion(pDistortionFrame);
			DrawDistortionGUI();
			
			DrawCenterLine();
			m_FrameManager.SetEndOfDistortion(pDistortionFrame);

			// 按时间交换屏幕缓冲
			THREAD_TimeSwap(pDistortionFrame);
			return true;
		}


		RenderFrame * MojingRenderMultiThread::SelectDistortionFrame()
		{
			// 等待启动一帧绘制
			RenderFrame *pDistortionFrame = NULL;
			// 设置畸变的时间为5毫秒
			double  dDistionTime = 0.005;
			pDistortionFrame = m_FrameManager.GetCurrentDistortionFrame(m_ui64NextDisplayFrameIndex, m_dNextFrameDistortionTime);
			// 没有可以去畸变的，拿上一个显示时用的
			if (!pDistortionFrame)
			{
				pDistortionFrame = m_FrameManager.GetLastDisplayFrame();
#ifdef _DEBUG
				if (pDistortionFrame)
				{
					MOJING_ERROR(g_APIlogger, "Using last display frame " << pDistortionFrame->GetFrameIndex());
				}
#endif
			}


			if (pDistortionFrame == NULL)
			{
#ifdef _DEBUG
				MOJING_ERROR(g_APIlogger, "Can not find any frame to distortion....");
#endif
				return NULL;// 没有任何可以绘制的内容
			}
			// 畸变开始前，获取预测误差矩阵
			// m_DistortionFixMatrix = m_DistortionFixMatrix.RotationX((m_ui64NextDisplayFrameIndex %5) / 10);
// 			if (m_bEnableTimeWarp)// 如果不启用预测的话，就不启用了吧 
// 			{
// 				m_DistortionFixMatrix.SetIdentity();
// 				// 考虑到swap的时间，再放大一帧的修正
// 				m_DistortionFixMatrix = pDistortionFrame->CalculateTimeWarpMatrix(m_dNextFrameDisplayTime);
// 			}

			// 调用基类的畸变绘制函数
			static UInt32 LastFrame = 0;
			SetLeftEyeTexID(pDistortionFrame->GetLeftEyeTexture().GetTextureID());
			SetRightEyeTexID(pDistortionFrame->GetRightEyeTexture().GetTextureID());
			return pDistortionFrame;
		}
		bool MojingRenderMultiThread::AfterDisplay(RenderFrame *pDisplayFrame)
		{
			do
			{
				if (m_ui64NextDisplayFrameIndex + 1 >= pDisplayFrame->GetFrameIndex() + 1)
				{
					m_ui64NextDisplayFrameIndex++;
				}
				else
				{
					if (m_ui64NextDisplayFrameIndex == 0)
						m_ui64NextDisplayFrameIndex = pDisplayFrame->GetFrameIndex() + 1;// 第一帧
					else
						m_ui64NextDisplayFrameIndex = pDisplayFrame->GetFrameIndex();

				}

				uint64_t ui64NextDisplayFrameIndex = fmax(m_ui64NextDisplayFrameIndex, floorf(GetFractionalVsync()));
				double dNextFrameDisplayTime = FramePointTimeInSeconds(ui64NextDisplayFrameIndex);
				if (ui64NextDisplayFrameIndex > m_ui64NextDisplayFrameIndex &&
					(dNextFrameDisplayTime - Timer::GetSeconds()) > (0.8 * DISPLY_FRAME_TIME_IN_SEC))
				{/// 不需要修正的情况
					// MOJING_TRACE(g_APIlogger, CURRENT_RENDER_TIME() << "From vsync :" << ui64NextDisplayFrameIndex << " We count: " << m_ui64NextDisplayFrameIndex);
				}
				else
				{
					m_ui64NextDisplayFrameIndex = ui64NextDisplayFrameIndex;
				}
				m_dNextFrameDisplayTime = FramePointTimeInSeconds(m_ui64NextDisplayFrameIndex);
			} while (m_dNextFrameDisplayTime < Timer::GetSeconds());

			m_FrameManager.SetEndOfDisplay();
			m_dNextFrameDistortionTime = m_dNextFrameDisplayTime -0.005;
			// MOJING_TRACE(g_APIlogger, "End of AfterDisplay");
			return true;
		}
	}
}