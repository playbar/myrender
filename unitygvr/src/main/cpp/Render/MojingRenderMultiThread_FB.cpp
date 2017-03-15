#include "MojingRenderMultiThread_FB.h"
#include "Vsync.h"
#include "../Base/MojingTimer.h"
#define _CHECK_TIME_ 0
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
extern MojingLogger g_APIlogger;

namespace Baofeng
{
	namespace Mojing
	{
		MojingRenderMultiThread_FB::MojingRenderMultiThread_FB()
			:m_bSwapBuffer(false)
		{
		}


		MojingRenderMultiThread_FB::~MojingRenderMultiThread_FB()
		{
		}
		RenderFrame * MojingRenderMultiThread_FB::SelectDistortionFrame()
		{
			RenderFrame *pDistortionFrame = NULL;
			double  dDistionTime =  DISPLY_FRAME_TIME_IN_SEC;
			pDistortionFrame = m_FrameManager.GetCurrentDistortionFrame(m_ui64NextDisplayFrameIndex, m_dNextFrameDistortionTime);
			if (!pDistortionFrame)
			{
				pDistortionFrame = m_FrameManager.GetLastDisplayFrame();
			}


			if (pDistortionFrame == NULL)
			{
				return NULL;
			}
			return pDistortionFrame;
		}
		void MojingRenderMultiThread_FB::OnModifyed()
		{
			MojingRenderMultiThread::OnModifyed();
			SetSwapBuffer(false);
		}
		bool MojingRenderMultiThread_FB::InitVSyncParam()
		{
			return m_IntelVSync.OpenDRM();
		}
		bool MojingRenderMultiThread_FB::THREAD_DoDistortion()
		{
			RenderFrame * pDistortionFrame = SelectDistortionFrame();
			if (!pDistortionFrame)
				return false;
			SetLeftEyeTexID(pDistortionFrame->GetLeftEyeTexture().GetTextureID());
			SetRightEyeTexID(pDistortionFrame->GetRightEyeTexture().GetTextureID()); 
			EyeTextureType ETT[2] = { TEXTURE_RIGHT_EYE, TEXTURE_LEFT_EYE };

			double dDrawTimeSync = FramePointTimeInSeconds(m_ui64NextDisplayFrameIndex - 1) - DISPLY_FRAME_TIME_IN_SEC / 3;//m_dNextFrameDistortionTime;//fmax(m_dNextFrameDisplayTime, pDistortionFrame->GetPredictDisplayTime());
			
			double dDrawTime[2] = { dDrawTimeSync , dDrawTimeSync + DISPLY_FRAME_TIME_IN_SEC / 2 + 0.001 };
			double dShowTime[2] = { m_dNextFrameDisplayTime , m_dNextFrameDisplayTime + DISPLY_FRAME_TIME_IN_SEC / 2 };
			double dTime_DrawBegin, dSensorTime = 0, dGlFinish = 0;
#if _CHECK_TIME_
			double dDrawBegin = Timer::GetSeconds();
			static double dLast_DrawBegin = 0, dFirstFrameTime = 0;
			static uint64_t ui64FrameCount = 0;
			static double dCalculateTimeWarpMatrix, dDistortion, dDistortionGUI, dEglWaitClient, dEndOfEye = 0 ;
			static char sTime_DrawBegin[20], sDraw[20], sDisplay[20], sTimeSleep[20], sCalculateTimeWarpMatrix[20], sDistortion[20], sDistortionSensor[20], sDistortionGUI[20], sEglWaitClient[20], sEndOfEye[20], sTotle[20], sTotleAfterSleep[20];
			static char sSensorTime[20] = "--:--:--.---.---";
			static char sLastEyeTime[20] = "---.---";
			static char sFrameRate[20] = "000.00 / 000.00";

#endif
			const int O = 28000 * DISPLY_FRAME_RATE_60; // 14秒 撕裂条带从最左移动到最右
			double dDrawBefore = 0;// (GetVsyncState().vsyncCount % O) * (DISPLY_FRAME_TIME_IN_SEC / O);// 为消除撕裂误差
			
			m_IntelVSync.StartLatencyTiming();

			for (int iEye = 0; iEye < 2; iEye++)
			{
				// 注意：等于的情况只会发生在lastVSync = 0 的时候
#if 1 // 撕裂控制
				bool bNextIsVSync = m_IntelVSync.GetTimeToNextVSync() <= m_IntelVSync.GetTimeToNextHalfVSync();
				if (iEye == 0 && !bNextIsVSync)
				{
#if _CHECK_TIME_
					MOJING_WARN(g_APIlogger , "======= SKIP RIGHT EYE =======");
#endif
					continue;
				}
				else if (iEye == 1 && bNextIsVSync)
				{
#if _CHECK_TIME_
					MOJING_WARN(g_APIlogger, "======= SKIP LEFT EYE =======");
#endif
					continue;
				}
#endif // if 0 , 撕裂控制
				dTime_DrawBegin = Timer::GetSeconds();
				//SleepUntilTimePoint(dDrawTime[iEye] - dDrawBefore, (dDrawTime[iEye] - dDrawBefore - dTime_DrawBegin) < 0.004);
				// 3毫秒以内的睡眠直接用循环代替而不要再Sleep
				int iNextVSync = 0;
				
				if (iEye == 0)
				{
#if _CHECK_TIME_
					MOJING_TRACE(g_APIlogger, "NEXT VSync R = " << m_IntelVSync.GetTimeToNextVSync());
#endif
					m_IntelVSync.WaitForVSync();
				}
				else
				{
#if _CHECK_TIME_
					MOJING_TRACE(g_APIlogger, "NEXT VSync L = " << m_IntelVSync.GetTimeToNextHalfVSync());
#endif
					m_IntelVSync.WaitForHalfVSync();
				}
#if _CHECK_TIME_
				dCalculateTimeWarpMatrix = Timer::GetSeconds();
#endif
				if (m_bEnableTimeWarp)// 如果不启用预测的话，就不启用了吧
				{
					m_DistortionFixMatrix.SetIdentity();
					m_DistortionFixMatrix = pDistortionFrame->CalculateTimeWarpMatrix(dShowTime[iEye] + /*(1-iEye) * */DISPLY_FRAME_TIME_IN_SEC,
						&dSensorTime);

// 					char szDistortionFixMatrix[256];
// 					m_DistortionFixMatrix.ToString(szDistortionFixMatrix , 256);
// 					MOJING_TRACE(g_APIlogger , "DistortionFixMatrix = " << szDistortionFixMatrix);

#if _CHECK_TIME_
					strcpy(sSensorTime, Timer::FormatDoubleTime(dSensorTime));
#endif
				}
#if _CHECK_TIME_
				dDistortion = Timer::GetSeconds();
#endif
				DrawDistortionRange(ETT[iEye]);
				glFlush();
				DrawDistortion(pDistortionFrame, ETT[iEye]);
				glFlush();
#if _CHECK_TIME_
				dDistortionGUI = Timer::GetSeconds();
#endif
				DrawDistortionGUI(ETT[iEye]);
				glFlush();
				
#if _CHECK_TIME_
				dEglWaitClient = Timer::GetSeconds();
#endif

				eglWaitClient();
// #if _CHECK_TIME_
// 				dGlFinish = Timer::GetSeconds();
// #endif
				

#if _CHECK_TIME_		

				dEndOfEye = Timer::GetSeconds();
				strcpy(sTime_DrawBegin, Timer::FormatDoubleTime(dTime_DrawBegin));
				strcpy(sDraw, Timer::FormatDoubleTime(dDrawTime[iEye]));
				strcpy(sDisplay, Timer::FormatDoubleTime(dShowTime[iEye]));

				strcpy(sTimeSleep, Timer::FormatDoubleTimeInMS(dCalculateTimeWarpMatrix - dTime_DrawBegin));
				strcpy(sCalculateTimeWarpMatrix, Timer::FormatDoubleTimeInMS(dDistortion - dCalculateTimeWarpMatrix));
				strcpy(sDistortionSensor, Timer::FormatDoubleTimeInMS(dDistortion - dSensorTime));
				strcpy(sDistortion, Timer::FormatDoubleTimeInMS(dDistortionGUI - dDistortion));
				strcpy(sDistortionGUI, Timer::FormatDoubleTimeInMS(dEglWaitClient - dDistortionGUI));
				strcpy(sEglWaitClient, Timer::FormatDoubleTimeInMS(dEndOfEye - dEglWaitClient));
				strcpy(sTotleAfterSleep, Timer::FormatDoubleTimeInMS(dEndOfEye - dCalculateTimeWarpMatrix));
				strcpy(sTotle, Timer::FormatDoubleTimeInMS(dEndOfEye - dTime_DrawBegin));
				if (iEye == 0)
				{
					ui64FrameCount++;

					if (dLast_DrawBegin)
					{
						sprintf(sFrameRate, "%3.2f / %3.2f(Avg)",
							1 / (dTime_DrawBegin - dLast_DrawBegin),
							1 / ((dTime_DrawBegin - dFirstFrameTime) / (ui64FrameCount * 1.0)));
					}
					dLast_DrawBegin = dTime_DrawBegin;

					if (dFirstFrameTime == 0)
					{
						dFirstFrameTime = dTime_DrawBegin;
					}

				}
				MOJING_TRACE(g_APIlogger, "Eye = " << (iEye == 0 ? "Right" : "Left ") << \
					" , Frame = " << pDistortionFrame->GetFrameIndex() << " / " << m_ui64NextDisplayFrameIndex << \
					" , Now = " << sTime_DrawBegin << \
					" , Draw = " << sDraw << \
					" , Display = " << sDisplay << \
					" , Sensor = " << sSensorTime << \
					" , Sleep = " << sTimeSleep << \
					" , CalculateTimeWarpMatrix = " << sCalculateTimeWarpMatrix << \
					" , Distortion = " << sDistortion << " ( After Sensor : " << sDistortionSensor << " ) "\
					" , DistortionGUI = " << sDistortionGUI << \
					" , EglWaitClient = " << sEglWaitClient << \
					//" , GlFinish = " << sGlFinish << 
					" , Totle = " << sTotle << " (Draw) / " << sTotleAfterSleep << " (After Sleep) / " << (dSensorTime > 0 ? Timer::FormatDoubleTimeInMS(dEndOfEye - dSensorTime) : sSensorTime) << " (Sensor)" << \
					" , FPS = " << sFrameRate);
#endif
			}
			DrawCenterLine();
			AfterDisplay(pDistortionFrame);
			{
				if (!m_bSwapBuffer)
				{
					m_bSwapBuffer = true;
					EGLDisplay dpy = eglGetCurrentDisplay();
					EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
					eglSurfaceAttrib(dpy, surface, EGL_RENDER_BUFFER, EGL_SINGLE_BUFFER);
					eglSwapBuffers(dpy, surface);
				}
			}

			return true;
		}
		void MojingRenderMultiThread_FB::SetCenterLine(int iWidth, int colR, int colG, int colB, int colA)
		{
			MojingRenderBase::SetCenterLine(iWidth, 128, 0, 0, 255);
		}
		bool MojingRenderMultiThread_FB::AfterDisplay(RenderFrame *pDisplayFrame)
		{
			bool bRet = MojingRenderMultiThread::AfterDisplay(pDisplayFrame);
			m_dNextFrameDistortionTime = m_dNextFrameDisplayTime - DISPLY_FRAME_TIME_IN_SEC;
			return bRet ;
		}
	}
}
