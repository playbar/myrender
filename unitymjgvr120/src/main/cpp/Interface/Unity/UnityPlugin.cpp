#ifdef MJ_OS_ANDROID
#include <jni.h>
#include "GlStateSave.h"
#endif//MJ_OS_ANDROID
#include <unistd.h>						// usleep, etc



#ifdef __OBJC__
#include <TargetConditionals.h>
#include "../../Render/Metal/MojingRenderMetal.h"
#include "MetalForwardDecls.h" // UnityRenderTex
/* Unity 5.2, 5.3 Metal Interface */
#include "Unity/UnityInterface.h" // we use 1.UnityGetMetalDevice, 2.UnityRenderBufferMTLTexture

/* Unity 5.4 Metal Interface */
#include "Unity/IUnityGraphicsMetal.h"

#include "MojingMetalView.h"

#include "UnityPluginInterface/UnityPluginInterface_iOSMetal.h"
#endif

#include "../../Base/MojingTypes.h"
#include "../../Base/MojingMath.h"
#include "../../MojingAPI.h"
#include "../../MojingManager.h"
#include "../../MojingSDKStatus.h"
#include "../../Tracker/MojingTracker.h"
#include "../../Base/MojingSystem.h"
#include "../../Platform/MojingPlatformBase.h"
#include "../../Render/MojingRenderBase.h"

#include "../../Profile/GlassesConfigProfile.h"
#include "../../Profile/MobileConfigProfile.h"
#include "../../Reporter/GyroTempCalibrationReporter.h"
#include "UnityPluginInterface/UnityPluginStatus.h"
#include "UnityPluginInterface/UnityPluginInterfaceBase.h"
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#ifndef UNITY_EXPORT
#ifdef MJ_OS_WIN32
#define UNITY_EXPORT __declspec(dllexport)
#else
#define UNITY_EXPORT
#endif
#endif 

using namespace Baofeng;
using namespace Baofeng::Mojing;

//#ifndef MJ_OS_IOS
static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;
#if defined(MJ_OS_IOS)
static IUnityGraphicsMetal* s_GraphicsMetal = NULL;
#endif
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;
static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
//static UNITY_INTERFACE_API void OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
	g_UnityPluginStatus.OnEvent_GraphicsDeviceEvent(eventType);
}

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
//extern "C" UNITY_EXPORT void	UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	g_UnityPluginStatus.Init(unityInterfaces);
	g_UnityPluginStatus.GetUnityGraphics()->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
//extern "C" UNITY_EXPORT void UnityPluginUnload()
{
    g_UnityPluginStatus.GetUnityGraphics()->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
	g_UnityPluginStatus.Release();
}

//#endif

extern "C"
{
	UNITY_EXPORT void Unity_Log(int logLevel, char* info, char* filename, int line)
	{
#ifdef ENABLE_LOGGER
#ifdef MJ_OS_ANDROID
		if (g_APIlogger.m_nLogLevel <= logLevel)
		{
			g_APIlogger.Log(logLevel, info, filename, line);
		}
#else
		log4cplus::Logger const & log = log4cplus::detail::macros_get_logger(g_APIlogger.m_logger);
		if (log.isEnabledFor(logLevel))
		{
			log4cplus::detail::macro_forced_log(log, logLevel, info, filename, line, LOG4CPLUS_MACRO_FUNCTION());
		}
#endif
#endif
	}

	// Note: These must be kept in sync with the Unity RenderEventType
	enum RenderEventType
	{
		EVENT_INIT_RENDERTHREAD,
		EVENT_PAUSE,
		EVENT_RESUME,
		EVENT_LEFTEYE_ENDFRAME,
		EVENT_RIGHTEYE_ENDFRAME,
		EVENT_TIMEWARP,
		EVENT_PLATFORMUI_GLOBALMENU,
		EVENT_PLATFORMUI_CONFIRM_QUIT,
		EVENT_RESET_VRMODEPARMS,
		EVENT_PLATFORMUI_TUTORIAL,
		EVENT_SHUTDOWN_RENDERTHREAD,
		NUM_EVENTS
	};
	enum UnityEventID
	{
		EnterMojingWorld,
		ChangeMojingWorld,
		LeaveMojingWorld,
		DestroyMojingWorld,
		DistortFrame,
		SetTextureID,
		SetCenterLine,
		BackerTexture
	};

	struct _tagUnityVersion
	{
		_tagUnityVersion(){ m_MajorVersion = m_MinorVersion = 0; m_bEnableATW = m_bATW_ON = false; };
		int m_MajorVersion;
		int m_MinorVersion;
		bool m_bEnableATW;	// 当前版本是否可以使用ATW
		bool m_bATW_ON;	// 当前应用是否使用了ATW
	}g_UnityVersion;
	// FIXME: OVR compile time assert doesn't work outside of functions
	//MJ_COMPILER_ASSERT( NUM_EVENTS < up.MAX_EVENTS );


	UNITY_EXPORT float Unity_GetGlassesFOV()
	{
		return MojingSDK_GetFOV();
	}

	UNITY_EXPORT float Unity_GetGlassesSeparation()
	{
		return MojingSDK_GetGlassesSeparation();
	}

	UNITY_EXPORT int Unity_GetGlassesSeparationInPixel()
	{
		return (int)(MojingSDK_GetGlassesSeparationInPix() + 0.5f);
	}

	UNITY_EXPORT void Unity_GetScreenSize(float* pSize)
	{
		MojingSDK_GetScreenSize(pSize);
	}

	UNITY_EXPORT bool Unity_StartTracker(int nSampleFrequence)
	{
		MOJING_FUNC_TRACE(g_APIlogger);
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			pUnityPlugin->SetSampleFrequence(nSampleFrequence);
			pUnityPlugin->StartTracker();
		}
		return false;
	}

	UNITY_EXPORT bool Unity_StartTrackerCalibration()
	{
        return (MojingSDK_StartTrackerCalibration() == 0);
	}

	UNITY_EXPORT float Unity_IsTrackerCalibrated()
	{
        return MojingSDK_IsTrackerCalibrated();
	}

#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
	UNITY_EXPORT int Unity_Device_GetKeymask(int iID, int *pKeyMask)
	{
		return MojingSDK_Device_GetKeymask(iID, pKeyMask);
	}

	UNITY_EXPORT float Unity_Device_GetInfo(int iID/*设备ID*/,
		float *pQuart/*四元数表示的旋转，依次为XYZW*/,
		float *pAngularAccel/*角加速度，依次为XYZ*/,
		float *pLinearAccel/*线加速度，依次为XYZ*/,
		float *pPosition,/*设备的空间位置，以米为单位，默认是0,0,0。*/
		unsigned int *pKeystatus/*设备上的按键状态，默认是0表示没有按键被按下*/)
	{
		//MOJING_FUNC_TRACE(g_APIlogger);
		return MojingSDK_Device_GetCurrentPoaseInfo(iID, pQuart, pAngularAccel, pLinearAccel, pPosition, pKeystatus);
	}

	UNITY_EXPORT float Unity_Device_GetFixInfo(int iID/*设备ID*/,
		float *pQuart/*四元数表示的旋转，依次为XYZW*/,
		float *pAngularAccel/*角加速度，依次为XYZ*/,
		float *pLinearAccel/*线加速度，依次为XYZ*/,
		float *pPosition/*设备的空间位置，以米为单位，默认是0,0,0。*/)
	{
		//MOJING_FUNC_TRACE(g_APIlogger);
		return MojingSDK_Device_GetFixPoaseInfo(iID, pQuart, pAngularAccel, pLinearAccel, pPosition);
	}

    UNITY_EXPORT float Unity_Device_GetFixCurrentInfo(int iID/*设备ID*/,
        float *pQuart/*四元数表示的旋转，依次为XYZW*/,
        float *pAngularAccel/*角加速度，依次为XYZ*/,
        float *pLinearAccel/*线加速度，依次为XYZ*/,
        float *pPosition,/*设备的空间位置，以米为单位，默认是0,0,0。*/
        unsigned int *pKeystatus/*设备上的按键状态，默认是0表示没有按键被按下*/)
    {
        //MOJING_FUNC_TRACE(g_APIlogger);
        return MojingSDK_Device_GetControlFixCurrentInfo(iID, pQuart, pAngularAccel, pLinearAccel, pPosition, pKeystatus);
    }

    UNITY_EXPORT void Unity_Device_GetFixScore(
        int* pStatus,
        int* pScore)
    {
        //MOJING_FUNC_TRACE(g_APIlogger);
        MojingSDK_Device_GetFixScore(pStatus, pScore);
    }
#endif

	UNITY_EXPORT void Unity_getLastHeadView(float* pfViewMatrix)
	{
		// 注意：在启用ATW的时候，这个数据需要和帧陀螺仪对应使用
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			Matrix4f Ret = pUnityPlugin->GetLastHeadView();
			memcpy(pfViewMatrix, &Ret.M[0][0], sizeof(float)* 16);
		}
	}
    
#ifndef MJ_OS_WIN32
	UNITY_EXPORT bool Unity_ktxLoadTextureN(const char* const filename, unsigned int* pTexture, unsigned int* pTarget, int* pHeight, int* pWidth, int* pDepth, bool* pIsMipmapped, unsigned int* pGlerror, int* ktxError)
	{
		return MojingSDK_ktxLoadTextureN(filename, pTexture, pTarget, pHeight, pWidth, pDepth, pIsMipmapped, pGlerror, ktxError);
	}
#endif

	UNITY_EXPORT void Unity_getLastHeadQuarternion(float &w, float &x, float &y, float &z)
	{
        Quatf Ret;
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			Ret = pUnityPlugin->GetLastHeadQuarternion();
		}
		w = Ret.w;
		x = Ret.x;
		y = Ret.y;
		z = Ret.z;
	}

	UNITY_EXPORT void Unity_StopTracker(void)
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			pUnityPlugin->StopTracker();
		}
	}

	UNITY_EXPORT bool Unity_Init(const char* szEngineVersion, const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szChannelID, int nWidth, int nHeight, float xdpi, float ydpi, const char* szProfilePath , void * pOtherParmams)
	{
		__tagUnityInterfaceInitParams InitParams;
//		MOJING_TRACE(g_APIlogger, "EngineVersion = " << szEngineVersion);
		InitParams.m_szEngineVersion = szEngineVersion;
		InitParams.m_szMerchantID = szMerchantID;
		InitParams.m_szAppID = szAppID;
		InitParams.m_szAppKey = szAppKey;
		InitParams.m_szChannelID = szChannelID;
		InitParams.m_szProfilePath = szProfilePath;

		InitParams.m_iWidth = nWidth;
		InitParams.m_iHeight = nHeight;
		InitParams.m_fXDPI = xdpi;
		InitParams.m_fYDPI = ydpi;
		InitParams.m_pParams = pOtherParmams;
		return UnityPluginInterfaceBase::CreateUnityPlugin(&InitParams);
		
	}

	UNITY_EXPORT bool Unity_OnSurfaceChanged(int newWidth , int newHeight)
	{
        MOJING_FUNC_TRACE(g_APIlogger);
        return MojingSDK_OnSurfaceChanged(newWidth, newHeight);
	}

	// 设定标准Overlay，这种Overlay允许用户指定位置
	UNITY_EXPORT void Unity_SetOverlay(void* iLeftOverlayTextureID, void* iRightOverlayTextureID, float fLeft, float fTop, float fWidth, float fHeight)
	{
//        MOJING_FUNC_TRACE(g_APIlogger);
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			__tagUnityOverlay Overlay;
			Overlay.m_Rect = Rectf(fLeft, fTop, fWidth, fHeight);
			Overlay.m_fDistanceInMetre = 0;
			/************************************************************************/
			/* 注意：这种情况下不能区分是Metal纹理还是普通的Unity纹理			    */
			/************************************************************************/
			Overlay.m_mtlTextureID = g_UnityPluginStatus.ConvertTexture(Overlay.m_iTextureID, iLeftOverlayTextureID);
			pUnityPlugin->SetLeftOverlay(Overlay);
			
			Overlay.m_mtlTextureID = g_UnityPluginStatus.ConvertTexture(Overlay.m_iTextureID, iRightOverlayTextureID);
			pUnityPlugin->SetRightOverlay(Overlay);
		}
	}
        
	UNITY_EXPORT void Unity_SetOverlay3D_V2(int iEyeType, void* iTextureID, float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre)
	{
//        MOJING_FUNC_TRACE(g_APIlogger);
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			__tagUnityOverlay Overlay;
			Overlay.m_mtlTextureID = g_UnityPluginStatus.ConvertTexture(Overlay.m_iTextureID, iTextureID, (EyeTextureType)iEyeType);

			Overlay.m_Rect = Rectf(fLeft, fTop, fWidth, fHeight);
			Overlay.m_fDistanceInMetre = fDistanceInMetre;

#ifdef _DEBUG
			//MOJING_TRACE(g_APIlogger, "Unity_SetOverlay3D -- Src = { " << Overlay.m_Rect.x << " , "
			//	<< Overlay.m_Rect.y << " , " << Overlay.m_Rect.w << " , " << Overlay.m_Rect.h <<
			//	" } , DistanceInMetre = " << Overlay.m_fDistanceInMetre);
#endif
			
			if (iEyeType & TEXTURE_LEFT_EYE)
				pUnityPlugin->SetLeftOverlay(Overlay);
			if (iEyeType & TEXTURE_RIGHT_EYE)
				pUnityPlugin->SetRightOverlay(Overlay);
		}
		return;
	}

    // 设定标准Overlay，这种Overlay允许用户指定位置
	UNITY_EXPORT void Unity_SetOverlay3D(int iEyeType , void* iTextureID, float fWidth, float fHeight, float fDistanceInMetre)
	{
//        MOJING_FUNC_TRACE(g_APIlogger);
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			__tagUnityOverlay Overlay;
			Overlay.m_mtlTextureID = g_UnityPluginStatus.ConvertTexture(Overlay.m_iTextureID, iTextureID, (EyeTextureType)iEyeType);

			Overlay.m_Rect = Rectf(0.5 - fWidth / 2, 0.5 - fHeight / 2, fWidth, fHeight);
			Overlay.m_fDistanceInMetre = fDistanceInMetre;
#ifdef _DEBUG
			//MOJING_TRACE(g_APIlogger, "Unity_SetOverlay3D -- Src = { " << Overlay.m_Rect.x << " , " 
			//	<< Overlay.m_Rect.y << " , " << Overlay.m_Rect.w << " , " << Overlay.m_Rect.h <<
			//	" } , DistanceInMetre = " << Overlay.m_fDistanceInMetre);
#endif
			
			if (iEyeType & TEXTURE_LEFT_EYE)
				pUnityPlugin->SetLeftOverlay(Overlay);
			if (iEyeType & TEXTURE_RIGHT_EYE)
				pUnityPlugin->SetRightOverlay(Overlay);
		}
        else
        {
            MOJING_ERROR(g_APIlogger, "pUnityPlugin is null");
        }
		return;
	}

	int g_iFrameCount = 0;
	UNITY_EXPORT void Unity_SetFremeCount(int iFreamCount)
	{
		g_iFrameCount = iFreamCount;
	}
	UNITY_EXPORT void Unity_SetTextureID(void* iLeftTextureID, void* iRightTextureID)
	{
        //MOJING_FUNC_TRACE(g_APIlogger);
		UnityPluginInterfaceBase* pUnityPlugin =UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			__tagUnityTexture UnityTexture;
			static EyeTextureType TextureType = TEXTURE_OPEN_GL;
			if (g_UnityPluginStatus.GetDeviceType() == kUnityGfxRendererMetal)
			{
				TextureType = TEXTURE_IS_UNITY_METAL_RENDER_TEXTURE;
			}
			UnityTexture.m_mtlLeftTextureID = g_UnityPluginStatus.ConvertTexture(
				UnityTexture.m_iLeftTextureID, iLeftTextureID, TextureType);

			UnityTexture.m_mtlRightTextureID = g_UnityPluginStatus.ConvertTexture(
				UnityTexture.m_iRightTextureID, iRightTextureID, TextureType);

			UnityTexture.m_ValueFlag = (__tagUnityTexture::VF)(__tagUnityTexture::TEXTURE_LEFT | __tagUnityTexture::TEXTURE_RIGHT);

			//MOJING_TRACE(g_APIlogger, "-- HX -- Set Texture ID : " << UnityTexture.m_iLeftTextureID << " , FrameCount = "<< g_iFrameCount);

			pUnityPlugin->SetTextureID(UnityTexture);
		}
		return;
	}

	UNITY_EXPORT void Unity_ResetSensorOrientation()
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			pUnityPlugin->ResetSensorOrientation();
		}
	}

	UNITY_EXPORT void Unity_ResetSensorOrientation2()
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			pUnityPlugin->ResetSensorOrientation2();
		}
	}

	UNITY_EXPORT void Unity_ResetTracker(void)
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			pUnityPlugin->ResetTracker();
		}
	}

	UNITY_EXPORT int Unity_GetTextureSize(void)
	{
        return MojingSDK_GetTextureSize();
	}

	char g_sGlassesName[128] = "Mojing3";
	bool g_MultithreadingType = false;
	bool  g_TimeWarpType = false;
	bool  g_TimeWarpTypeChanged = false;
	UNITY_EXPORT bool Unity_SetMojingWorld(const char * szGlassesName, bool bTimeWarp , void * pAndroidActivity)
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			if (szGlassesName)
				pUnityPlugin->SetGlassName(szGlassesName);
			else
				pUnityPlugin->SetGlassName("UNKNOWN");
			__tagTimeWarpStatus TimeWarpStatus;
			TimeWarpStatus.m_bTimeWarpType = bTimeWarp;
			TimeWarpStatus.m_bTimeWarpTypeChanged = false;
			pUnityPlugin->SetTimeWarpStatus(TimeWarpStatus);
#ifdef MJ_OS_ANDROID
			extern JavaVM* gJavaVM;
			__tagUnityInterfaceInitParams InitParams = pUnityPlugin->GetInitParameters();
			JNIEnv* jniEnv;
			gJavaVM->AttachCurrentThread(&jniEnv, 0);
			void *pOldParams = InitParams.m_pParams;
			
			if (pAndroidActivity)
			{
				InitParams.m_pParams = jniEnv->NewGlobalRef((jobject)pAndroidActivity);
			}
			char szLog[1024];
			sprintf(szLog, "Change Activity %08X --> %08X", (unsigned int)pOldParams, (unsigned int)InitParams.m_pParams);
			MOJING_TRACE(g_APIlogger, szLog);

			pUnityPlugin->SetInitParameters(InitParams);
#endif
		}
		return true;
	}

	UNITY_EXPORT void Unity_SetEnableTimeWarp(bool bEnable)
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			__tagTimeWarpStatus TimeWarpStatus;
			TimeWarpStatus.m_bTimeWarpTypeChanged = true;
			TimeWarpStatus.m_bTimeWarpType = bEnable;
			pUnityPlugin->SetTimeWarpStatus(TimeWarpStatus);
		}
	}
	UNITY_EXPORT bool Unity_CanEnterMojingWorld()
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			return pUnityPlugin->CanEnterMojingWorld();
		}
		return false;
	}
	UNITY_EXPORT bool Unity_IsInMojingWorld(const char * szGlassesName)
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			return pUnityPlugin->IsInMojingWorld(szGlassesName);
		}
		return false;
	}

	UNITY_EXPORT bool Unity_EnterMojingWorld(const char * szGlassesName, bool bTimewarp, void* pOtherParams)
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			pUnityPlugin->SetGlassName(szGlassesName);
		}
		return true;
	}
    
	UNITY_EXPORT bool Unity_ChangeMojingWorld(const char * szGlassesName)
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			pUnityPlugin->SetGlassName(szGlassesName);
		}
		return true;
		//return MojingSDK_ChangeMojingWorld(szGlassesName);
	}

	UNITY_EXPORT bool Unity_IsGlassesNeedDistortion(void)
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			return pUnityPlugin->IsGlassesNeedDistortion();
		}
		return false;// 连控制器都没有，自己画吧
	}

	UNITY_EXPORT bool Unity_IsGlassesNeedDistortionByName(const char * szGlassesName)
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			return pUnityPlugin->IsGlassesNeedDistortionByName(szGlassesName);
		}
		return false;// 连控制器都没有，自己画吧
	}


	UNITY_EXPORT void Unity_SetCenterLine(int iWidth, int colR, int colG, int colB, int colA)
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			__tagCenterLine Line(colR, colG, colB, colA , iWidth);
			pUnityPlugin->SetCenterLine(Line);
		}
	}



	UNITY_EXPORT void Unity_FreeString(char * pReleaseString)
	{
		//MOJING_FUNC_TRACE(g_APIlogger);
		if (pReleaseString)
			free(pReleaseString);
	}
	/*工具函数，分配空间并生成一个String对象的副本*/
	const char* Unity_AllocString(const String &sReleaseString)
	{
		char* pRet = (char*)malloc(sReleaseString.GetSize() + 1);
		strcpy(pRet, sReleaseString.ToCStr());
		return pRet;
	}

	UNITY_EXPORT const char* Unity_GetDefaultMojingWorld(const char* strLanguageCodeByISO639)
	{
		return Unity_AllocString(MojingSDK_GetDefaultMojingWorld(strLanguageCodeByISO639));
	}

	UNITY_EXPORT const char* Unity_GetLastMojingWorld(const char* strLanguageCodeByISO639)
	{
        return Unity_AllocString(MojingSDK_GetLastMojingWorld(strLanguageCodeByISO639));
	}

	UNITY_EXPORT const char* Unity_GetManufacturerList(const char* strLanguageCodeByISO639)
	{
		return Unity_AllocString(MojingSDK_GetManufacturerList(strLanguageCodeByISO639));
	}

	UNITY_EXPORT const char* Unity_GetProductList(const char* strManufacturerKey, const char* strLanguageCodeByISO639)
	{
		return Unity_AllocString(MojingSDK_GetProductList(strManufacturerKey, strLanguageCodeByISO639));
	}

	UNITY_EXPORT const char* Unity_GetGlassList(const char* strProductKey, const char* strLanguageCodeByISO639)
	{
		return Unity_AllocString(MojingSDK_GetGlassList(strProductKey, strLanguageCodeByISO639));
	}

	UNITY_EXPORT const char* Unity_GetGlassInfo(const char* strGlassKey, const char* strLanguageCodeByISO639)
	{
		return Unity_AllocString(MojingSDK_GetGlassInfo(strGlassKey, strLanguageCodeByISO639));
	}

	UNITY_EXPORT const char* Unity_GenerationGlassKey(const char* strProductQRCode, const char* strGlassQRCode)
	{
		return Unity_AllocString(MojingSDK_GenerationGlassKey(strProductQRCode, strGlassQRCode));
	}

	UNITY_EXPORT const char* Unity_GetSDKVersion()
	{
		return Unity_AllocString(MojingSDK_GetSDKVersion());
	}


	UNITY_EXPORT const char* Unity_GetUserSettings()
	{
        //MOJING_FUNC_TRACE(g_APIlogger);
        return Unity_AllocString(MojingSDK_GetUserSettings());
	}

	UNITY_EXPORT bool Unity_SetUserSettings(const char * sUserSettings)
	{
        //MOJING_FUNC_TRACE(g_APIlogger);
        return MojingSDK_SetUserSettings(sUserSettings);
	}
	struct backerInfo
	{
		unsigned int texid;
		unsigned int x;
		unsigned int y;
		unsigned int width;
		unsigned int  height;
		unsigned int  desX;
		unsigned int  desY;
	};

	struct UnityBacker
	{
		UnityBacker()
		:g_width(0)
		,g_height(0)
		{
		}
		int g_width;
		int g_height;
		std::vector<backerInfo> vInfo;
	}g_UnityBacker;
	bool g_bIsFinishBaker = false;
	UNITY_EXPORT bool Unity_GetBackerTexID(int* pTexID)
	{
        MOJING_FUNC_TRACE(g_APIlogger);
        
#ifndef DO_NOT_USING_TEXTURE_BACKER
		*pTexID = 0;
	
		if (g_bIsFinishBaker)
		{
			MojingSDK_GetBackerTexID(pTexID);
		}

		return true;
#else
		return false;
#endif
	}
	UNITY_EXPORT void Unity_SetStartBacker(int width, int height, backerInfo * pInfo, int Infolength)
	{
     
#ifndef DO_NOT_USING_TEXTURE_BACKER
		MOJING_FUNC_TRACE(g_APIlogger);
		g_bIsFinishBaker = false;

		g_UnityBacker.vInfo.clear();
		g_UnityBacker.g_width = width;
		g_UnityBacker.g_height = height;
		for (int i = 0; i < Infolength; ++i)
		{
			g_UnityBacker.vInfo.push_back(*(pInfo + i));
		}
#endif
	}

	UNITY_EXPORT void Unity_SetBackerTexture()
	{        
#ifndef DO_NOT_USING_TEXTURE_BACKER
		MOJING_FUNC_TRACE(g_APIlogger);

		if (g_UnityBacker.vInfo.size() <= 0)
		{
			return;
		}
		MojingSDK_StartBacker(g_UnityBacker.g_height, g_UnityBacker.g_width);
		for (std::vector<backerInfo>::iterator it = g_UnityBacker.vInfo.begin(); it != g_UnityBacker.vInfo.end(); ++it)
		{
			MojingSDK_BackerTexture(it->texid, it->x, it->y, it->width, it->height, it->desX, it->desY);
		}

		g_bIsFinishBaker = true;
#endif
	}
	

	// When Unity's multi-threaded renderer is enabled, the GL context is never current for
	// the script execution thread, so the only way for a plugin to execute GL code is to
	// have it done through the GL.IssuePluginEvent( int ) call, which calls this function.
    static void OnRenderEvent(int eventID)
    {
#ifdef _DEBUG
		//MOJING_TRACE(g_APIlogger , "Event ID = " << eventID);
#endif

		UnityPluginInterfaceBase *pUnityPlugin =        UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin == NULL)
		{
			MOJING_ERROR(g_APIlogger, "OnRenderEvent : Not init MojingSDK!!Event ID = " << eventID);
			return;
		}
        // MOJING_TRACE(g_APIlogger, "UnityRenderEvent with " << eventID);
        Manager* pManager = Manager::GetMojingManager();
        if (pManager)
        {
            switch (eventID)
            {
                case DistortFrame:
                {
									 //#ifdef _DEBUG
//                    MOJING_TRACE(g_APIlogger, "DistortFrame");
									 //#endif
					pUnityPlugin->OnEvent_DistortFrame();
                }
                    break;
                    
                case EnterMojingWorld:
					pUnityPlugin->OnEvent_EnterMojingWorld();
                    break;
                    
                case ChangeMojingWorld:
					MOJING_TRACE(g_APIlogger, "ChangeMojingWorld: " << g_sGlassesName);
					pUnityPlugin->OnEvent_ChangeMojingWorld();
                    break;
                    
                case LeaveMojingWorld:
					MOJING_TRACE(g_APIlogger, "LeaveMojingWorld");
					pUnityPlugin->OnEvent_LeaveMojingWorld();
                    break;
                    
                case SetTextureID:
                {
                    // MOJING_TRACE(g_APIlogger, "SetTextureID");
					pUnityPlugin->OnEvent_SetTextureID();
				}
                    break;
                    
                case BackerTexture:
                {
#ifndef DO_NOT_USING_TEXTURE_BACKER
                    Unity_SetBackerTexture();
#endif
                }
                    break;
                case SetCenterLine:
					// EVENT 模式下不用这个
			      break;
				case DestroyMojingWorld:
				{
					pUnityPlugin->OnEvent_DestroyMojingWorld();
                    break;
				}
                default:
                    break;
            }
        }
        return;
    }


#define EYE_LEFT	0
#define EYE_CENTER	1
#define EYE_RIGHT	2


	void GetTanAngles(float* result)
	{
		// Tan-angles from the max FOV.
		float halfFOV = Unity_GetGlassesFOV() / 2;
		float fovLeft = tan(-halfFOV * PI / 180);
		float fovTop = tan(halfFOV * PI / 180);
		float fovRight = tan(halfFOV * PI / 180);
		float fovBottom = tan(-halfFOV * PI / 180);

		if (Unity_IsGlassesNeedDistortion())
		{
			// 1:1 view
		}
		else
		{
		}
		result[0] = fovLeft;
		result[1] = fovTop;
		result[2] = fovRight;
		result[3] = fovBottom;

//		float ipd = Unity_GetGlassesSeparation();

		// Viewport size.
		float screen[2];
		Unity_GetScreenSize(screen);
//		float halfWidth = screen[0] / 4;
//		float halfHeight = screen[1] / 2;
		// Viewport center, measured from left lens position.
//		float centerX = ipd / 2 - halfWidth;
	}

	UNITY_EXPORT bool Unity_SetEngineVersion(const char* lpszEngine)
	{
		// 注意：这个函数不能消除。因为在切到登陆/支付逻辑的时候会把Engine切换到Android模式
		// 需要使用这个接口切换回来
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			pUnityPlugin->SetEngineVersion(lpszEngine);
		}
		return MojingSDK_SetEngineVersion(lpszEngine);
	}

	UNITY_EXPORT bool Unity_IsLowPower()
	{
        // MOJING_FUNC_TRACE(g_APIlogger);
        return MojingSDK_IsLowPower();
	}
	UNITY_EXPORT float Unity_GetDistortionR()
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			return MojingSDK_GetDistortionR(pUnityPlugin->GetGlassName().c_str());
		}
		return 0;
	}

	void Unity_GetProjectionMatrix(int eye, bool bVrMode, float fFOV, float fNear, float fFar, float* pfProjectionMatrix, int* pfViewRect)
	{
        // MOJING_FUNC_TRACE(g_APIlogger);
        MojingSDK_GetProjectionMatrix(eye, bVrMode, fFOV, fNear, fFar, pfProjectionMatrix, pfViewRect);
	}

	UNITY_EXPORT void Unity_AppSetContinueInterval(int interval)
	{
        // MOJING_FUNC_TRACE(g_APIlogger);
        MojingSDK_AppSetContinueInterval(interval);
	}

	UNITY_EXPORT void Unity_AppSetReportInterval(int interval)
	{
       // MOJING_FUNC_TRACE(g_APIlogger);
        MojingSDK_AppSetReportInterval(interval);
	}

	UNITY_EXPORT void Unity_AppSetReportImmediate(bool bImmediate)
	{
        // MOJING_FUNC_TRACE(g_APIlogger);
        MojingSDK_AppSetReportImmediate(bImmediate);
	}

	UNITY_EXPORT void Unity_AppPageStart(const char* szPageName)
	{
        // MOJING_FUNC_TRACE(g_APIlogger);
        MojingSDK_AppPageStart(szPageName);
	}

	UNITY_EXPORT void Unity_AppPageEnd(const char* szPageName)
	{
        // MOJING_FUNC_TRACE(g_APIlogger);
        MojingSDK_AppPageEnd(szPageName);
	}

	UNITY_EXPORT void Unity_AppSetEvent(const char* szEventName, const char* szEventChannelID, const char* szEventInName, float eInData, const char* szEventOutName, float eOutData)
	{
        // MOJING_FUNC_TRACE(g_APIlogger);
        MojingSDK_AppSetEvent(szEventName, szEventChannelID, szEventInName, eInData, szEventOutName, eOutData);
	}
    
    UNITY_EXPORT const char* Unity_AppGetUserID()
    {
		// MOJING_FUNC_TRACE(g_APIlogger);
        return MojingPlatformBase::GetPlatform()->GetUserID();
    }
    
	UNITY_EXPORT const char* Unity_AppGetRunID()
	{
		// MOJING_FUNC_TRACE(g_APIlogger);
        return MojingPlatformBase::GetPlatform()->GetRunID();
	}

	UNITY_EXPORT int  Unity_GetMojingWorldDistortionMesh(const char * szGlassesName, int iScreenWidth, int iScreenHeight, int iWidthCells, int iHeightCells,
		float *pFOV, float *pGlassesSeparationInMeter,
		float * pVerts, float * pUV, int * pIndices)
	{
		//  MOJING_FUNC_TRACE(g_APIlogger);
        return MojingSDK_GetDistortionMesh(szGlassesName, iScreenWidth, iScreenHeight, iWidthCells, iHeightCells,
			pFOV, pGlassesSeparationInMeter,
			pVerts, pUV, pIndices);

	}

	UNITY_EXPORT void Unity_AppReportLog(int iLogType, const char* szTypeName, const char* szLogContent)
	{
		// MOJING_FUNC_TRACE(g_APIlogger);
        MojingSDK_ReportLog(iLogType, szTypeName, szLogContent, false);
	}

	UNITY_EXPORT void Unity_AppResume()
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			pUnityPlugin->AppResume();
		}
	}

	UNITY_EXPORT void Unity_AppPause()
	{
		UnityPluginInterfaceBase* pUnityPlugin = UnityPluginInterfaceBase::GetUnityPluginEventObj();
		if (pUnityPlugin)
		{
			pUnityPlugin->AppPause();
		}
	}

#if defined(MJ_OS_WIN32) || defined(MJ_OS_IOS)
	UNITY_EXPORT void UnitySetGraphicsDevice(void* device, int deviceType, int eventType)
	{
		MOJING_TRACE(g_APIlogger, "UnitySetGraphicsDevice deviceType : " << deviceType << " eventType : " << eventType);
#ifdef MJ_OS_WIN32
		if ((deviceType == 0 || deviceType == 17) && eventType == 0)
		{
			GLenum eRet = glewInit();
			MOJING_TRACE(g_APIlogger, "glewInit finished. ret = " << eRet);
		}
#endif
        
	}
#endif
    
#ifdef MJ_OS_IOS
     void UnityRenderEvent(int eventID)
     {
         OnRenderEvent(eventID);
     }

    extern int g_GamePad_AxisMode ;
    UNITY_EXPORT void Unity_SetGamePadAxisMode(int mode)
    {
        MOJING_FUNC_TRACE(g_APIlogger);
        g_GamePad_AxisMode = mode;
    }
#endif
    
     UnityRenderingEvent UNITY_EXPORT Unity_GetRenderEventFunc()
    {
        return OnRenderEvent;
    }

#ifdef MJ_OS_ANDROID
	 void Unity_DD_SetEnableTracker(bool bEnable)
	 {
		 MojingSDK_DD_SetEnableTracker(bEnable);
	 }

	 // 获取DD陀螺仪状态
	 bool Unity_DD_GetEnableTracker()
	 {
		return MojingSDK_DD_GetEnableTracker();
	 }

	 // 当DD陀螺仪关闭时，存放真实的陀螺仪数据
	 bool Unity_DD_GetLastHeadView(float* pfViewMatrix)
	 {
		 return MojingSDK_DD_GetLastHeadView(pfViewMatrix);
	 }
#endif
}	// extern "C"

	// --------------------------------------------------------------------------
	// GetRenderEventFunc, an example function we export which is used to get a rendering event callback function.
