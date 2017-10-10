#include "UnityPluginInterfaceBase.h"
#ifdef MJ_OS_ANDROID
#include "UnityPluginInterfaceQ820.h"
#endif

#ifdef MJ_OS_WIN32
#include "UnityPluginInterfaceWin32.h"
#endif

#ifdef MJ_OS_IOS
#include "../../iOS/MojingIOSAPI.h"
#import "../../iOS/MojingIOSBase.h"
#include "UnityPluginInterface_iOSGL.h"
#include "UnityPluginInterface_iOSMetal.h"
#endif

#include "../../../MojingManager.h"
#include "../../../Parameters/MojingParameters.h"
#include "../../../Parameters/MojingDeviceParameters.h"
#include "../../../Base/GlUtils.h"

#include "../../../MojingAPI.h"
#include "../../../MojingSDKStatus.h"
#include "../../../Render/MojingRenderBase.h"
#include "../GlStateSave.h"


#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#ifdef MJ_OS_ANDROID
#include "../../../Platform/Android/MojingAndroidDeviceInfo.h"
extern char *g_pAPPName;
extern char *g_pPacketName;
extern char * g_pUserID;
#endif

namespace Baofeng
{
	namespace Mojing
	{
		UnityPluginInterfaceBase* UnityPluginInterfaceBase::g_pUnityPlugin = NULL;

		UnityPluginInterfaceBase::UnityPluginInterfaceBase():
			m_bIsInMojingWorld(false),
			// m_fFOV(0),
			//m_fGlassesSeparation(0),
			//m_iGlassesSeparationInPixel(0),
			m_iSampleFrequence(0)
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			SetClassName(__FUNCTION__);
		}

		UnityPluginInterfaceBase::~UnityPluginInterfaceBase()
		{
		}
#ifndef PROP_VALUE_MAX
#define PROP_VALUE_MAX 96
#endif
		bool UnityPluginInterfaceBase::MojingInit(__tagUnityInterfaceInitParams *pInitParameters)
		{
			char Brand[PROP_VALUE_MAX], Model[PROP_VALUE_MAX], Serial[PROP_VALUE_MAX];
			char szMerchantID[PROP_VALUE_MAX], szAppID[PROP_VALUE_MAX], szAppKey[PROP_VALUE_MAX];
			char szChannelID[PROP_VALUE_MAX], szProfilePath[260];
            char szAppName[PROP_VALUE_MAX];
            char szPackageName[PROP_VALUE_MAX];
            char szUserID[PROP_VALUE_MAX];
			bool bRet = false;
			int nWidth = 0, nHeight = 0;
			float xdpi = 0, ydpi = 0;
			float scale = 1;
			nWidth = pInitParameters->m_iWidth;
			nHeight = pInitParameters->m_iHeight;
			xdpi = pInitParameters->m_fXDPI;
			ydpi = pInitParameters->m_fYDPI;

#ifdef MJ_OS_ANDROID	
			// 注意：因为Android的JAR包里面会自动的Init，所以这里直接返回就好了
			return true;
			AndroidProperties::GetBrand(Brand);
			MOJING_TRACE(g_APIlogger, "Brand: " << Brand);
			AndroidProperties::GetModel(Model);
			MOJING_TRACE(g_APIlogger, "Model: " << Model);
			AndroidProperties::GetSerialNo(Serial);
			MOJING_TRACE(g_APIlogger, "Serial: " << Serial);

			strcpy(szAppName , g_pAPPName);
			MOJING_TRACE(g_APIlogger, "AppName: " << szAppName);
			strcpy(szPackageName , g_pPacketName);
			MOJING_TRACE(g_APIlogger, "PackageName: " << szPackageName);
			strcpy(szUserID , g_pUserID);

			strcpy(szMerchantID , pInitParameters->m_szMerchantID.c_str());
			strcpy(szAppID , pInitParameters->m_szAppID.c_str());
			strcpy(szAppKey, pInitParameters->m_szAppKey.c_str());

			strcpy(szChannelID, pInitParameters->m_szChannelID.c_str());
			strcpy(szProfilePath, pInitParameters->m_szProfilePath.c_str());


#elif defined(MJ_OS_WIN32)
			strcpy(Brand ,"PC");
			strcpy(Model, "X86");
			strcpy(Serial, "0000001");
			nWidth = 1920;
			nHeight = 1080;
			xdpi = 320;
			ydpi = 320;
			strcpy(szAppName, "UNKNOWN");
			strcpy(szPackageName, "UNKNOWN");
			strcpy(szUserID, "USERID00000001");
			strcpy(szMerchantID , pInitParameters->m_szMerchantID.c_str());
			strcpy(szAppID , pInitParameters->m_szAppID.c_str());
			strcpy(szAppKey, pInitParameters->m_szAppKey.c_str());

			strcpy(szChannelID, pInitParameters->m_szChannelID.c_str());
			strcpy(szProfilePath, pInitParameters->m_szProfilePath.c_str());
#elif defined(MJ_OS_IOS)
			strcpy(Brand, (char *)GetMobileBrand());
			strcpy(Model, (char *)GetMobileInnerModelName());
			strcpy(Serial, (char *)GetMobileSerialNumber());
			bRet = GetMobileScreenSize(nWidth, nHeight, xdpi, ydpi, scale, true);
			if (!bRet)
			{
				return  false;
			}
			MOJING_TRACE(g_APIlogger, "Screen Size: " << nWidth << " X " << nHeight);
			MOJING_TRACE(g_APIlogger, "Screen DPI:  " << xdpi << " X " << ydpi);
			
			strcpy(szMerchantID , pInitParameters->m_szMerchantID.c_str());
			strcpy(szAppID , pInitParameters->m_szAppID.c_str());
			strcpy(szAppKey, pInitParameters->m_szAppKey.c_str());
			strcpy(szChannelID, pInitParameters->m_szChannelID.c_str());
			
			strcpy(szAppName, GetCurAppVersion());
			strcpy(szPackageName, GetCurPackageName());
			strcpy(szUserID, GetUserID());
            const char* path = GetSDKAssetsDirPath();
            if(path != NULL)
                strcpy(szProfilePath, path);
#endif


			bRet = MojingSDK_Init(nWidth, nHeight,
				xdpi, ydpi,
				Brand, Model, Serial,
				szMerchantID, szAppID, szAppKey,
				szAppName, szPackageName, szUserID,
				szChannelID,
				szProfilePath);
#ifdef MJ_OS_IOS
			Manager* pManager = Manager::GetMojingManager();
			if (pManager)
			{
				MojingDisplayParameters *pDisplay = pManager->GetParameters()->GetDisplayParameters();
				if (pDisplay)
					pDisplay->SetScale(scale);
			}
#endif
			MOJING_TRACE(g_APIlogger, "Unity_Init. ret: " << bRet);
			return bRet;
		}

		bool UnityPluginInterfaceBase::Init(__tagUnityInterfaceInitParams *pInitParameters)
		{ 
			MOJING_FUNC_TRACE(g_APIlogger);
			SetInitParameters(*pInitParameters); 
			return true;
		}
		bool UnityPluginInterfaceBase::CanEnterMojingWorld()
		{
			return true;
		}
		bool UnityPluginInterfaceBase::CreateUnityPlugin(__tagUnityInterfaceInitParams *pInitParameters)
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			if (g_pUnityPlugin == NULL)
			{
				MojingInit(pInitParameters);
				// g_pUnityPlugin = new UnityPluginInterfaceBase;
#ifdef MJ_OS_ANDROID
				MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
				MachineListNode CurrentMachineType = pDeviceParameters->GetCurrentMachine();

				if (pDeviceParameters->GetAbility() & DEVICE_ABILITY_SVR && (pDeviceParameters->GetCurrentMachine().m_iID == 2))
				{
					MOJING_TRACE(g_APIlogger, "Create UnityPluginInterfaceQ820");
					g_pUnityPlugin = new UnityPluginInterfaceQ820;
				}
				if (g_pUnityPlugin)
				{
					if (!g_pUnityPlugin->Init(pInitParameters))
					{
#ifndef MJ_OS_WIN32 // 注意：这里不支持Win32原因不详
						MOJING_TRACE(g_APIlogger, "UnityPlugin Init Error!! Plugin = " << g_pUnityPlugin->GetClassName().c_str());
#endif
						delete g_pUnityPlugin;
						g_pUnityPlugin = NULL;
					}
					else
					{
#ifndef MJ_OS_WIN32 // 注意：这里不支持Win32原因不详
						MOJING_TRACE(g_APIlogger, "Using CurrentMachineType.m_iID = " << CurrentMachineType.m_iID << " , pUnityPlugin = " << g_pUnityPlugin->GetClassName().c_str());
#endif
					}
				}
				else
				{
					MOJING_TRACE(g_APIlogger, "Not Machine");
				}

#endif
				if (g_pUnityPlugin == NULL)
				{// 不是一体机或者一体机初始化失败，使用默认初始化
					MOJING_TRACE(g_APIlogger, "Not Machine or Create machine plugin faild");
#ifdef MJ_OS_ANDROID
					g_pUnityPlugin = new UnityPluginInterfaceBase;
#elif defined(MJ_OS_WIN32)
					g_pUnityPlugin = new UnityPluginInterfaceWin32;
#elif defined(MJ_OS_IOS)
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
					if (g_UnityPluginStatus.GetDeviceType() == kUnityGfxRendererMetal)
						g_pUnityPlugin = new UnityPluginInterface_iOSMetal;
					else
#endif //#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
						g_pUnityPlugin = new UnityPluginInterface_iOSGL;
#endif


					if (g_pUnityPlugin && !g_pUnityPlugin->Init(pInitParameters))
					{
#ifndef MJ_OS_WIN32 // 注意：这里不支持Win32原因不详
						MOJING_ERROR(g_APIlogger, "UnityPlugin Init Error!! Plugin = " << g_pUnityPlugin->GetClassName());
#endif
						delete g_pUnityPlugin;
						g_pUnityPlugin = NULL;
					}
				}

				if (g_pUnityPlugin)
				{
#ifndef MJ_OS_WIN32 // 注意：这里不支持Win32原因不详
					MOJING_TRACE(g_APIlogger, "Create UnityPlugin:" << g_pUnityPlugin->GetClassName().c_str());
#endif
					g_pUnityPlugin->SetEngineVersion(pInitParameters->m_szEngineVersion.c_str());
				}
				else
				{
					MOJING_ERROR(g_APIlogger, "Can not create UnityPlugin");
				}
			}
			return g_pUnityPlugin != NULL;
		}
		UnityPluginInterfaceBase* UnityPluginInterfaceBase::GetUnityPluginEventObj()
		{
			return g_pUnityPlugin;
		}
		void UnityPluginInterfaceBase::OnEvent_CheckTimeWarpState()
		{			
			if (m_TimeWarpStatus.m_bTimeWarpTypeChanged)
			{
#if USING_LOCKER
				Mutex::Locker Locker(&m_TimeWarpStatusLocker);
#endif
				m_TimeWarpStatus.m_bTimeWarpTypeChanged = false;
				MojingSDK_SetEnableTimeWarp(m_TimeWarpStatus.m_bTimeWarpType);
			}
		}
		void UnityPluginInterfaceBase::OnEvent_SetOverlay(MojingRenderBase* pRender)
		{
			pRender->SetOverlayTextureID((long)m_LeftOverlay.m_iTextureID, (long)m_RightOverlay.m_iTextureID);
#ifdef _DEBUG
			//MOJING_TRACE(g_APIlogger, "OnEvent_SetOverlay -- Src = { " << m_LeftOverlay.m_Rect.x << " , " << m_LeftOverlay.m_Rect.y << " , " << m_LeftOverlay.m_Rect.w << " , " << m_LeftOverlay.m_Rect.h << " } , DistanceInMetre = " << m_LeftOverlay.m_fDistanceInMetre);
#endif
 			MojingSDK_SetOverlayPosition3D_V2(TEXTURE_LEFT_EYE, m_LeftOverlay.m_Rect.x, m_LeftOverlay.m_Rect.y, m_LeftOverlay.m_Rect.w, m_LeftOverlay.m_Rect.h, m_LeftOverlay.m_fDistanceInMetre);
 			MojingSDK_SetOverlayPosition3D_V2(TEXTURE_RIGHT_EYE, m_RightOverlay.m_Rect.x, m_RightOverlay.m_Rect.y, m_RightOverlay.m_Rect.w, m_RightOverlay.m_Rect.h, m_RightOverlay.m_fDistanceInMetre);
		}
	
		void UnityPluginInterfaceBase::OnEvent_SetTextureID()
		{
			OnEvent_CheckTimeWarpState();
			MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
			if (pRender)
			{
				MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
				if (pStatus->GetVerfiyStatus() != VERIFY_OK)
				{
					pRender->SetEyeTexID(0, 0);
					pRender->SetOverlayTextureID(0, 0);
				}
				else
				{
					__tagUnityTexture TextureID = GetTextureID();
					pRender->SetEyeTexID((int)TextureID.m_iLeftTextureID, (int)TextureID.m_iRightTextureID);
					OnEvent_SetOverlay(pRender);
					OnEvent_SetCenterLine();
				}
			}// pRender
		}

		void UnityPluginInterfaceBase::OnEvent_EnterMojingWorld()
		{
			MOJING_FUNC_TRACE(g_APIlogger); 
			if (GetIsInMojingWorld())
			{
				MOJING_TRACE(g_APIlogger , "Call OnEvent_EnterMojingWorld again!");
				return;
			}
			MojingSDK_SetEngineVersion(m_strEngineVersion.c_str());
			bool bRet = MojingSDK_EnterMojingWorld(m_strGlassName.c_str(), false, m_TimeWarpStatus.m_bTimeWarpType);
			if (bRet)
			{
				m_TimeWarpStatus.m_bTimeWarpTypeChanged = false;
				SetIsInMojingWorld(true);

				// 注意：Resume的时候不会额外调用SetCenterLine，所以无论如何这里要重新设置一下CenterLine。
				m_CenterLine.m_bChanged = true;
				MOJING_TRACE(g_APIlogger, "MojingSDK_EnterMojingWorld = " << bRet);
			}
		}
		void UnityPluginInterfaceBase::OnEvent_ChangeMojingWorld()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			MojingSDK_ChangeMojingWorld(m_strGlassName.c_str());
		}
		void UnityPluginInterfaceBase::OnEvent_LeaveMojingWorld()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			MojingSDK_LeaveMojingWorld();
			SetIsInMojingWorld(false);
		}

		void UnityPluginInterfaceBase::OnEvent_DestroyMojingWorld()
		{
			// MOJING_TRACE(g_APIlogger , "UnityPluginInterfaceBase::OnEvent_DestroyMojingWorld");
		}

		void UnityPluginInterfaceBase::OnEvent_DistortFrame()
		{
#ifdef MJ_OS_ANDROID
			GLStateSave SaveGLState;
			// WORKAROUND: On Mali with static-batching enabled, Unity leaves
			// ibo mapped entire frame. When we inject our vignette and timewarp
			// rendering with the ibo mapped, rendering corruption will occur.
			// Explicitly unbind here.
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			// Discard the depth buffer, so the tiler won't need to write it back out to memory
			GL_InvalidateFramebuffer(INV_FBO, false, true);
			// glFlush();
#endif
			MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
			if (pRender)
			{
				pRender->WarpToScreen();// 提交绘制完的场景
			}
		}

		void UnityPluginInterfaceBase::OnEvent_SetCenterLine()
		{
			if (m_CenterLine.m_bChanged)
			{
				m_CenterLine.m_bChanged = false;
				MojingSDK_SetCenterLine(
					m_CenterLine.m_iWidth,
					m_CenterLine.m_Color.m_R,
					m_CenterLine.m_Color.m_G,
					m_CenterLine.m_Color.m_B,
					m_CenterLine.m_Color.m_A);
			}
		}

		/*以下是对外提供的响应*/
		bool UnityPluginInterfaceBase::IsInMojingWorld(const char * szGlassesName)
		{
			//MOJING_FUNC_TRACE(g_APIlogger); 
			const char* pGlassesNow = MojingSDK_GetGlasses();
			bool bRet = (strcmp(pGlassesNow, szGlassesName) == 0);
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger , "IsInMojingWorld = " << bRet);
#endif
			return  bRet && GetIsInMojingWorld();
		}

		bool UnityPluginInterfaceBase::IsGlassesNeedDistortion()
		{
			// MOJING_FUNC_TRACE(g_APIlogger); 
			return MojingSDK_IsGlassesNeedDistortion();
		}

		bool UnityPluginInterfaceBase::IsGlassesNeedDistortionByName(const char * szGlassesName)
		{
			// MOJING_FUNC_TRACE(g_APIlogger); 
			return MojingSDK_IsGlassesNeedDistortionByName(szGlassesName);
		}

// 		bool UnityPluginInterfaceBase::SetEngineVersion(const char* lpszEngine)
// 		{
// 			return MojingSDK_SetEngineVersion(lpszEngine);
// 		}

		bool UnityPluginInterfaceBase::StartTracker()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			return MojingSDK_StartTracker(m_iSampleFrequence);
		}

		Matrix4f UnityPluginInterfaceBase::GetLastHeadView()
		{
			Matrix4f Ret;
			float f[16];
			MojingSDK_getLastHeadView(f);
			memcpy(&Ret.M[0][0], f, sizeof(float)* 16);
			return Ret;
		}

		Quatf UnityPluginInterfaceBase::GetLastHeadQuarternion()
		{
			Quatf Ret;
			MojingSDK_getLastHeadQuarternion(&Ret.w, &Ret.x, &Ret.y, &Ret.z);
			return Ret;
		}

		void UnityPluginInterfaceBase::StopTracker()
		{
			MojingSDK_StopTracker();
		}

		void UnityPluginInterfaceBase::ResetSensorOrientation()
		{
			MojingSDK_ResetSensorOrientation();
		}

		void UnityPluginInterfaceBase::ResetSensorOrientation2()
		{
			MojingSDK_ResetSensorOrientation2();
		}
		
		void UnityPluginInterfaceBase::ResetTracker()
		{
			MojingSDK_ResetTracker();
		}
	}
}
