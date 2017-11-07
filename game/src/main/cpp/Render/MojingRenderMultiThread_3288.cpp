#include "MojingRenderMultiThread_3288.h"
#include "Vsync.h"
#include "../Base/MojingTimer.h"
#include "../Base/GlUtils.h"
#define _CHECK_TIME_ 0
#include "../3rdPart/log4cplus/LogInterface.h"
// DrawDistortion
#include "../MojingSDKStatus.h"
#include "../MojingManager.h"
#include "../Distortion/MojingDistortion.h"
#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDisplayParameters.h"

extern MojingLogger g_APIlogger;
// MojingLogger g_3288logger("3288FrameRate");
namespace Baofeng
{
	namespace Mojing
	{
		
		MojingRenderMultiThread_3288::MojingRenderMultiThread_3288() :
			m_bSwapBuffer(false)
#if USING_OVERLAY			
			, m_pRedrawAIM(NULL)
			, m_pRedrawOSD(NULL)
#endif
		{
		}

		MojingRenderMultiThread_3288::~MojingRenderMultiThread_3288()
		{
		}

		RenderFrame * MojingRenderMultiThread_3288::SelectDistortionFrame()
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
#if USING_OVERLAY
			if (m_pRedrawAIM && m_RedrawAIMParmeat.GetChanged())
			{
				m_RedrawAIMParmeat.SetChanged(false);
				m_pRedrawAIM->SetLeftOverlayRect(m_RedrawAIMParmeat.GetLeftOverlayRect());
				m_pRedrawAIM->SetRightOverlayRect(m_RedrawAIMParmeat.GetRightOverlayRect());
				m_pRedrawAIM->SetLeftOverlayTextureID(m_RedrawAIMParmeat.GetLeftOverlayTextureID());
				m_pRedrawAIM->SetRightOverlayTextureID(m_RedrawAIMParmeat.GetRightOverlayTextureID());
				m_pRedrawAIM->SetMaxDrawFrameCount(m_RedrawAIMParmeat.GetDrawCount());
			}
			if (m_pRedrawOSD && m_RedrawOSDParmeat.GetChanged())
			{
				m_RedrawOSDParmeat.SetChanged(false);
				m_pRedrawOSD->SetLeftOverlayRect(m_RedrawOSDParmeat.GetLeftOverlayRect());
				m_pRedrawOSD->SetRightOverlayRect(m_RedrawOSDParmeat.GetRightOverlayRect());
				m_pRedrawOSD->SetLeftOverlayTextureID(m_RedrawOSDParmeat.GetLeftOverlayTextureID());
				m_pRedrawOSD->SetRightOverlayTextureID(m_RedrawOSDParmeat.GetRightOverlayTextureID());
				m_pRedrawOSD->SetMaxDrawFrameCount(m_RedrawOSDParmeat.GetDrawCount());
			}
#endif
			return pDistortionFrame;
		}
		void MojingRenderMultiThread_3288::OnModifyed()
		{
			MojingRenderMultiThread::OnModifyed();
			SetSwapBuffer(false);
		}
		bool MojingRenderMultiThread_3288::InitVSyncParam()
		{
			return m_3288VSync.OpenVSync();
		}

		void MojingRenderMultiThread_3288::SetEyeTexID(int leftEye, int rightEye)
		{
			RenderFrame *pFR;
			if (NULL != (pFR = m_FrameManager.GetCurrentModleFrame()))
			{// 注意：这里要处理来自外部的Eye Texture
				if (HasEXT_multiview_ovr)
				{
					pFR->SetMultiviewTexture(leftEye);
				}
				else
				{
					pFR->GetLeftEyeTexture().UsingTextureID(leftEye);
					pFR->GetRightEyeTexture().UsingTextureID(rightEye);
				}
				
			}
		}

		void MojingRenderMultiThread_3288::InstallShader()
		{
			m_bShowTimeWapRange = true;
			Distortion *pDistortion = Manager::GetMojingManager()->GetDistortion();
			// 释放掉已经申请和分配的程序/Shader对象
			// 主场场景
			if (!glIsProgram(m_warpProg_NoDispersion.Prog.program))
				m_warpProg_NoDispersion = BuildWarpProg(VertexShader, FragmentShader); // TODO: FragmentShaderNoDispersion

			if (!glIsProgram(m_warpProg_Dispersion.Prog.program))
				m_warpProg_Dispersion = BuildWarpProg(VertexShader, FragmentShader);
			// Layout
			if (!glIsProgram(m_warpProgLayout_NoDispersion.Prog.program))
				m_warpProgLayout_NoDispersion = BuildWarpProg(VertexShaderLayout, FragmentShaderLayoutNoDispersion);

			if (!glIsProgram(m_warpProgLayout_Dispersion.Prog.program))
				m_warpProgLayout_Dispersion = BuildWarpProg(VertexShaderLayout, FragmentShaderLayout);

			if (!glIsProgram(m_warpProgDistortionRange.Prog.program))
			{
				m_warpProgDistortionRange = BuildWarpProg(VertexShaderDistortionRange, m_bShowTimeWapRange ? FragmentShaderDistortionRange : FragmentShaderDistortionRangeDark);
			}
			// Multiview
#ifdef USE_GLES3
			if (HasEXT_multiview_ovr)
			{
				if (!glIsProgram(m_warpProg_Multiview_NoDispersion.Prog.program))
				{
					m_warpProg_Multiview_NoDispersion = BuildWarpProg(VertexShaderMultiviewNoDispertion, FragmentShaderMultiviewNoDispertion);
				}
				if (!glIsProgram(m_warpProg_MultiviewHighRes_NoDispersion.Prog.program))
				{
					m_warpProg_MultiviewHighRes_NoDispersion = BuildWarpProg(VertexShaderMultiviewHighResNoDispertion, FragmentShaderMultiviewHighResNoDispertion);
				}
			}
#endif
			if (pDistortion->GetNoDispersion())
			{// 使用无色散Shader
				MOJING_TRACE(g_APIlogger, "MojingRenderBase::InstallShader ==> No Dispersion Shader......");
				// 是否使用 OVR_Multiview 拓展
				if (HasEXT_multiview_ovr)
				{
					m_warpProg = m_warpProg_Multiview_NoDispersion;
				}
				else
				{
					m_warpProg = m_warpProg_NoDispersion;//BuildWarpProg(VertexShader, FragmentShaderNoDispersion);
				}
				m_warpProgLayout = m_warpProgLayout_NoDispersion;// BuildWarpProg(VertexShaderLayout, FragmentShaderLayoutNoDispersion);
			}
			else
			{// 使用有色散Shader
				MOJING_TRACE(g_APIlogger, "MojingRenderBase::InstallShader ==> Dispersion Shader......");

				m_warpProg = m_warpProg_Dispersion;// BuildWarpProg(VertexShader, FragmentShader);
				m_warpProgLayout = m_warpProgLayout_Dispersion;// BuildWarpProg(VertexShaderLayout, FragmentShaderLayout);
			}
		}

		/* Textured quad geometry */
		float texturedQuadCoordinates[] =
		{
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,

			-1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f
		};

		/* Textured quad low resolution texture coordinates */
		float texturedQuadLowResTexCoordinates[] =
		{
			0, 0,
			1, 0,
			1, 1,

			0, 0,
			1, 1,
			0, 1
		};

		/* Textured quad high resolution texture coordinates */
		float texturedQuadHighResTexCoordinates[] =
		{
			-0.5, -0.5,
			1.5, -0.5,
			1.5, 1.5,

			-0.5, -0.5,
			1.5, 1.5,
			-0.5, 1.5
		};

		void MojingRenderMultiThread_3288::BindEyeTextures(GLuint TexId)
		{ 
			if (HasEXT_multiview_ovr)
			{
#ifdef GL_TEXTURE_2D_ARRAY
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D_ARRAY, TexId);

				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				WarpProg warpProg = m_warpProg;
				GlProgram prog = warpProg.Prog;

				glUniform1i(prog.uTexture0, GL_TEXTURE0 - GL_TEXTURE0);
#endif
			}
			else
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, TexId);


				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				WarpProg warpProg = m_warpProg;
				GlProgram prog = warpProg.Prog;

				glUniform1i(prog.uTexture0, GL_TEXTURE0 - GL_TEXTURE0);
			}
		}

		bool MojingRenderMultiThread_3288::DrawDistortion_Multiview(RenderFrame *pDistortionFrame, EyeTextureType DrawEyes)
		{
			bool bRet = true;

			GLuint MultiviewTexId = pDistortionFrame->GetMultiviewTexture();

			MojingDisplayParameters *pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
			if (!m_warpMesh->IsAvailable() || g_bIsModifyed)
			{
				OnModifyed(); // create warpmesh
			}

			// return true;
			/*
			注意：	以下M44矩阵将在Shader中与表示[-1,1]区间的色散畸变颜色分量系数V4(x,y,-1,1)相乘
			即M44 * V4 ，得到原始图的颜色采样坐标
			*/
			const Matrix4f SingleEye(
				0.5, 0.0, 0.0, 0,				// 这是第一列
				0.0, 0.5, 0.0, 0,
				-0.5, -0.5, -1.0, 1,
				0.0, 0.0, 0.0, 0					// 这是第四列
				);

			const Matrix4f LeftEye(
				0.5, 0, 0, 0,
				0, 0.5, 0, 0,
				-0.5, -0.5, -1, -1,
				0, 0, 0, 0
				);
			const Matrix4f RightEye(
				0.5, 0, 0, 0,
				0, 0.5, 0, 0,
				-0.5, -0.5, -1, -1,
				0, 0, 0, 0
				);
			GLuint TexId[2];
			Matrix4f timeWarps[2][2];
			bool bUseClamp = false;

			bool bIsTextureAvaliable = (MultiviewTexId != 0 && glIsTexture(MultiviewTexId));

			EyeTextureType ET = TEXTURE_LEFT_EYE;

			if (bIsTextureAvaliable)
			{
				TexId[0] = MultiviewTexId;
				TexId[1] = MultiviewTexId;

				timeWarps[0][0] = timeWarps[0][1] = LeftEye;
				timeWarps[1][0] = timeWarps[1][1] = RightEye;

			}

#ifndef MJ_OS_WIN32
			SetWarpState(m_bShowTimeWapRange);
#else
			SetWarpState(TRUE);
#endif // !MJ_OS_WIN32
#define DRAW_COLOR_BLOCK 0
			for (int eye = 0; eye < 2; eye++)
			{

				BindWarpProgram(m_warpProg, timeWarps[eye][0], eye, bUseClamp);
				BindEyeTextures(TexId[eye]);

				/*
				* Upload uniforms. The layerIndex is used to choose what layer of the array texture to sample from.
				* The shader will use the given layerIndex and layerIndex + 2, where layerIndex gives the layer with
				* the wide field of view, where the entire scene has been rendered, and layerIndex + 2 gives the layer
				* with the narrow field of view, where only the center of the scene has been rendered.
				*/

				{
					glUniform1i(m_warpProg.Prog.uLayerIndex, eye);
				}

				m_warpMesh->BindVBO();
				m_warpMesh->DrawElements(eye);
			}

			UnbindEyeTextures();

			glUseProgram(0);
			m_warpMesh->UnBindVBO();
			glDisable(GL_SCISSOR_TEST);

			return bRet;
		}

		bool MojingRenderMultiThread_3288::THREAD_InitGLThread()
		{
			if (MojingRenderMultiThread::THREAD_InitGLThread())
			{
#if USING_OVERLAY
				if (m_pRedrawOSD == NULL)
				{
					m_pRedrawOSD = new RenderOverlay3288;
					m_pRedrawOSD->SetStartParmat("OSD Layer", 1920, 1080, 0x40000000, m_GLParam.m_eglShareContext, m_GLParam.m_eglClientVersion);
					m_pRedrawOSD->Start();
				}
				if (m_pRedrawAIM == NULL)
				{
					m_pRedrawAIM = new RenderOverlay3288;
					m_pRedrawAIM->SetStartParmat("AIM Layer", 1440, 256, 0x41000000, m_GLParam.m_eglShareContext, m_GLParam.m_eglClientVersion);
					m_pRedrawAIM->Start();
				}

				if (m_pRedrawOSD)
				{
					m_pRedrawOSD->WaitInitFinsh();
					if (!m_pRedrawOSD->GetInitSucceeded())
					{
						delete m_pRedrawOSD;
						m_pRedrawOSD = NULL;
					}
				}

				if (m_pRedrawAIM)
				{
					m_pRedrawAIM->WaitInitFinsh();
					if (!m_pRedrawAIM->GetInitSucceeded())
					{
						delete m_pRedrawAIM;
						m_pRedrawAIM = NULL;
					}
				}
#endif 
				return true; //m_pRedrawAIM != NULL && m_pRedrawOSD != NULL;
			}
			return false;
		}

		bool MojingRenderMultiThread_3288::THREAD_DoDistortion()
		{
			RenderFrame * pDistortionFrame = SelectDistortionFrame();
			if (!pDistortionFrame)
				return false;
			SetLeftEyeTexID(pDistortionFrame->GetLeftEyeTexture().GetTextureID());
			SetRightEyeTexID(pDistortionFrame->GetRightEyeTexture().GetTextureID()); 
			EyeTextureType ETT[2] = {  TEXTURE_RIGHT_EYE , TEXTURE_LEFT_EYE};

			double dDrawTimeSync = FramePointTimeInSeconds(m_ui64NextDisplayFrameIndex - 1) - DISPLY_FRAME_TIME_IN_SEC / 3;//m_dNextFrameDistortionTime;//fmax(m_dNextFrameDisplayTime, pDistortionFrame->GetPredictDisplayTime());
			

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
			
			static double LastVSync = 0;
			
			

			EGLDisplay dpy = eglGetCurrentDisplay();
			EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
			
			
			double dCurrentVSync = m_3288VSync.WaitNextVSync();
			/*开始通知Overlay绘制*/
#if USING_OVERLAY
			if (m_pRedrawAIM && m_pRedrawAIM->GetDrawFrameCount())
			{
				m_pRedrawAIM->SetVsync(true);
			}
			if (m_pRedrawOSD && m_pRedrawOSD->GetDrawFrameCount())
			{
				m_pRedrawOSD->SetVsync(true);
			}
#endif
			double dAfterVSync = Timer::GetSeconds();
			MOJING_TRACE(g_APIlogger, "Vsync  = "  <<
				Timer::FormatDoubleTime(dAfterVSync));
			double dDrawTime[2] = { dCurrentVSync, dCurrentVSync + DISPLY_FRAME_TIME_IN_SEC / 2 };
			double dShowTime[2] = { dCurrentVSync + 3 * DISPLY_FRAME_TIME_IN_SEC, dCurrentVSync + DISPLY_FRAME_TIME_IN_SEC / 2 + 3 * DISPLY_FRAME_TIME_IN_SEC };
			for (int iEye = 0; iEye < 2; iEye++)
			{
#if _CHECK_TIME_
				dTime_DrawBegin = Timer::GetSeconds();
#endif
#ifdef _DEBUG
				glClearColor(1-iEye , iEye , 0 ,1);
#endif
				if (iEye)
				{
					if (Timer::GetSeconds() > dDrawTime[iEye])
						continue;// 超时，不画了
				}
				SleepUntilTimePoint(dDrawTime[iEye], true);



				MOJING_TRACE(g_APIlogger, "Eye Begin = " << (iEye == 0 ? "Right" : "Left ") << Timer::FormatDoubleTime(Timer::GetSeconds()));

#if _CHECK_TIME_
				dCalculateTimeWarpMatrix = Timer::GetSeconds();
#endif				
				if (m_bEnableTimeWarp)// 如果不启用预测的话，就不启用了吧
				{
					m_DistortionFixMatrix.SetIdentity();
					// 显示时间为绘制时间+3帧
					m_DistortionFixMatrix = pDistortionFrame->CalculateTimeWarpMatrix(dShowTime[iEye],&dSensorTime);
#if _CHECK_TIME_
					strcpy(sSensorTime, Timer::FormatDoubleTime(dSensorTime));
#endif
				}
#if _CHECK_TIME_
				dDistortion = Timer::GetSeconds();
#endif
				// TODO: 不画的话会有撕裂
 				//DrawDistortionRange(ETT[iEye]);
 				//glFlush();

				if (HasEXT_multiview_ovr)
				{
					DrawDistortion_Multiview(pDistortionFrame, ETT[iEye]);
				}
				else
				{
					DrawDistortion(pDistortionFrame, ETT[iEye]);
				}
				
				//glFlush();
#if _CHECK_TIME_
				dDistortionGUI = Timer::GetSeconds();
#endif
 				//DrawDistortionGUI(ETT[iEye]);
 				//glFlush();

				
#if _CHECK_TIME_
				dEglWaitClient = Timer::GetSeconds();
#endif
				if (0 == iEye)
					glFinish();
				MOJING_TRACE(g_APIlogger, "Eye Finsh = " << (iEye == 0 ? "Right" : "Left ") << Timer::FormatDoubleTime(Timer::GetSeconds()));

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
					" , FPS = " << sFrameRate << \
					", After Vsync = " << Timer::FormatDoubleTimeInMS(dEndOfEye - dAfterVSync));


				static double dLastFrameRate = 0;
				if (dLast_DrawBegin - dLastFrameRate >= 60)
				{
					dLastFrameRate = dLast_DrawBegin;
					MOJING_TRACE(g_3288logger,"FrameRate = " << sFrameRate);
				}
#endif
				
			}// end of for
			//SleepUntilTimePoint(Timer::GetSeconds() + 16.0f/1000.0f);

 			DrawCenterLine();
			AfterDisplay(pDistortionFrame);
			eglSwapBuffers(dpy, surface);
			return true;
		}
		void MojingRenderMultiThread_3288::SetCenterLine(int iWidth, int colR, int colG, int colB, int colA)
		{
			MojingRenderBase::SetCenterLine(iWidth, 128, 0, 128, 255);
		}
		bool MojingRenderMultiThread_3288::AfterDisplay(RenderFrame *pDisplayFrame)
		{
			bool bRet = MojingRenderMultiThread::AfterDisplay(pDisplayFrame);
			m_dNextFrameDistortionTime = m_dNextFrameDisplayTime - DISPLY_FRAME_TIME_IN_SEC;
			return bRet ;
		}
	}
}
