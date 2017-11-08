#include "MojingRenderBase.h"
#include "../MojingAPI.h"
#include "../MojingSDKStatus.h"
#include "../MojingManager.h"
#include "../Platform/MojingPlatformBase.h"
#include "../Distortion/MojingDistortion.h"
#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDisplayParameters.h"
#include "../Profile/DebugSettingsProfile.h"

#include "../Base/MojingTypes.h"
#include "../Base/BuildType.h"
#include "../Base/MojingTimer.h"
#include "VerifyFailedInfo.h"
#include "VersionLow.h"
#include "Vsync.h"
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif

#include <unistd.h>
#include <map>
#include <string>
#include <ctime>

#ifdef MJ_OS_ANDROID
#include "MojingRenderMultithread.h"
#include "MojingRenderMultiThread_FB.h"

#include "MojingRenderMultiThread_3288.h"

#elif defined(MJ_OS_WIN32)
#include <Windows.h>
#elif defined(MJ_OS_MAC)
#include <sys/syscall.h>
#endif

#if MOTION_TO_PHOTON_TEST
#include "../Base/MojingLog.h"
#endif

/*#define  PI 3.14159265358979323846*/

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

namespace Baofeng
{
	namespace Mojing
	{
		class GLShaderUtils;
		Mutex *g_pmtMojingRenderBaseMap;

		std::map<int, MojingRenderBase *> g_MojingRenderBaseMap;

#if defined(MJ_OS_MAC)
		int gettid(void)
		{
			return  (int)  syscall(SYS_thread_selfid);
		}
#elif defined(MJ_OS_WIN32)
		int gettid(void)
		{
			//pthread_t pt = pthread_self();
			//return (int)pt.x;
			return (int)::GetCurrentThreadId();
		}
#endif

		bool MojingRenderBase::g_bIsModifyed = false;
		MojingRenderBase::MojingRenderBase()
			:m_CellsImageTexID(0),
			m_iLeftEyeTexID(0),
			m_iRightEyeTexID(0),
			m_fCenterLineWidth(0),
			m_ui64NextDisplayFrameIndex(0),
			m_dNextFrameDisplayTime(0), 
			m_dNextFrameDistortionTime(0),
			m_iLeftOverlayTextureID(0),
			m_iRightOverlayTextureID(0),
			m_v4LeftOverlayRect(0.0f, 0.0f, 0.0f, 0.0f),
			m_v4RightOverlayRect(0.0f, 0.0f, 0.0f, 0.0f),
			m_HasEXT_sRGB_write_control(false),
			m_bShowTimeWapRange(false)
		{
			m_warpMesh = new GlGeometryTriangles();		// 使用独立三角形的方式绘制
			//			m_warpMesh = new GlGeometryTriangleStrip();	// 使用三角形条带的方式绘制
			m_warpLayer = new GlGeometryQuad();//
			SetRenderThreadID(gettid());
			SetDistortionThreadID(gettid());

			// [-1 ， 1] 空间内的中线，即 直线F(X)  X=0
// 			m_LineDraw.AddVertex(0, 1);
// 			m_LineDraw.AddVertex(0, -1);
			m_bShowTimeWapRange = Manager::GetMojingManager()->GetParameters()->GetDebugSettingsProfile()->GetShowTimeWapRange();


#ifdef MJ_OS_ANDROID
			m_HasEXT_sRGB_write_control = ExtensionStringPresent("GL_EXT_sRGB_write_control",
				(const char *)glGetString(GL_EXTENSIONS));
#endif
		}

		MojingRenderBase::~MojingRenderBase()
		{
			//MOJING_FUNC_TRACE(g_APIlogger);
			MOJING_TRACE(g_APIlogger, "Delete MojingRenderBase , RenderThreadID = " << GetRenderThreadID());
			if (GetRenderThreadID() == gettid())
			{
				clearShaderIds();
				clearProgramIds();
				clearEyeTextures();
			}
			else
			{
				MOJING_ERROR(g_APIlogger, "Release MojingRenderBase an free Open GL ES Object in an other thread! Render thread = " << GetRenderThreadID() << " Current Thread ID = " << gettid());
			}
			delete m_warpMesh;

			if (m_warpLayer)
			{
				delete m_warpLayer;
				m_warpLayer = NULL;
			}


		}
		void MojingRenderBase::OnModifyed()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			MojingDisplayParameters *pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
#if AUTO_LANDSCAPE
			m_bLandscape = pDisplayParameters->GetScreenWidth() > pDisplayParameters->GetScreenHeight();
			m_iScreenWidth = pDisplayParameters->GetScreenWidth();
			m_iScreenHeight = pDisplayParameters->GetScreenHeight();
#else
			m_iScreenWidth = fmax(pDisplayParameters->GetScreenWidth(), pDisplayParameters->GetScreenHeight());
			m_iScreenHeight = fmin(pDisplayParameters->GetScreenWidth(), pDisplayParameters->GetScreenHeight());
			m_bLandscape = true;
#endif

			m_iScreenWidth_Half = m_iScreenWidth / 2;
			m_iScreenHeight_Half = m_iScreenHeight / 2;

			LoadMeshFromMemory();
			m_warpLayer->BuildGeometry();
			
			InstallShader();
			g_bIsModifyed = false;
		}
		void MojingRenderBase::CreateCurrentRender(bool bEnableMultiThread, bool bEnableTimeWarp)
		{
			if (NULL == g_pmtMojingRenderBaseMap)
			{
				g_pmtMojingRenderBaseMap = new Mutex;
			}
			g_pmtMojingRenderBaseMap->DoLock();
			int iThreadID = gettid();

			std::map<int, MojingRenderBase *>::iterator itFind = g_MojingRenderBaseMap.find(iThreadID);
			if (itFind == g_MojingRenderBaseMap.end()) // Create new thread
			{
#ifdef MJ_OS_ANDROID // for Android
				// 如果可以创建出多线程渲染环境 则使用默认的线程来渲染， 否则启用多线程来进行渲染
				MojingRenderMultiThread *pRenderMultiThread = NULL;
				if (bEnableMultiThread)
				{
					MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
					MachineListNode CurrentMachineType = pDeviceParameters->GetCurrentMachine();
					if (CurrentMachineType.m_iRender == 1)
					{
						if (MojingRenderMultiThread_3288::InitRenderMultiThreadGLParam())
						{
							pRenderMultiThread = new MojingRenderMultiThread_3288;
							if (((MojingRenderMultiThread_3288*)pRenderMultiThread)->InitVSyncParam())
							{
								MOJING_TRACE(g_APIlogger, "Using RK3288 FBR");
							}
							else
							{
								MOJING_ERROR(g_APIlogger, "Init RK3288 VSyncParam FAILD!!!");
								delete pRenderMultiThread;
								pRenderMultiThread = NULL;
							}
						}
					}
					if (pRenderMultiThread == NULL)
					{// 不是一体机或者不是需要特殊畸变支持的一体机,或者一体机模式下启动特定的渲染类失败
						MOJING_TRACE(g_APIlogger, "Using MT Render");
						if (MojingRenderMultiThread::InitRenderMultiThreadGLParam())
						{
							pRenderMultiThread = new MojingRenderMultiThread;
						}

					}
				}

				if (pRenderMultiThread)
				{
					pRenderMultiThread->Start();
				}

				if (pRenderMultiThread && pRenderMultiThread->GetInitThread())
				{
					g_MojingRenderBaseMap[iThreadID] = pRenderMultiThread;
					RenderFrame::SetMultiThread(true);
					MOJING_TRACE(g_APIlogger, "Create Multi-Thread-Render at thread : " << iThreadID << "/" << pRenderMultiThread->GetThreadId());
				}
				else
				{
					if (pRenderMultiThread)
						delete pRenderMultiThread;

					MOJING_TRACE(g_APIlogger, "Create render at thread : " << iThreadID);
					g_MojingRenderBaseMap[iThreadID] = new MojingRenderBase();
					RenderFrame::SetMultiThread(false);
					// g_MojingRenderBaseMap[iThreadID]->m_FrameManager.ResetFrame();
				}

#else
				MOJING_TRACE(g_APIlogger, "Create render at thread : " << iThreadID);
				g_MojingRenderBaseMap[iThreadID] = new MojingRenderBase();
				RenderFrame::SetMultiThread(false);
#endif
				g_MojingRenderBaseMap[iThreadID]->SetEnableTimeWarp(bEnableTimeWarp);
				g_MojingRenderBaseMap[iThreadID]->InstallShader();

			}
			else // Thread already exists
			{
#ifdef MJ_OS_ANDROID
				if (itFind->second->GetDistortionThreadID() != itFind->second->GetRenderThreadID())
				{// 多线程模型
					if (itFind->second->GetDistortionThreadID() == 0)
					{// 多线程的停止状态
						MojingRenderMultiThread *pMojingRenderMultiThread = (MojingRenderMultiThread*)itFind->second;
						pMojingRenderMultiThread->Start();
						if (pMojingRenderMultiThread->GetInitThread())
						{
							return;
						}
					}
				}
#endif
				MOJING_WARN(g_APIlogger, "Already has a render on thread : " << iThreadID);
			}
			g_pmtMojingRenderBaseMap->Unlock();
		}

		MojingRenderBase *MojingRenderBase::GetCurrentRender()
		{
			return GetRender(gettid());
		}

		MojingRenderBase *MojingRenderBase::GetRender(int iID)
		{
			MojingRenderBase *pRet = NULL;
			if (g_pmtMojingRenderBaseMap)
			{
				g_pmtMojingRenderBaseMap->DoLock();
				std::map<int, MojingRenderBase *>::iterator it = g_MojingRenderBaseMap.find(iID);
				if (it != g_MojingRenderBaseMap.end())
				{
					pRet = it->second;
					// 这里处理无效的畸变线程
					if (pRet->GetRenderThreadID() == 0 || pRet->GetDistortionThreadID() == 0)
					{
                        MOJING_ERROR(g_APIlogger, "Invalid render object! , RenderThreadID = " << pRet->GetRenderThreadID() << " , DistortionThreadID = " << pRet->GetDistortionThreadID());
						delete pRet;
						g_MojingRenderBaseMap.erase(it);
						pRet = NULL;
					}
				}
				g_pmtMojingRenderBaseMap->Unlock();
			}

			return pRet;
		}

		void MojingRenderBase::ReleaseRender(int iID)
		{
			if (g_pmtMojingRenderBaseMap)
			{
				g_pmtMojingRenderBaseMap->DoLock();
				std::map<int, MojingRenderBase *>::iterator it = g_MojingRenderBaseMap.find(iID);
				if (it != g_MojingRenderBaseMap.end())
				{				
					delete it->second;
				}
				g_MojingRenderBaseMap.erase(it);
				g_pmtMojingRenderBaseMap->Unlock();
			}
		}

		void MojingRenderBase::ReleaseCurrentRender()
		{
			ReleaseRender(gettid());
		}


		GLuint MojingRenderBase::GetEyeTextureId(EyeTextureType ett,
			unsigned int &iWidth, unsigned int &iHeight, GLenum &format, RenderFrame *pRF)
		{
			if (Manager::GetMojingManager()->GetDistortion()->IsDistortionNeeded())
			{
				iWidth = iHeight = MojingSDK_GetTextureSize();
			}
			else
			{
				iWidth = iHeight = MojingSDK_GetTextureSize();
			}

			if (ett == TEXTURE_BOTH_EYE)
				iWidth *= 2;// 双眼缓冲宽度增加一倍
			format = GL_RGBA;
			/*程序执行到这里，意味着得到了一个可以交给上层去绘制场景的纹理（m_ProducerIndex序号）*/
			// 如果当前可用于生产的纹理还没有建立，那么建立之

			CEyeTextureParam &EYE = ett == TEXTURE_LEFT_EYE ? pRF->GetLeftEyeTexture() : pRF->GetRightEyeTexture();

			if (!EYE.CheckMatch(iWidth, iHeight, format))
			{
				EYE.Create(iWidth, iHeight, format);
			}

			return EYE.GetTextureID();
		}
		GLuint MojingRenderBase::GetEyeTextureId(EyeTextureType ett, unsigned int &iWidth,
			unsigned int &iHeight, GLenum &format)
		{
			GLuint Ret = GetEyeTextureId(ett, iWidth, iHeight, format, &m_RenderFrame);
			FrameWorkFlowState WS = m_RenderFrame.GetFrameWorkFlowState();
			if (WS == DISPLAY_END || WS == UNINIT)
			{
				while (!m_RenderFrame.BeforModle(m_ui64NextDisplayFrameIndex))
					m_ui64NextDisplayFrameIndex++;
			}
			FrameWorkFlowState NS = RENDER_MODLE_BOTH;
			if (ett == TEXTURE_LEFT_EYE)
				NS = RENDER_MODLE_LEFT;
			else if (ett == TEXTURE_RIGHT_EYE)
				NS = RENDER_MODLE_RIGHT;
			m_RenderFrame.SetFrameWorkFlowState(NS);
			return Ret;
		}

		void MojingRenderBase::SetEyeTexID(int leftEye, int rightEye, RenderFrame *pRF)
		{
			pRF->GetLeftEyeTexture().UsingTextureID(leftEye);
			pRF->GetRightEyeTexture().UsingTextureID(rightEye);

		}
		void MojingRenderBase::UnityATW_SetEyeTexID(int leftEye, int rightEye, float *pViewMatrix, RenderFrame *pRF)
		{
			pRF->GetLeftEyeTexture().UsingTextureID(leftEye);
			pRF->GetRightEyeTexture().UsingTextureID(rightEye);
			Matrix4f m4fViewMatrix;
			memcpy(m4fViewMatrix.M[0], pViewMatrix, sizeof(float)* 16);
			Quatf QF(m4fViewMatrix);

// 			char szBuffer[256];
// 			*szBuffer = 0;
// 			for (int i = 0; i < 16; i++)
// 				sprintf(szBuffer + strlen(szBuffer) , "%g ", pViewMatrix[i]);
// 			MOJING_TRACE(g_APIlogger, "pViewMatrix = " << szBuffer);
// 
// 			m4fViewMatrix.ToString(szBuffer, 256);
// 			MOJING_TRACE(g_APIlogger, "mViewMatrix = " << szBuffer);
// 
// 			MOJING_TRACE(g_APIlogger , "SetPredictView = " << QF.x << " , " << QF.y << " , "<< QF.z << " , " << QF .w);

			pRF->SetPredictView(QF);
		}
		void MojingRenderBase::SetOverlayTextureID(int leftOverlayEye, int rightOverlayEye)
		{
			SetLeftOverlayTextureID(leftOverlayEye);
			SetRightOverlayTextureID(rightOverlayEye);
		}

		void MojingRenderBase::SetEyeTexID(int leftEye, int rightEye)
		{
			FrameWorkFlowState WS = m_RenderFrame.GetFrameWorkFlowState();
			if (WS == DISPLAY_END || WS == UNINIT)
			{
				while (!m_RenderFrame.BeforModle(m_ui64NextDisplayFrameIndex))
					m_ui64NextDisplayFrameIndex++;

				m_RenderFrame.SetFrameWorkFlowState(INIT);
			}

			SetEyeTexID(leftEye, rightEye, &m_RenderFrame);
			SetLeftEyeTexID(leftEye);
			SetRightEyeTexID(rightEye);
			m_RenderFrame.SetFrameWorkFlowState(RENDER_SUCCEED);

		}
		void MojingRenderBase::UnityATW_SetEyeTexID(int leftEye, int rightEye, float *pViewMatrix)
		{
#ifdef _DEBUG
			MOJING_FUNC_TRACE(g_APIlogger);
#endif
			FrameWorkFlowState WS = m_RenderFrame.GetFrameWorkFlowState();
			if (WS == DISPLAY_END || WS == UNINIT)
			{
				while (!m_RenderFrame.BeforModle(m_ui64NextDisplayFrameIndex))
					m_ui64NextDisplayFrameIndex++;

				m_RenderFrame.SetFrameWorkFlowState(INIT);
			}

			UnityATW_SetEyeTexID(leftEye, rightEye, pViewMatrix, &m_RenderFrame);
			SetLeftEyeTexID(leftEye);
			SetRightEyeTexID(rightEye);
			m_RenderFrame.SetFrameWorkFlowState(RENDER_SUCCEED);
		}
		RenderFrame * MojingRenderBase::SelectDistortionFrame()
		{
			m_RenderFrame.SetFrameWorkFlowState(DISTION_BEGIN);
			return &m_RenderFrame;
		}

		void MojingRenderBase::DoMtpTest(EyeTextureType eyeType)
		{
			if (!m_warpMesh->IsAvailable() || g_bIsModifyed)
			{
				OnModifyed();
			}

			Quatf diffRotation = Quatf(m_DistortionFixMatrix);
			diffRotation.w = 0;
			float gMotionToPhotonW = diffRotation.Length();
			Recti rcScreen = Recti(0, 0, m_iScreenWidth, m_iScreenHeight);

			if (eyeType & TEXTURE_LEFT_EYE)
			{
				rcScreen.w /= 2;
			}
			else if (eyeType & TEXTURE_RIGHT_EYE)
			{
				rcScreen.w /= 2;
				rcScreen.x = rcScreen.w;
			}
			glScissor(rcScreen.x, rcScreen.y, rcScreen.w, rcScreen.h);

			///////////////////////////////////////////////////////////////

			if (gMotionToPhotonW > 0.001f)
			{
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			}

			glClear(GL_COLOR_BUFFER_BIT);
		}

		bool MojingRenderBase::DrawDistortion(RenderFrame *pDistortionFrame, EyeTextureType DrawEyes /*= TEXTURE_BOTH_EYE*/)
		{
#if MOTION_TO_PHOTON_TEST
			DoMtpTest(DrawEyes);
			return true;
#endif
			// MOJING_FUNC_TRACE(g_APIlogger);
			bool bRet = true;

			GLuint LeftTexId = pDistortionFrame->GetLeftEyeTexture().GetTextureID();
			GLuint RightTexId = pDistortionFrame->GetRightEyeTexture().GetTextureID();

			if (!m_warpMesh->IsAvailable() || g_bIsModifyed)
			{
				OnModifyed();
			}
			
			// return true;
			/*
			注意：	以下M44矩阵将在Shader中与表示[-1,1]区间的色散畸变颜色分量系数V4(x,y,-1,1)相乘
			即M44 * V4 ，得到原始图的颜色采样坐标
			*/
			
			float fFOV = MojingSDK_GetFOV();
			float fT = 1 / tanf(fFOV / 2 / 180 * PI);
			Matrix4f SingleEye(
				0.5 , 0.0, 0.0, 0,				// 这是第一列
				0.0, 0.5, 0.0, 0,
				-0.5, -0.5, -1.0, 1,
				0.0, 0.0, 0.0, 0					// 这是第四列
				);

			const Matrix4f TimeWarpProjection(
				0.25, 0, 0, 0,
				0, 0.5, 0, 0,
				-0.25, -0.5, -1, -1,
				0, 0, 0, 0
				);
// 			/*glm::mat4 CalculateProjectionMatrix(float fovRad)
// 				//-----------------------------------------------------------------------------
// 			{
// 				//Project the UVs in NDC onto the far plane and convert from NDC to viewport space
// 				glm::mat4 retMtx;
// 				float tanHalfFov = tanf(0.5* fovRad);
// 
// 				retMtx[0] = glm::vec4(1.0f / tanHalfFov, 0.0f, 0.0f, 0.0f);
// 				retMtx[1] = glm::vec4(0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f);
// 				retMtx[2] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
// 				retMtx[3] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
// 
// 				return glm::transpose(retMtx);
// 			}*/
			const Matrix4f LeftEye(
				0.25, 0, 0, 0,
				0, 0.5, 0, 0,
				-0.25, -0.5, -1, -1,
				0, 0, 0, 0
				);
			const Matrix4f RightEye(
				0.25, 0, 0, 0,
				0, 0.5, 0, 0,
				-0.75, -0.5, -1, -1,
				0, 0, 0, 0
				);
			GLuint TexId[2];
			Matrix4f timeWarps[2][2];
			bool bUseClamp = false;

			bool bLeftTex = (LeftTexId != 0 && glIsTexture(LeftTexId));
			bool bRightTex =(RightTexId != 0 && glIsTexture(RightTexId));

// #ifdef _DEBUG
// 			MOJING_TRACE(g_APIlogger, "MojingRenderBase::DrawDistortion LeftTexture = " << LeftTexId << (bLeftTex ? "(TRUE)" : "(FALSE)") << " , " <<
// 				"RightTexture = " << RightTexId << (bRightTex ? "(TRUE)" : "(FALSE)"));
// #endif // _DEBUG

			EyeTextureType ET = TEXTURE_LEFT_EYE;
			if (bLeftTex && bRightTex)
			{
				// 左眼图像，按左右分屏绘制到双眼
				TexId[0] = LeftTexId;
				TexId[1] = RightTexId;
				if (LeftTexId == RightTexId)
				{
					timeWarps[0][0] = timeWarps[0][1] = LeftEye;
					timeWarps[1][0] = timeWarps[1][1] = RightEye;
					bUseClamp = true;
					ET = TEXTURE_BOTH_EYE;
				}
				else
				{// 左右眼图像分别独立绘制到双眼
					timeWarps[0][0] = timeWarps[0][1] = SingleEye;
					timeWarps[1][0] = timeWarps[1][1] = SingleEye;
				}
			}
			else if (bLeftTex && !bRightTex)
			{// 左眼图像绘制到双眼
				TexId[0] = LeftTexId;
				TexId[1] = LeftTexId;
				timeWarps[0][0] = timeWarps[0][1] = SingleEye;
				timeWarps[1][0] = timeWarps[1][1] = SingleEye;
			}
			else
			{// 使用内部文理


				if (m_CellsImageTexID == 0 || !glIsTexture(m_CellsImageTexID))
				{
					if (GetRenderThreadID() != GetDistortionThreadID() && MojingSDKStatus::GetSDKStatus()->GetEngineStatus() == ENGINE_UNITY)
					{// Unity引擎的绘制文理不是我们管理的，我们没法预期它的内容什么时候被释放。所以为了防止出现不预期的网格图，这里直接返回掉不画了
						//return false;
					}
				
				MOJING_ERROR(g_APIlogger, "Create CellsImage : Left = " << (int)LeftTexId << " GLT = " << (bool)glIsTexture(LeftTexId) << " ; Right = " << (int)RightTexId << " GLT = " << (bool)glIsTexture(RightTexId));
					// 					int iWidth = pDisplayParameters->GetScreenWidth();
					//                     iWidth = fmin(iWidth, pDisplayParameters->GetScreenHeight());
					//					int iWidth = MojingSDK_GetTextureSize();
					//					m_CellsImageTexID = CreateCells(iWidth, iWidth, 40, 4, 0, -1, pDisplayParameters->GetPPI(), Manager::GetMojingManager()->GetDistortion()->GetMetersPerTanAngleAtCenter());
					//					m_texIds.push_back(m_CellsImageTexID);
					m_CellsImageTexID = CreateCellsImage();
				}
				TexId[0] = m_CellsImageTexID;
				TexId[1] = m_CellsImageTexID;
				timeWarps[0][0] = timeWarps[0][1] = SingleEye;
				timeWarps[1][0] = timeWarps[1][1] = SingleEye;

				if (LeftTexId != 0 || RightTexId != 0)
					bRet = false;// 被迫使用了与预期不符的纹理
			}
			if (m_bEnableTimeWarp)
			{// 根据开关决定是否要启用预测修正
				timeWarps[0][0] = timeWarps[0][1] = m_DistortionFixMatrix * timeWarps[0][0];
				timeWarps[1][0] = timeWarps[1][1] = m_DistortionFixMatrix * timeWarps[1][0];
			}

			// 背景清成黑色,在部分平台上这个背景的颜色有时候会被换掉
#ifdef _DEBUG
			;
			glClearColor((DrawEyes & TEXTURE_LEFT_EYE) / TEXTURE_LEFT_EYE, 
				(DrawEyes & TEXTURE_RIGHT_EYE) / TEXTURE_RIGHT_EYE, 0.0f, 1.0f);
#else
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
#endif
			// 			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			
#ifndef MJ_OS_WIN32
			SetWarpState(m_bShowTimeWapRange);
#else

			SetWarpState(TRUE);
			// SetWarpState(FALSE);
#endif // !MJ_OS_WIN32			
			glViewport(0, 0, m_iScreenWidth, m_iScreenHeight);
			// Warp each eye to the display surface
			if (DrawEyes & TEXTURE_LEFT_EYE)
			{
				if (m_bLandscape)
					glScissor(0, 0, m_iScreenWidth_Half, m_iScreenHeight);// 设置只有左半屏会被绘制
				else
					glScissor(0, m_iScreenHeight_Half, m_iScreenWidth, m_iScreenHeight_Half);// 设置只有下半屏会被绘制

				glClear(GL_COLOR_BUFFER_BIT);
				int eye = 0;
				
				BindWarpProgram(m_warpProg, timeWarps[0][0], eye, bUseClamp);

				BindEyeTextures(TexId[eye]);

				// Draw the warp triangles.
				m_warpMesh->BindVBO();
				m_warpMesh->DrawElements(eye);
				if (ET == TEXTURE_LEFT_EYE)
					ET = TEXTURE_RIGHT_EYE;
			}
			if (DrawEyes & TEXTURE_RIGHT_EYE)
			{
				if (m_bLandscape)
					glScissor(m_iScreenWidth_Half, 0, m_iScreenWidth_Half, m_iScreenHeight);
				else
					glScissor(0, 0, m_iScreenWidth, m_iScreenHeight_Half);

				glClear(GL_COLOR_BUFFER_BIT);
				int eye = 1;
				
				BindWarpProgram(m_warpProg, timeWarps[1][0], eye, bUseClamp);

				BindEyeTextures(TexId[eye]);

				// Draw the warp triangles.
				m_warpMesh->BindVBO();
				m_warpMesh->DrawElements(eye);

				if (ET == TEXTURE_LEFT_EYE)
					ET = TEXTURE_RIGHT_EYE;
			}


			UnbindEyeTextures();
			
			glUseProgram(0);
			m_warpMesh->UnBindVBO();
			glDisable(GL_SCISSOR_TEST);

			return bRet;
			// 将这一帧移动到畸变完成队列,函数内部会调用glFlush
		}

		Matrix4f GUI_Matrix4f(Vector4f v4fGUIRect)
		{
			MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
			enum X_Axis_Order
			{
				XO_RIGHT_BIGGER,
				XO_LEFT_BIGGER
			}XOrder = XO_RIGHT_BIGGER;
			enum X_Axis
			{
				XA_REVERSAL,
				//XO_CENTER,
				XA_NORMAL
			}XOrigin = XA_REVERSAL;
			enum Y_Axis_Order
			{
				YO_UP_BIGGER,
				YO_DOWN_BIGGER
			}YOrder = YO_DOWN_BIGGER;
			enum Y_Axis
			{
				YA_REVERSAL,
				//YO_CENTER,
				YA_NORMAL
			}YOrigin = YA_REVERSAL;
			Vector3f OverlayPosition(v4fGUIRect.x ,v4fGUIRect.y , 0);
			Vector3f OverlayScale(v4fGUIRect.z ,v4fGUIRect.w , 0);
#if 0
			{
				static float fFrame = 0;
				if (fFrame > 200)
					fFrame = 0;
				OverlayPosition.x = fFrame;
				OverlayPosition.y = fFrame;

				OverlayScale.x = 0.5;
				OverlayScale.y = 0.5;

				fFrame = fFrame + 1;
			}
#endif

#if defined(MJ_OS_ANDROID) || defined(MJ_OS_WIN32)
			XOrder = XO_LEFT_BIGGER;
			XOrigin = XA_NORMAL;
			YOrder = YO_DOWN_BIGGER;
			YOrigin = YA_REVERSAL;
			if (pStatus->GetEngineStatus() == ENGINE_UNITY)
			{
				XOrder = XO_LEFT_BIGGER;
				XOrigin = XA_NORMAL;
				YOrder = YO_DOWN_BIGGER;
				YOrigin = YA_NORMAL;
			}
#elif defined(MJ_OS_IOS)

			// HOME 在右的时候如此设置
#if 1
			XOrder = XO_LEFT_BIGGER;
			XOrigin = XA_NORMAL;
			YOrder = YO_DOWN_BIGGER;
			YOrigin = YA_REVERSAL;

			if (pStatus->GetEngineStatus() == ENGINE_UNITY)
			{
				YOrigin = YA_NORMAL;
			}
#else
			// HOME 在左的时候
			XOrder = XO_RIGHT_BIGGER;
			XOrigin = XA_REVERSAL;
			YOrder = YO_UP_BIGGER;
			YOrigin = YA_REVERSAL;

			if (pStatus->GetEngineStatus() == ENGINE_UNITY)
			{
				XOrder = XO_LEFT_BIGGER;
				XOrigin = XA_NORMAL;
				YOrder = YO_DOWN_BIGGER;
				YOrigin = YA_NORMAL;
			}
#endif

#endif
			/*
			注意：	以下M44矩阵将在Shader中与表示[-1,1]区间的色散畸变颜色分量系数V4(x,y,-1,1)相乘
			即M44 * V4 ，得到原始图的颜色采样坐标
			*/
			// 基准矩阵，将图像缩放到正好满屏
			Matrix4f SingleEye =
				Matrix4f(	0.5, 0.0, 0.0, 0.0,
							0.0, 0.5, 0.0, 0.0,
							0.5, 0.5, 1.0, 0.0,
							0.0, 0.0, 0.0, 0.0);

			Matrix4f SingleEye_Scale;// 对上面的矩阵做缩放和轴旋转
			Matrix4f ScaleToTarget;// 用来将缩放后的矩阵移动到左上角，再移动到目标位置上
			// 防止除零错
			OverlayScale.x = fmax(0.000001, OverlayScale.x);
			OverlayScale.y = fmax(0.000001, OverlayScale.y);

			SingleEye_Scale.M[0][0] = 1 / (OverlayScale.x);
			SingleEye_Scale.M[1][1] = 1 / (OverlayScale.y);
			if (XOrigin == XA_REVERSAL)
			{
				SingleEye_Scale.M[0][0] = -SingleEye_Scale.M[0][0];
			}

			if (YOrigin == YA_REVERSAL)
			{
				SingleEye_Scale.M[1][1] = -SingleEye_Scale.M[1][1];
			}
			if (XOrder == XO_LEFT_BIGGER)
			{
				ScaleToTarget.M[3][0] = 1 - OverlayScale.x - OverlayPosition.x * 2;
			}
			else
			{
				ScaleToTarget.M[3][0] = OverlayScale.x - 1 + OverlayPosition.x * 2;
			}
			if (YOrder == YO_DOWN_BIGGER)
			{
				ScaleToTarget.M[3][1] = OverlayScale.y - 1 + OverlayPosition.y * 2;
			}
			else
			{
				ScaleToTarget.M[3][1] = 1 - OverlayScale.y - OverlayPosition.y * 2;
			}

			Matrix4f TopLeftToTarget;// 将缩放后归一到左上角的矩阵移动到需要的位置
			// 1 缩放
			SingleEye = SingleEye_Scale * SingleEye;
			// 2 移动到移动到目标位置
			SingleEye = ScaleToTarget *SingleEye;

			return SingleEye;
		}
		bool MojingRenderBase::DrawDistortionGUI(EyeTextureType DrawEyes/* = TEXTURE_BOTH_EYE*/)
		{
			bool bLeftOverlay = (GetLeftOverlayTextureID() != 0) && glIsTexture(GetLeftOverlayTextureID()) && m_v4LeftOverlayRect.w > 1e-4 && m_v4LeftOverlayRect.z > 1e-4;
			bool bRightOverlay = (GetRightOverlayTextureID() != 0) && glIsTexture(GetRightOverlayTextureID()) && m_v4RightOverlayRect.w > 1e-4 && m_v4RightOverlayRect.z > 1e-4;
			if (!bLeftOverlay && !bRightOverlay)
			{
				return true;// 不需要绘制 
			}
			struct __tagOverlayTexture
			{
				int	  m_iTextureID;
				Matrix4f m_m4Matrix;
			};
			__tagOverlayTexture OverlayParmat[2];
			OverlayParmat[0].m_iTextureID = GetLeftOverlayTextureID();
			OverlayParmat[0].m_m4Matrix = GUI_Matrix4f(GetLeftOverlayRect());
			
			if (bRightOverlay)
			{
				OverlayParmat[1].m_iTextureID = GetRightOverlayTextureID();
				OverlayParmat[1].m_m4Matrix = GUI_Matrix4f(GetRightOverlayRect());
			}
			else
			{
				OverlayParmat[1] = OverlayParmat[0];
			}

			// MOJING_FUNC_TRACE(g_APIlogger);
			bool bRet = true;
			const Matrix4f timeWarps[2][2] = { 
												{ OverlayParmat[0].m_m4Matrix, OverlayParmat[0].m_m4Matrix }, 
												{ OverlayParmat[1].m_m4Matrix, OverlayParmat[1].m_m4Matrix } 
											};

			SetWarpState(true);
			if (DrawEyes & TEXTURE_LEFT_EYE)
			{
				if (m_bLandscape)
					glScissor(0, 0, m_iScreenWidth_Half, m_iScreenHeight);
				else
					glScissor(0, m_iScreenHeight_Half, m_iScreenWidth, m_iScreenHeight_Half);

				int eye = 0;
				
				BindWarpProgram(m_warpProgLayout , OverlayParmat[eye].m_m4Matrix, eye, false);
				BindEyeOverlayTextures(OverlayParmat[eye].m_iTextureID);

				// Draw the warp triangles.
				m_warpLayer->BindVBO();
				m_warpLayer->DrawElements(eye);
			}	// for eye
			if (DrawEyes & TEXTURE_RIGHT_EYE)
			{
				if (m_bLandscape)
					glScissor(m_iScreenWidth_Half, 0, m_iScreenWidth_Half, m_iScreenHeight);
				else
					glScissor(0, 0, m_iScreenWidth, m_iScreenHeight_Half);

				int eye = 1;

				BindWarpProgram(m_warpProgLayout, OverlayParmat[eye].m_m4Matrix, eye, false);
				BindEyeOverlayTextures(OverlayParmat[eye].m_iTextureID);

				// Draw the warp triangles.
				m_warpLayer->BindVBO();

				m_warpLayer->DrawElements(eye);
			}	// for eye

			UnbindEyeOverlayTextures();
			glUseProgram(0);
			m_warpLayer->UnBindVBO();
			
			return true;

		}

		bool MojingRenderBase::DrawDistortionRange(EyeTextureType DrawEyes /*= TEXTURE_BOTH_EYE*/)
		{
			bool bRet = true;

			if (!m_bShowTimeWapRange)
				return false;

			MojingDisplayParameters *pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
			if (!m_warpMesh->IsAvailable() || g_bIsModifyed)
			{
				OnModifyed();
			}

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

			EyeTextureType ET = TEXTURE_LEFT_EYE;
			// 背景清成黑色
			// 			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			// 			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			SetWarpState(true);
			glViewport(0, 0, m_iScreenWidth, m_iScreenHeight);
			//glScissor(0, 0, pDisplayParameters->GetScreenWidth(), pDisplayParameters->GetScreenHeight());
			// Warp each eye to the display surface
			if (DrawEyes & TEXTURE_LEFT_EYE)
			{
				if (m_bLandscape)
					glScissor(0, 0, m_iScreenWidth_Half, m_iScreenHeight);// 设置只有左半屏会被绘制
				else
					glScissor(0, m_iScreenHeight_Half, m_iScreenWidth, m_iScreenHeight_Half);// 设置只有下半屏会被绘制

				int eye = 0;
				double vsyncBase = 0;

				BindWarpProgram(m_warpProgDistortionRange, SingleEye, eye, false);
				// Draw the warp triangles.
				m_warpMesh->BindVBO();
				/*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
				
				m_warpMesh->DrawElements(eye);
				/*glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/
				if (ET == TEXTURE_LEFT_EYE)
					ET = TEXTURE_RIGHT_EYE;
			}
			if (DrawEyes & TEXTURE_RIGHT_EYE)
			{
				if (m_bLandscape)
					glScissor(m_iScreenWidth_Half, 0, m_iScreenWidth_Half, m_iScreenHeight);
				else
					glScissor(0, 0, m_iScreenWidth, m_iScreenHeight_Half);

				int eye = 1;
				double vsyncBase = 0;
				BindWarpProgram(m_warpProgDistortionRange, SingleEye, eye, false);
				// Draw the warp triangles.
				m_warpMesh->BindVBO();
				m_warpMesh->DrawElements(eye);

				if (ET == TEXTURE_LEFT_EYE)
					ET = TEXTURE_RIGHT_EYE;
			}


			UnbindEyeTextures();
			glUseProgram(0);
			m_warpMesh->UnBindVBO();

			return bRet;
		}

		bool MojingRenderBase::DrawCenterLine()
		{
			bool bRet = true;

			/*绘制中心的竖线*/
			float fCenterLineWidth = GetCenterLineWidth();

			
			if (fCenterLineWidth > 1)
			{
				glEnable(GL_SCISSOR_TEST);
				int iLeft = floor(m_iScreenWidth_Half - fCenterLineWidth / 2);
				// floor(x)返回的是小于或等于x的最大整数。
				glScissor(iLeft, 0, fCenterLineWidth, m_iScreenHeight);
				glClearColor(m_CenterLineParam.m_fColR, m_CenterLineParam.m_fColG, m_CenterLineParam.m_fColB, 1.0);
				glClear(GL_COLOR_BUFFER_BIT);
				glDisable(GL_SCISSOR_TEST);
			}

			return bRet;
		}

		void MojingRenderBase::SetModify()
		{
			if (!g_bIsModifyed)
			{
				g_bIsModifyed = true;
			}
		}

		GLuint MojingRenderBase::CreateCellsImage()
		{
			MojingDisplayParameters * pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
			int iWidth = pDisplayParameters->GetScreenWidth();
			iWidth = fmin(iWidth, pDisplayParameters->GetScreenHeight());
			
			if (iWidth > 1024)
				iWidth = 1024;
			int iLineColor = -1;
			if (MojingSDKStatus::GetSDKStatus()->GetEngineStatus() == ENGINE_UNITY)
			{// Unity引擎的绘制文理不是我们管理的，我们没法预期它的内容什么时候被释放。所以为了防止出现不预期的网格图，这里直接返回掉不画了
				iLineColor = 0;
			}
			GLuint iId = CreateCells(iWidth, iWidth, 40, 4, 0, iLineColor, pDisplayParameters->GetPPI(), Manager::GetMojingManager()->GetDistortion()->GetMetersPerTanAngleAtCenter());
			// m_texIds.push_back(iId);
			return iId;
		}

		void MojingRenderBase::SetCenterLine(int iWidth, int colR, int colG, int colB, int colA)
		{
			m_CenterLineParam.m_fColR = colR / 255.0;
			m_CenterLineParam.m_fColG = colG / 255.0;
			m_CenterLineParam.m_fColB = colB / 255.0;
			m_CenterLineParam.m_fWidth = iWidth;

			SetCenterLineWidth(iWidth * 1.0);
// 			SetCenterLineWidth(iWidth * 1.0);
// 			m_LineDraw.SetLineColor(colR / 255.0, colG / 255.0, colB / 255.0);
		}

		bool MojingRenderBase::WarpToScreen()
		{// 用于单线程
			RenderFrame * pDistortionFrame = SelectDistortionFrame();
			if (pDistortionFrame)
			{
				if (m_bEnableTimeWarp)
					m_DistortionFixMatrix = m_RenderFrame.CalculateTimeWarpMatrix(m_dNextFrameDisplayTime + DISPLY_FRAME_TIME_AFTER_DRAW);
#ifdef MJ_OS_WIN32
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glClearColor(0, 0, 0, 1);
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
#else
				DrawDistortionRange();
#endif				
				
				DrawDistortion(pDistortionFrame);
 				DrawDistortionGUI();
				DrawCenterLine();
				pDistortionFrame->SetFrameWorkFlowState(DISPLAY_END);

				if (m_ui64NextDisplayFrameIndex + 1 >= pDistortionFrame->GetFrameIndex() + 1)
				{
					m_ui64NextDisplayFrameIndex++;
				}
				else
				{	// 单线程模式下不压帧
					m_ui64NextDisplayFrameIndex = pDistortionFrame->GetFrameIndex() + 1;
				}

				m_dNextFrameDisplayTime = FramePointTimeInSeconds(m_ui64NextDisplayFrameIndex);
			}
			
#ifdef _DEBUG
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
					MOJING_TRACE(g_APIlogger, szInfo);
				}
				dStartDrawTime = dDrawTime;
			}

			
#endif
			return true;
		}


		void MojingRenderBase::FramesPerSecond()
		{

		}

		UInt64 MojingRenderBase::GetCurrentModelFrameInfo(RenderFrame *pRF, double *pTime /*= NULL*/, float * pMatrix /*= NULL*/)
		{
			FrameWorkFlowState WS = pRF->GetFrameWorkFlowState();
			if (WS == DISPLAY_END || WS == UNINIT)
			{
				while (!pRF->BeforModle(m_ui64NextDisplayFrameIndex))
					m_ui64NextDisplayFrameIndex++;

				pRF->SetFrameWorkFlowState(INIT);
			}

			if (pTime)
				*pTime = pRF->GetPredictDisplayTime();
			// 更新帧的预测
			pRF->UpdatePredictView();
			if (pMatrix)
			{
				Matrix4f viewMatrix(pRF->GetPredictView());
				for (int i = 0; i < 16; i++)
				{
					pMatrix[i] = viewMatrix.M[i / 4][i % 4];
				}
			}
			return pRF->GetFrameIndex();
		}

		UInt64 MojingRenderBase::GetCurrentModelFrameInfo(RenderFrame *pRF, double *pTime /*= NULL*/, float * x, float * y, float * z, float * w)
		{
			FrameWorkFlowState WS = pRF->GetFrameWorkFlowState();
			if (WS == DISPLAY_END || WS == UNINIT)
			{
				while (!pRF->BeforModle(m_ui64NextDisplayFrameIndex))
					m_ui64NextDisplayFrameIndex++;

				pRF->SetFrameWorkFlowState(INIT);
			}

			if (pTime)
				*pTime = pRF->GetPredictDisplayTime();

			pRF->UpdatePredictView();
			Quatf Q = pRF->GetPredictView();
			*x = Q.x;
			*y = Q.y;
			*z = Q.z;
			*w = Q.w;
			return pRF->GetFrameIndex();
		}

		UInt64 MojingRenderBase::GetCurrentModelFrameInfo(double *pTime /*= NULL*/, float * pMatrix /*= NULL*/)
		{
			// 单线程不需要TimeWarp的时候直接返回0，用默认陀螺仪信息
			return m_bEnableTimeWarp ? GetCurrentModelFrameInfo(&m_RenderFrame, pTime, pMatrix) : 0;
		};

		UInt64 MojingRenderBase::GetCurrentModelFrameInfo(double *pTime /*= NULL*/, float * x, float * y, float * z, float * w)
		{
			return m_bEnableTimeWarp ? GetCurrentModelFrameInfo(&m_RenderFrame, pTime, x, y, z, w) : 0;
		}

		GLuint MojingRenderBase::CreateCells(int iImageWidth, int iImageHeight, int iCellWidth, int iLineWidth, int iBKCol, int iLineCol, float fPPI, float fMetersPerTanAngleAtCenter)
		{
			char* pBuffer = new char[iImageWidth * iImageHeight * 4];
			// FILE  *file = fopen("/sdcard/tt.txt", "wt");
			/*计算R^2/10*/
			int iRt = iImageHeight / 2;
			iRt *= iRt;
			iRt /= 10;
			// fprintf(file, "Width = %d  Rt = %d \n ix , iy , iR , iTest\n", iImageHeight, iRt);
			SDK_VERIFY  status = MojingSDKStatus::GetSDKStatus()->GetVerfiyStatus();
			if (status != VERIFY_OK)
			{
				for (int iY = 1; iY < iImageHeight - 1; iY++)
				{
					int* lpLenBuffer = (int*)pBuffer + iImageWidth * iY;
					for (int iX = 1; iX < iImageWidth - 1; iX++)
					{
						lpLenBuffer[iX] = (iBKCol & 0x00FFFFFF);
					}
				}
				int width = 0;
				int height = 0;
				const unsigned char *pInfo = NULL;
				if (status == VERIFY_INVALID)
				{
					width = INFO_VERIFYFAILED_WIDTH;
					height = INFO_VERIFYFAILED_HEIGHT;
					pInfo = VerifyFailedInfo;
				}
				else if (status == VERIFY_VERSION_LOW)
				{
					width = INFO_VERSIONLOW_WIDTH;
					height = INFO_VERSIONLOW_HEIGHT;
					pInfo = SDKVersionLowInfo;
				}

				if (pInfo != NULL)
				{
					for (int iY = 0; iY < height; iY++)
					{
						int* lpLenBuffer = (int*)pBuffer + iImageWidth * (iY + (iImageHeight - height) / 2) + (iImageWidth - width) / 2;
						for (int iX = 0; iX < width / 8; iX++)
						{
							int offset = iY*(width / 8) + iX;
							unsigned char c = *(pInfo + offset);
							offset = 8 * iX;
							for (int i = 0; i < 8; i++)
							{
								if (((c >> (7 - i)) & 0x01) == 0)
									lpLenBuffer[offset + i] = (iBKCol | 0x00FFFFFF);
							}
						}
					}
				}
			}
			else
			{

				// 最边上一圈需要黑色,所以循环边界要各缩放1 像素
				for (int iY = 1; iY < iImageHeight - 1; iY++)
				{
					int iTestY = iY - iImageHeight / 2;

					int* lpLenBuffer = (int*)pBuffer + iImageWidth * iY;
					for (int iX = 1; iX < iImageWidth - 1; iX++)
					{
						lpLenBuffer[iX] = (iBKCol & 0x00FFFFFF);

						int iTestX = iX - iImageHeight / 2;

						int iTestR = iTestX *iTestX + iTestY * iTestY;
						int iTest = iTestR;
						int iT = 1024;
						if (iTest > iRt)
							iTest %= iRt;
						iTest = abs(iTest - iRt);
						if ((iLineWidth > (abs((iY + iLineWidth / 2) - (iImageHeight / 2)) % iCellWidth)) || // 1 Y轴判定横线
							(iLineWidth > (abs((iX + iLineWidth / 2) - (iImageWidth / 2)) % iCellWidth)) ||// 2 X轴判定纵线 
							iLineWidth / 2 > abs(abs(iX - iImageWidth / 2) - abs(iY - iImageHeight / 2)) // 3 对角线
							|| iTest < iT)
						{
							lpLenBuffer[iX] = iLineCol | 0xFF000000;
						}
					}
				}
			}
			// 			if (file)
			// 				fclose(file);

			//	memset(pBuffer, 128, iImageWidth * iImageHeight * 4);
			/////////////////////////////////////////////////////////////////////////
			GLuint iRet = 0;
			glGenTextures(1, &iRet);     // Create The Texture

			if (iRet)
			{
				glBindTexture(GL_TEXTURE_2D, iRet);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				//	glPixelStoref(GL_PACK_ALIGNMENT, 1);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iImageWidth, iImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBuffer);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			delete[] pBuffer;
			return iRet;
		}

		bool MojingRenderBase::LoadMeshFromMemory()
		{
			return m_warpMesh->BuildGeometry();
			// return m_warpMesh->BuildGeometry_GVR();
		}

		void MojingRenderBase::SetWarpState(bool bEnableAlpha)
		{
			glDepthMask(GL_FALSE);	// don't write to depth, even if Unity has depth on window
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_SCISSOR_TEST);// 允许区域剪裁
			if (bEnableAlpha)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			// sRGB conversion on write will only happen if BOTH the window
			// surface was created with EGL_GL_COLORSPACE_KHR,  EGL_GL_COLORSPACE_SRGB_KHR
			// and GL_FRAMEBUFFER_SRGB_EXT is enabled.
			//这里是扩展不不支持，需要关闭
#ifdef MJ_OS_ANDROID
			if (m_HasEXT_sRGB_write_control)
			{
				glEnable(GL_FRAMEBUFFER_SRGB_EXT);
			}
#endif
			// GL_CheckErrors("SetWarpState");
		}
		int step(float x, float y)
		{
			return x < y ? 0 : 1;
		}

		void MojingRenderBase::InstallShader()
		{
			Distortion *pDistortion = Manager::GetMojingManager()->GetDistortion();
			// 释放掉已经申请和分配的程序/Shader对象
			// 主场场景
			if (!glIsProgram(m_warpProg_NoDispersion.Prog.program))
			{
				m_warpProg_NoDispersion = BuildWarpProg(VertexShader, FragmentShaderNoDispersion);
				m_programIds.push_back(m_warpProg_NoDispersion.Prog.program);
				m_shaderIds.push_back(m_warpProg_NoDispersion.Prog.vertexShader);
				m_shaderIds.push_back(m_warpProg_NoDispersion.Prog.fragmentShader);
			}

			if (!glIsProgram(m_warpProg_Dispersion.Prog.program))
			{
				m_warpProg_Dispersion = BuildWarpProg(VertexShader, FragmentShader);
				m_programIds.push_back(m_warpProg_Dispersion.Prog.program);
				m_shaderIds.push_back(m_warpProg_Dispersion.Prog.vertexShader);
				m_shaderIds.push_back(m_warpProg_Dispersion.Prog.fragmentShader);
			}
			// Layout
			if (!glIsProgram(m_warpProgLayout_NoDispersion.Prog.program))
			{
				m_warpProgLayout_NoDispersion = BuildWarpProg(VertexShaderLayout, FragmentShaderLayoutNoDispersion);
				m_programIds.push_back(m_warpProgLayout_NoDispersion.Prog.program);
				m_shaderIds.push_back(m_warpProgLayout_NoDispersion.Prog.vertexShader);
				m_shaderIds.push_back(m_warpProgLayout_NoDispersion.Prog.fragmentShader);
			}
			if (!glIsProgram(m_warpProgLayout_Dispersion.Prog.program))
			{
				m_warpProgLayout_Dispersion = BuildWarpProg(VertexShaderLayout, FragmentShaderLayout);
				m_programIds.push_back(m_warpProgLayout_Dispersion.Prog.program);
				m_shaderIds.push_back(m_warpProgLayout_Dispersion.Prog.vertexShader);
				m_shaderIds.push_back(m_warpProgLayout_Dispersion.Prog.fragmentShader);
			}

			if (!glIsProgram(m_warpProgDistortionRange.Prog.program))
			{
				m_warpProgDistortionRange = BuildWarpProg(VertexShaderDistortionRange, m_bShowTimeWapRange ? FragmentShaderDistortionRange : FragmentShaderDistortionRangeDark);
				m_programIds.push_back(m_warpProgDistortionRange.Prog.program);
				m_shaderIds.push_back(m_warpProgDistortionRange.Prog.vertexShader);
				m_shaderIds.push_back(m_warpProgDistortionRange.Prog.fragmentShader);
			}
			if (pDistortion->GetNoDispersion())
			{// 使用无色散Shader
				MOJING_TRACE(g_APIlogger , "MojingRenderBase::InstallShader ==> No Dispersion Shader......");
				m_warpProg = m_warpProg_NoDispersion;//BuildWarpProg(VertexShader, FragmentShaderNoDispersion);
				m_warpProgLayout = m_warpProgLayout_NoDispersion;// BuildWarpProg(VertexShaderLayout, FragmentShaderLayoutNoDispersion);
			}
			else
			{// 使用有色散Shader
				MOJING_TRACE(g_APIlogger , "MojingRenderBase::InstallShader ==> Dispersion Shader......");
				
				m_warpProg = m_warpProg_Dispersion;// BuildWarpProg(VertexShader, FragmentShader);
				m_warpProgLayout = m_warpProgLayout_Dispersion;// BuildWarpProg(VertexShaderLayout, FragmentShaderLayout);
			}
		}
		// Returns false and logs the ShaderInfoLog on failure.
		bool MojingRenderBase::CompileShader(const GLuint shader, const char * src)
		{
			glShaderSource(shader, 1, &src, 0);
			glCompileShader(shader);

			GLint r;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
			if (r == GL_FALSE)
			{
				GLchar msg[4096];
				glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
				MOJING_ERROR(g_APIlogger, "MojingRenderBase::CompileShader failed! msg : " << msg);
				return false;
			}
			return true;
		}

		void RemoveKeyWord(char *szInOutString, const char * szKeyWord)
		{
			int iRemoveLen = strlen(szKeyWord);
			char *pPos = szInOutString;
			while (NULL != (pPos = strstr(pPos , szKeyWord)))
			{
				memmove(pPos, pPos + iRemoveLen, strlen(pPos + iRemoveLen) + 1);
			}
		}

		WarpProg MojingRenderBase::BuildWarpProg(const char * vertex, const char * fragment)
		{
			WarpProg wp;

#ifdef MJ_OS_WIN32
			char *pVertex = new char[strlen(vertex) + 1];
			char *pFragment = new char[strlen(fragment) + 1];
			strcpy(pVertex, vertex);
			strcpy(pFragment, fragment);
			RemoveKeyWord(pVertex , "lowp");
			RemoveKeyWord(pVertex, "mediump");
			RemoveKeyWord(pVertex, "highp");

			RemoveKeyWord(pFragment, "lowp");
			RemoveKeyWord(pFragment, "mediump");
			RemoveKeyWord(pFragment, "highp");

			wp.Prog = BuildProgram(pVertex, pFragment);
			delete pVertex;
			delete pFragment;
#else

			wp.Prog = BuildProgram(vertex, fragment);
#endif

			return wp;
		}
		GlProgram MojingRenderBase::BuildProgram(const char * vertexSrc,
			const char * fragmentSrc)
		{
			GlProgram prog;
			prog.vertexShader = glCreateShader(GL_VERTEX_SHADER);
			if (!CompileShader(prog.vertexShader, vertexSrc))
			{
				return prog;
			}
			//m_shaderIds.push_back(prog.vertexShader);

			prog.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			if (!CompileShader(prog.fragmentShader, fragmentSrc))
			{
				return prog;
			}
			//m_shaderIds.push_back(prog.fragmentShader);

			prog.program = glCreateProgram();
			// m_programIds.push_back(prog.program);
			glAttachShader(prog.program, prog.vertexShader);
			glAttachShader(prog.program, prog.fragmentShader);

			// set attributes before linking
			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_LOCATION_POSITION, "Position");
			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_DISTORTION_R, "dR");
			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_DISTORTION_B, "dB");

			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_LOCATION_COLOR, "VertexColor");
			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_DISTORTION_G, "TexCoord");
			glBindAttribLocation(prog.program, VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO, "TexCoord1");


			// link and error check
			glLinkProgram(prog.program);
			GLint r;
			glGetProgramiv(prog.program, GL_LINK_STATUS, &r);
			if (r != GL_FALSE)
			{
				prog.uMvp = glGetUniformLocation(prog.program, "Mvpm");
				prog.uModel = glGetUniformLocation(prog.program, "Modelm");
				prog.uView = glGetUniformLocation(prog.program, "Viewm");
				prog.uTexm = glGetUniformLocation(prog.program, "Texm");
				prog.uTexClipX = glGetUniformLocation(prog.program, "TexClipX");
				prog.uTexture0 = glGetUniformLocation(prog.program, "Texture0");
				prog.uLayerIndex = glGetUniformLocation(prog.program, "layerIndex");
				glUseProgram(prog.program);

				// texture and image_external bindings
				for (int i = 0; i < 8; i++)
				{
					char name[32];
					sprintf(name, "Texture%i", i);
					const GLint uTex = glGetUniformLocation(prog.program, name);
					if (uTex != -1)
					{
						glUniform1i(uTex, i);
					}
				}

				glUseProgram(0);
			}
			else
			{
				GLchar msg[1024];
				glGetProgramInfoLog(prog.program, sizeof(msg), 0, msg);
			}
			return prog;
		}

		void MojingRenderBase::BindWarpProgram(WarpProg warpProg , const Matrix4f timeWarps, const int eye, bool bClamp)
		{
			static Matrix4f LandscapeOrientationMatrix = m_bLandscape ? Matrix4f(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				): 
				Matrix4f(
				0.0f, -1.0f, 0.0f, 0.0f, 
				1.0f, 0.0f, 0.0f, 0.0f,				
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);

			// Select the warp program.
			GlProgram prog = warpProg.Prog;

			glUseProgram(prog.program);

			// Set the shader parameters.
			glUniformMatrix4fv(prog.uMvp, 1, GL_FALSE, LandscapeOrientationMatrix.M[0]);

			glUniformMatrix4fv(prog.uTexm, 1, GL_FALSE, timeWarps.M[0]);
			// glUniformMatrix4fv(prog.uTexm2, 1, GL_FALSE, timeWarps[eye][1].M[0]);
			if (prog.uTexClipX > 0)
			{
				// split screen clamping for UE4
				Vector2f clampX(0, 1.0);
				if (bClamp)
				{
					clampX.x = 0.5 * eye + eye * 0.001f;
					clampX.y = 0.5 * eye + 0.5 - (1 - eye) * 0.001f;
				}
				glUniform2fv(prog.uTexClipX, 1, &clampX.x);
			}

		}
	
		void MojingRenderBase::BindEyeTextures(GLuint TexId)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TexId);

 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			WarpProg warpProg = m_warpProg;
			GlProgram prog = warpProg.Prog;

			glUniform1i(prog.uTexture0, GL_TEXTURE0 - GL_TEXTURE0);
		}

		void MojingRenderBase::UnbindEyeTextures()
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
			//////////////////////////////////////
			glActiveTexture(GL_TEXTURE0);

		}


		bool MojingRenderBase::ExtensionStringPresent(const char * extension, const char * allExtensions)
		{
			if (extension && allExtensions && 
				*extension&&*allExtensions &&
				strstr(allExtensions, extension))
			{
				//LOG("Found: %s", extension);
				return true;
			}
			else
			{
				//LOG("Not found: %s", extension);
				return false;
			}
		}

		void MojingRenderBase::clearShaderIds()
		{
			for (unsigned int i = 0; i < m_shaderIds.size(); i++)
			{
				if (glIsShader(m_shaderIds[i]))
				{
					glDeleteShader(m_shaderIds[i]);
				}
			}
		}

		void MojingRenderBase::clearProgramIds()
		{
			for (unsigned int i = 0; i < m_programIds.size(); i++)
			{
				if (glIsProgram(m_programIds[i]))
				{
					glDeleteProgram(m_programIds[i]);
				}
			}
		}

		void MojingRenderBase::clearEyeTextures()
		{
			for (std::map<unsigned int, EyeTextureParam>::iterator itor = m_eyeTextureParams.begin();
				itor != m_eyeTextureParams.end(); itor++)
			{
				if (glIsTexture(itor->first))
				{
					glDeleteTextures(1, &(itor->first));
				}
			}
			m_eyeTextureParams.clear();
		}

		void MojingRenderBase::DeleteProgram(GlProgram & prog)
		{
			glUseProgram(0);
			if (prog.program != 0)
			{
				glDeleteProgram(prog.program);
			}
			if (prog.vertexShader != 0)
			{
				glDeleteShader(prog.vertexShader);
			}
			if (prog.fragmentShader != 0)
			{
				glDeleteShader(prog.fragmentShader);
			}
			prog.program = 0;
			prog.vertexShader = 0;
			prog.fragmentShader = 0;
		}

		void MojingRenderBase::BindEyeOverlayTextures(GLuint TexId)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TexId);
			//	WarpProg warpProg = GetLayerTextureID() ? m_warpProgLayout : m_warpProg;
			WarpProg warpProg = m_warpProgLayout;
			GlProgram prog = warpProg.Prog;

			// int texId0 = glGetUniformLocation(prog.program, "Texture0");
			glUniform1i(prog.uTexture0, GL_TEXTURE0 - GL_TEXTURE0);
		}

		void MojingRenderBase::UnbindEyeOverlayTextures()
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}


	}
}


