#ifdef MJ_OS_ANDROID
#include <jni.h>
#endif
#include <unistd.h>						// usleep, etc

#include "Unity/IUnityGraphics.h"

#ifdef __OBJC__
#include <TargetConditionals.h>
#include "../../Render/Metal/MojingRenderMetal.h"
#include "MetalForwardDecls.h" // UnityRenderTexture
/* Unity 5.2, 5.3 Metal Interface */
#include "Unity/UnityInterface.h" // we use 1.UnityGetMetalDevice, 2.UnityRenderBufferMTLTexture

/* Unity 5.4 Metal Interface */
#include "Unity/IUnityGraphicsMetal.h"

#include "MojingMetalView.h"
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
#include "GlStateSave.h"

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
	UnityGfxRenderer currentDeviceType = s_DeviceType;

	switch (eventType)
	{
	case kUnityGfxDeviceEventInitialize:
	{
	   MOJING_TRACE(g_APIlogger, "OnGraphicsDeviceEvent(Initialize).");
	   s_DeviceType = s_Graphics->GetRenderer();
	   MOJING_TRACE(g_APIlogger, "s_DeviceType = " << s_DeviceType);
	   currentDeviceType = s_DeviceType;
	   break;
	}

	case kUnityGfxDeviceEventShutdown:
	{
		MOJING_TRACE(g_APIlogger, "OnGraphicsDeviceEvent(Shutdown).");
		s_DeviceType = kUnityGfxRendererNull;
		break;
	}

	case kUnityGfxDeviceEventBeforeReset:
	{
		MOJING_TRACE(g_APIlogger, "OnGraphicsDeviceEvent(BeforeReset).");
		break;
	}

	case kUnityGfxDeviceEventAfterReset:
		MOJING_TRACE(g_APIlogger, "OnGraphicsDeviceEvent(AfterReset).");
		break;
	};

	switch (currentDeviceType)
	{
	case kUnityGfxRendererOpenGLES20:
		MOJING_TRACE(g_APIlogger, "OpenGLes 2.0.");
		break;
	case kUnityGfxRendererOpenGLES30:
		MOJING_TRACE(g_APIlogger, "OpenGLes 3.0.");
		break;
	case kUnityGfxRendererOpenGLCore:
		MOJING_TRACE(g_APIlogger, "OpenGL core.");
		break;
	case kUnityGfxRendererOpenGL:
		MOJING_TRACE(g_APIlogger, "OpenGL.");
		//DoEventGraphicsDeviceGLUnified(eventType);
		break;

	case kUnityGfxRendererMetal:
		MOJING_TRACE(g_APIlogger, "Metal.");
		break;
	}
}

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
//extern "C" UNITY_EXPORT void	UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	s_UnityInterfaces = unityInterfaces;
	s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
#if defined(MJ_OS_IOS)
    s_GraphicsMetal = s_UnityInterfaces->Get<IUnityGraphicsMetal>();
    if (s_GraphicsMetal == NULL)
        MOJING_TRACE(g_APIlogger, "use 5.2, 5.3 metal interface!!!");
    else
        MOJING_TRACE(g_APIlogger, "use 5.4 metal interface!!!");
#endif
	MOJING_TRACE(g_APIlogger, "s_UnityInterfaces = " << unityInterfaces << ", s_Graphics = " << s_Graphics);

	s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

	// Run OnGraphicsDeviceEvent(initialize) manually on plugin load
	OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
//extern "C" UNITY_EXPORT void UnityPluginUnload()
{
	s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
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

#ifndef MJ_OS_MAC
	//------------------------------
	// 31...16   15    14...8 7      6...0  
	// [payload][data][pos]   [exec][eventid]
	//------------------------------
	const UInt32 IS_DATA_FLAG = 0x00008000;
	const UInt32 DATA_POS_MASK = 0x00004F00;
	const UInt32 EXEC_FLAG = 0x00000080;
	const int DATA_POS_SHIFT = 8;
	const UInt32 EVENT_TYPE_MASK = 0x0000004F;
	const UInt32 PAYLOAD_MASK = 0xFFFF0000;

	static bool EventContainsData(const int eventID)
	{
		return (((UInt32)eventID & IS_DATA_FLAG) != 0);
	}

	static void DecodeDataEvent(const int eventData, int & outEventId, int & outPos, int & outData)
	{
		UInt32 pos = ((UInt32)eventData & DATA_POS_MASK) >> DATA_POS_SHIFT;
		UInt32 eventId = (UInt32)eventData & EVENT_TYPE_MASK;
		UInt32 payload = ((UInt32)eventData & PAYLOAD_MASK) >> 16;

		outEventId = eventId;
		outPos = pos;
		outData = payload;
	}
#endif

#define MAX_EVENTS 32       // plugin data channel relies on this value
	int g_eventData[MAX_EVENTS * 2];  // allow 2 "2-bytes" data events per event type

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
		return MojingSDK_GetScreenSize(pSize);
	}

	UNITY_EXPORT bool Unity_StartTracker(int nSampleFrequence)
	{
		return MojingSDK_StartTracker(nSampleFrequence);
	}

	UNITY_EXPORT bool Unity_StartTrackerCalibration()
	{
		return (MojingSDK_StartTrackerCalibration() == 0);
	}

	UNITY_EXPORT float Unity_IsTrackerCalibrated()
	{
		return MojingSDK_IsTrackerCalibrated();
	}


#ifndef MJ_OS_IOS
	UNITY_EXPORT void Unity_getLastHeadView(float* pfViewMatrix)
	{
		// 注意：在启用ATW的时候，这个数据需要和帧陀螺仪对应使用

            MojingSDK_getLastHeadView(pfViewMatrix);

	}
#endif
    
#ifndef MJ_OS_WIN32
	UNITY_EXPORT bool Unity_ktxLoadTextureN(const char* const filename, unsigned int* pTexture, unsigned int* pTarget, int* pHeight, int* pWidth, int* pDepth, bool* pIsMipmapped, unsigned int* pGlerror, int* ktxError)
	{
		return MojingSDK_ktxLoadTextureN(filename, pTexture, pTarget, pHeight, pWidth, pDepth, pIsMipmapped, pGlerror, ktxError);
	}
#endif

	UNITY_EXPORT void Unity_getLastHeadQuarternion(float &w, float &x, float &y, float &z)
	{
		Quatf Ret;
		MojingSDK_getLastHeadQuarternion(&Ret.w, &Ret.x, &Ret.y, &Ret.z);
		w = Ret.w;
		x = Ret.x;
		y = Ret.y;
		z = Ret.z;
	}

	UNITY_EXPORT void Unity_StopTracker(void)
	{
		MojingSDK_StopTracker();
	}

	UNITY_EXPORT bool Unity_OnSurfaceChanged(int newWidth , int newHeight)
	{
		return MojingSDK_OnSurfaceChanged(newWidth, newHeight);
	}

	struct __tagUnityOverlay
	{
		__tagUnityOverlay(){ m_bChanged = false; m_iTextureID = 0; m_fLeft = m_fTop = m_fHeight = m_fWidth = m_fDistanceInMetre = 0.0f; };
		bool m_bChanged;
		int  m_iTextureID;
		
		float m_fLeft;
		float m_fTop;
		float m_fWidth;
		float m_fHeight;
		float m_fDistanceInMetre;
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
        MTLTextureRef m_mtlTexture;
#endif
		bool operator ==(const __tagUnityOverlay & Other){
			return m_bChanged == Other.m_bChanged &&
				m_iTextureID == Other.m_iTextureID &&
				m_fLeft == Other.m_fLeft;
		}
	}g_LeftOverlay, g_RightOverlay;
	struct __tagCameraProjection
	{
		float m_fDistanceInMetre;
		float m_fProjectionNearInMetre;
	}g_CameraProjection;

	// 设定标准Overlay，这种Overlay允许用户指定位置
	UNITY_EXPORT void Unity_SetOverlay(int iLeftOverlayTextureID, int iRightOverlayTextureID, float fLeft, float fTop, float fWidth, float fHeight)
	{
		g_LeftOverlay.m_iTextureID = iLeftOverlayTextureID;
		g_RightOverlay.m_iTextureID = iRightOverlayTextureID;

		g_RightOverlay.m_fLeft = g_LeftOverlay.m_fLeft = fLeft;
		g_RightOverlay.m_fTop = g_LeftOverlay.m_fTop = fTop;
		g_RightOverlay.m_fWidth = g_LeftOverlay.m_fWidth = fWidth;
		g_RightOverlay.m_fHeight = g_LeftOverlay.m_fHeight = fHeight;
		g_RightOverlay.m_fDistanceInMetre = g_LeftOverlay.m_fDistanceInMetre = 0;// 默认焦点位置就在屏幕上
		g_RightOverlay.m_bChanged = g_LeftOverlay.m_bChanged = true;
	}
        
	UNITY_EXPORT void Unity_SetOverlay3D_V2(int iEyeType, int iTextureID, float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre)
	{
		if (iEyeType & TEXTURE_LEFT_EYE)
		{
			bool bChanged = (g_LeftOverlay.m_iTextureID != iTextureID) ||
				(g_LeftOverlay.m_fLeft != fLeft) ||
				(g_LeftOverlay.m_fTop != fTop) ||
				(g_LeftOverlay.m_fHeight != fHeight) ||
				(g_LeftOverlay.m_fWidth != fWidth) ||
				(g_LeftOverlay.m_fDistanceInMetre != fDistanceInMetre);
			if (bChanged)
			{
				g_LeftOverlay.m_iTextureID = iTextureID;
				g_LeftOverlay.m_fLeft = fLeft;
				g_LeftOverlay.m_fTop = fTop;
				g_LeftOverlay.m_fWidth = fWidth;
				g_LeftOverlay.m_fHeight = fHeight;
				g_LeftOverlay.m_fDistanceInMetre = fDistanceInMetre;
				g_LeftOverlay.m_bChanged = true;
			}
		}

		if (iEyeType & TEXTURE_RIGHT_EYE)
		{
			bool bChanged = (g_RightOverlay.m_iTextureID != iTextureID) ||
				(g_RightOverlay.m_fLeft != fLeft) ||
				(g_RightOverlay.m_fTop != fTop) ||
				(g_RightOverlay.m_fHeight != fHeight) ||
				(g_RightOverlay.m_fWidth != fWidth) ||
				(g_RightOverlay.m_fDistanceInMetre != fDistanceInMetre);
			if (bChanged)
			{
				g_RightOverlay.m_iTextureID = iTextureID;
				g_RightOverlay.m_fLeft = fLeft;
				g_RightOverlay.m_fTop = fTop;
				g_RightOverlay.m_fWidth = fWidth;
				g_RightOverlay.m_fHeight = fHeight;
				g_RightOverlay.m_fDistanceInMetre = fDistanceInMetre;
				g_RightOverlay.m_bChanged = true;
			}
		}

		return;
	}

    // 设定标准Overlay，这种Overlay允许用户指定位置
	UNITY_EXPORT void Unity_SetOverlay3D(int iEyeType , int iTextureID, float fWidth, float fHeight, float fDistanceInMetre)
	{
		if (iEyeType & TEXTURE_LEFT_EYE)
		{
			bool bChanged =  (g_LeftOverlay.m_iTextureID != iTextureID) ||
										(g_LeftOverlay.m_fWidth != fWidth) ||
										(g_LeftOverlay.m_fHeight != fHeight) ||
										(g_LeftOverlay.m_fDistanceInMetre != fDistanceInMetre);
			if (bChanged)
			{
				
				g_LeftOverlay.m_iTextureID = iTextureID;
				g_LeftOverlay.m_fLeft = 0.5 - fWidth/2;
				g_LeftOverlay.m_fTop = 0.5 - fHeight /2;
				g_LeftOverlay.m_fWidth = fWidth;
				g_LeftOverlay.m_fHeight = fHeight;
				g_LeftOverlay.m_fDistanceInMetre = fDistanceInMetre;
				g_LeftOverlay.m_bChanged = true;
			}
		}

		if (iEyeType & TEXTURE_RIGHT_EYE)
		{
			bool bChanged = (g_RightOverlay.m_iTextureID != iTextureID) ||
				(g_RightOverlay.m_fWidth != fWidth) ||
				(g_RightOverlay.m_fHeight != fHeight) ||
				(g_RightOverlay.m_fDistanceInMetre != fDistanceInMetre);
			if (bChanged)
			{
				g_RightOverlay.m_iTextureID = iTextureID;
				g_RightOverlay.m_fLeft = 0.5 - fWidth / 2;
				g_RightOverlay.m_fTop = 0.5 - fHeight / 2;
				g_RightOverlay.m_fWidth = fWidth;
				g_RightOverlay.m_fHeight = fHeight;
				g_RightOverlay.m_fDistanceInMetre = fDistanceInMetre;
				g_RightOverlay.m_bChanged = true;
			}
		}

		return;
	}
    
    int g_iNextModelFrameIndex = 0;
	int g_iFrameOutput = 0;
	int g_iLeftTextureID = 0;
	int g_iRightTextureID = 0;
/*
 Metal
 */
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
    static MTLTextureRef g_mtlLeftEyeTexture = nullptr;
    static MTLTextureRef g_mtlRightEyeTexture = nullptr;

    static MojingMetalView* g_MojingMetalView = nullptr;
    static MTLCommandQueueRef g_mtlCommandQueue = nullptr;
    
    bool MetalLayerCreated = false;
    bool g_UseMetal = false;
    
    UNITY_EXPORT void Unity_CreateMetalLayer()
    {
        if (!MetalLayerCreated)
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                UIWindow* mainWindow = UnityGetMainWindow();
                if (g_MojingMetalView == nullptr)
                {
                    g_MojingMetalView = [[MojingMetalView alloc]initWithFrame: CGRectMake(0, 0, [[UIScreen mainScreen]bounds].size.width, [[UIScreen mainScreen]bounds].size.height)];
                }
                
                [g_MojingMetalView setBackgroundColor:[UIColor blackColor]];
                [mainWindow addSubview:g_MojingMetalView];
                [mainWindow bringSubviewToFront:g_MojingMetalView];
                MetalLayerCreated = true;
                g_UseMetal = true;
            });
        }
    }
    
    UNITY_EXPORT void Unity_DestroyMetalLayer()
    {
        if (g_UseMetal && (MetalLayerCreated == true) )
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                UIWindow* mainWindow = UnityGetMainWindow();
                [mainWindow bringSubviewToFront:mainWindow.rootViewController.view];
                //[g_MojingMetalView removeFromSuperview];
                MetalLayerCreated = false;
                g_UseMetal = false;
            });
        }
    }
    
    UNITY_EXPORT bool Unity_EnterMojingWorld_Metal(const char * szGlassesName,
                                              bool bEnableMultiThread,
                                              bool bEnableTimeWarp,
                                              MTLDeviceRef device,
                                              MTLCommandQueueRef cmdQueue)
    {
        ENTER_MINIDUMP_FUNCTION;
        MOJING_FUNC_TRACE(g_APIlogger);
        MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
        if (!pStatus->IsMojingSDKEnbaled())
        {
            MOJING_ERROR(g_APIlogger, "EnterMojingWorld without Init SDK!");
            return false;
        }
        
        
        if (MojingSDK_ChangeMojingWorld(szGlassesName))
        {
            if (pStatus->GetEngineStatus() != ENGINE_UNREAL)
            {
                Unity_DestroyMetalLayer();
                MojingRenderMetal::CreateCurrentRenderMetal(false, false, device, cmdQueue);
                
            }
        }
        return true;
    }
    
    UNITY_EXPORT void Unity_SetTextureID_Metal(void* leftEye, void* rightEye)
    {
        Unity_CreateMetalLayer();
        
        if (s_GraphicsMetal == NULL)
        {
            g_mtlLeftEyeTexture = UnityRenderBufferMTLTexture((UnityRenderBuffer)leftEye);
            g_mtlRightEyeTexture = UnityRenderBufferMTLTexture((UnityRenderBuffer)rightEye);
        }
        else
        {
            g_mtlLeftEyeTexture = s_GraphicsMetal->TextureFromRenderBuffer((UnityRenderBuffer)leftEye);
            g_mtlRightEyeTexture = s_GraphicsMetal->TextureFromRenderBuffer((UnityRenderBuffer)rightEye);
        }
        
    }
    
    UNITY_EXPORT void Unity_SetOverlay_Metal(void* iLeftOverlayTextureID, void* iRightOverlayTextureID, float fLeft, float fTop, float fWidth, float fHeight)
    {
        if (s_GraphicsMetal == NULL)
        {
            g_LeftOverlay.m_mtlTexture = UnityRenderBufferMTLTexture((UnityRenderBuffer)iLeftOverlayTextureID);
            g_RightOverlay.m_mtlTexture = UnityRenderBufferMTLTexture((UnityRenderBuffer)iRightOverlayTextureID);
        }
        else
        {
            g_LeftOverlay.m_mtlTexture = s_GraphicsMetal->TextureFromRenderBuffer((UnityRenderBuffer)iLeftOverlayTextureID);
            g_RightOverlay.m_mtlTexture = s_GraphicsMetal->TextureFromRenderBuffer((UnityRenderBuffer)iRightOverlayTextureID);
        }
        
        
        g_RightOverlay.m_fLeft = g_LeftOverlay.m_fLeft = fLeft;
        g_RightOverlay.m_fTop = g_LeftOverlay.m_fTop = fTop;
        g_RightOverlay.m_fWidth = g_LeftOverlay.m_fWidth = fWidth;
        g_RightOverlay.m_fHeight = g_LeftOverlay.m_fHeight = fHeight;
        g_RightOverlay.m_fDistanceInMetre = g_LeftOverlay.m_fDistanceInMetre = 0;// 默认焦点位置就在屏幕上
        g_RightOverlay.m_bChanged = g_LeftOverlay.m_bChanged = true;
    }
    
    /*
     called from C# every frame
     */
    UNITY_EXPORT void Unity_SetOverlay3D_Metal(int iEyeType , void* iTextureID, float fWidth, float fHeight, float fDistanceInMetre)
    {
        MTLTextureRef mtlTexture;
        if (s_GraphicsMetal == NULL)
        {
            mtlTexture = UnityRenderBufferMTLTexture((UnityRenderBuffer)iTextureID);
        }
        else
        {
            mtlTexture = s_GraphicsMetal->TextureFromRenderBuffer((UnityRenderBuffer)iTextureID);
        }
        
        
        if (iEyeType & TEXTURE_LEFT_EYE)
        {
            g_LeftOverlay.m_bChanged =  (g_LeftOverlay.m_mtlTexture != mtlTexture) ||
            (g_LeftOverlay.m_fWidth != fWidth) ||
            (g_LeftOverlay.m_fHeight != fHeight) ||
            (g_LeftOverlay.m_fDistanceInMetre != fDistanceInMetre);
            if (g_LeftOverlay.m_bChanged)
            {
                g_LeftOverlay.m_fLeft = 0.5 - fWidth / 2;
                g_LeftOverlay.m_fTop = 0.5 - fHeight / 2;
                g_LeftOverlay.m_fWidth = fWidth;
                g_LeftOverlay.m_fHeight = fHeight;
                g_LeftOverlay.m_fDistanceInMetre = fDistanceInMetre;
                g_LeftOverlay.m_mtlTexture = mtlTexture;
            }
        }
        
        if (iEyeType & TEXTURE_RIGHT_EYE)
        {
            g_RightOverlay.m_bChanged = (g_RightOverlay.m_mtlTexture != mtlTexture) ||
            (g_RightOverlay.m_fWidth != fWidth) ||
            (g_RightOverlay.m_fHeight != fHeight) ||
            (g_RightOverlay.m_fDistanceInMetre != fDistanceInMetre);
            if (g_RightOverlay.m_bChanged)
            {
                g_RightOverlay.m_fLeft = 0.5 - fWidth / 2;
                g_RightOverlay.m_fTop = 0.5 - fHeight / 2;
                g_RightOverlay.m_fWidth = fWidth;
                g_RightOverlay.m_fHeight = fHeight;
                g_RightOverlay.m_fDistanceInMetre = fDistanceInMetre;
                g_RightOverlay.m_mtlTexture = mtlTexture;
            }
        }
        
        return;
    }
    
    UNITY_EXPORT void Unity_SetOverlay3D_V2_Metal(int iEyeType , void* iTextureID, float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre)
    {
        //MOJING_TRACE(g_APIlogger, "Unity_SetOverlay3D_V2_Metal : Left = " << fLeft << " , Top = " << fTop);
        MTLTextureRef mtlTexture;
        if (s_GraphicsMetal == NULL)
        {
            mtlTexture = UnityRenderBufferMTLTexture((UnityRenderBuffer)iTextureID);
        }
        else
        {
            mtlTexture = s_GraphicsMetal->TextureFromRenderBuffer((UnityRenderBuffer)iTextureID);
        }
        
        
        if (iEyeType & TEXTURE_LEFT_EYE)
        {
            g_LeftOverlay.m_bChanged =  (g_LeftOverlay.m_mtlTexture != mtlTexture) ||
            (g_LeftOverlay.m_fLeft != fLeft) ||
            (g_LeftOverlay.m_fTop != fTop) ||
            (g_LeftOverlay.m_fWidth != fWidth) ||
            (g_LeftOverlay.m_fHeight != fHeight) ||
            (g_LeftOverlay.m_fDistanceInMetre != fDistanceInMetre);
            if (g_LeftOverlay.m_bChanged)
            {
                g_LeftOverlay.m_fLeft = fLeft;
                g_LeftOverlay.m_fTop = fTop;
                g_LeftOverlay.m_fWidth = fWidth;
                g_LeftOverlay.m_fHeight = fHeight;
                g_LeftOverlay.m_fDistanceInMetre = fDistanceInMetre;
                g_LeftOverlay.m_mtlTexture = mtlTexture;
            }
            //MOJING_TRACE(g_APIlogger, "Unity_SetOverlay3D_V2_Metal : iEyeType = " << iEyeType << " , m_bChanged = " << g_LeftOverlay.m_bChanged );
        }
        
        if (iEyeType & TEXTURE_RIGHT_EYE)
        {
            g_RightOverlay.m_bChanged = (g_RightOverlay.m_mtlTexture != mtlTexture) ||
            (g_RightOverlay.m_fLeft != fLeft) ||
            (g_RightOverlay.m_fTop != fTop) ||
            (g_RightOverlay.m_fWidth != fWidth) ||
            (g_RightOverlay.m_fHeight != fHeight) ||
            (g_RightOverlay.m_fDistanceInMetre != fDistanceInMetre);
            if (g_RightOverlay.m_bChanged)
            {
                g_RightOverlay.m_fLeft = fLeft;
                g_RightOverlay.m_fTop = fTop;
                g_RightOverlay.m_fWidth = fWidth;
                g_RightOverlay.m_fHeight = fHeight;
                g_RightOverlay.m_fDistanceInMetre = fDistanceInMetre;
                g_RightOverlay.m_mtlTexture = mtlTexture;
            }
            //MOJING_TRACE(g_APIlogger, "Unity_SetOverlay3D_V2_Metal : iEyeType = " << iEyeType << " , m_bChanged = " << g_RightOverlay.m_bChanged );
        }
        
        return;
    }
    
    bool Unity_DrawDistortion_Metal(
                                          MTLTextureRef eyeTextureLeft,
                                          MTLTextureRef eyeTextureRight,
                                          MTLTextureRef overlayTextureLeft,
                                          MTLTextureRef overlayTextureRight
                                    )
    {
        MojingRenderMetal* metalRender = MojingRenderMetal::GetInstance();
        return metalRender->WarpToScreen(g_MojingMetalView.currentDrawable,
                                  eyeTextureLeft,
                                  eyeTextureRight,
                                  overlayTextureLeft,
                                  overlayTextureRight);
    }
#endif
    
	float g_ATW_FrameMatrix[16];
	UNITY_EXPORT void Unity_SetTextureID(int iLeftTextureID, int iRightTextureID)
	{
		g_iLeftTextureID = iLeftTextureID;
		g_iRightTextureID = iRightTextureID;
#ifdef _DEBUG
		MOJING_TRACE(g_APIlogger, "SetTextureID : Left = " << iLeftTextureID << " , Right = " << iRightTextureID);
#endif
		return;
	}
    
	UNITY_EXPORT void Unity_ATW_SetTextureID(int iLeftTextureID, int iRightTextureID, float *pFrameMatrix)
	{
		Unity_SetTextureID(iLeftTextureID, iRightTextureID);
		memcpy(g_ATW_FrameMatrix , pFrameMatrix , sizeof(float) * 16);
		
#ifdef _DEBUG
		static char szATW_FrameMatrix[512];
		*szATW_FrameMatrix = 0;
		for (int i = 0; i < 16; i++)
			sprintf(szATW_FrameMatrix + strlen(szATW_FrameMatrix), "%g ", pFrameMatrix[i]);

		MOJING_TRACE(g_APIlogger, "ATW SetTextureID : Frame = " << g_iFrameOutput << " , Index = " << g_iNextModelFrameIndex << " , Left = " << iLeftTextureID << " , Right = " << iRightTextureID << " , ATW_FrameMatrix = " << szATW_FrameMatrix);
#endif
		g_iFrameOutput++;
		if (g_iFrameOutput > 2)
			g_iNextModelFrameIndex = -1;
		else
			g_iNextModelFrameIndex++;
	}

	UNITY_EXPORT int Unity_ATW_GetModelFrameIndex()
	{
		while (g_iNextModelFrameIndex < 0)
			usleep(300);// 约1/3毫秒
#ifdef _DEBUG
		MOJING_TRACE(g_APIlogger, "Unity_ATW_GetModelFrameIndex = " << g_iNextModelFrameIndex );
#endif
		return g_iNextModelFrameIndex;
	}

	UNITY_EXPORT void Unity_ResetSensorOrientation()
	{
		MojingSDK_ResetSensorOrientation();
	}

	UNITY_EXPORT void Unity_ResetSensorOrientation2()
	{
		MojingSDK_ResetSensorOrientation2();
	}

	UNITY_EXPORT void Unity_ResetTracker(void)
	{
		MojingSDK_ResetTracker();
	}

	UNITY_EXPORT int Unity_GetTextureSize(void)
	{
		return MojingSDK_GetTextureSize();
	}

	char g_sGlassesName[128] = "Mojing3";
	bool g_MultithreadingType = false;
	bool  g_TimeWarpType = false;
	bool  g_TimeWarpTypeChanged = false;
	UNITY_EXPORT bool Unity_SetMojingWorld(const char * szGlassesName, bool bMultithreading, bool bTimeWarp, bool bUseMetal)
	{
		size_t len = strlen(szGlassesName);
		if (len >= sizeof(g_sGlassesName))
		{
			return false;
		}

		strcpy(g_sGlassesName, szGlassesName);
		g_TimeWarpType = bTimeWarp;
		g_TimeWarpTypeChanged = false;
		g_MultithreadingType = bMultithreading;
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
        g_UseMetal = bUseMetal;
#endif
        
		if (!g_UnityVersion.m_bEnableATW)
		{// 不支持ATW
			g_MultithreadingType = false;
		}
		return true;
	}

	UNITY_EXPORT void Unity_SetEnableTimeWarp(bool bEnable)
	{
		g_TimeWarpType = bEnable;
		g_TimeWarpTypeChanged = true;
	}

	UNITY_EXPORT bool Unity_IsInMojingWorld(const char * szGlassesName)
	{
#ifdef _DEBUG
		MOJING_FUNC_TRACE(g_APIlogger);
#endif // _DEBUG

		const char* pGlassesNow = MojingSDK_GetGlasses();
#ifdef _DEBUG
		MOJING_TRACE(g_APIlogger, "Wait for " << szGlassesName << ", now in " << pGlassesNow);
#endif
		return (strcmp(szGlassesName, pGlassesNow) == 0);
	}

	UNITY_EXPORT bool Unity_EnterMojingWorld(const char * szGlassesName)
	{
		size_t len = strlen(szGlassesName);
		if (len >= sizeof(g_sGlassesName))
		{
			return false;
		}

		strcpy(g_sGlassesName, szGlassesName);
		return true;
		//return MojingSDK_EnterMojingWorld(szGlassesName);
	}
    
	UNITY_EXPORT bool Unity_ChangeMojingWorld(const char * szGlassesName)
	{
		size_t len = strlen(szGlassesName);
		if (len >= sizeof(g_sGlassesName))
		{
			return false;
		}

		strcpy(g_sGlassesName, szGlassesName);
        
		return true;
		//return MojingSDK_ChangeMojingWorld(szGlassesName);
	}

	UNITY_EXPORT bool Unity_IsGlassesNeedDistortion(void)
	{
		MOJING_FUNC_TRACE(g_APIlogger);
		return MojingSDK_IsGlassesNeedDistortion();
	}

	UNITY_EXPORT bool Unity_IsGlassesNeedDistortionByName(const char * szGlassesName)
	{
		MOJING_FUNC_TRACE(g_APIlogger);
		return MojingSDK_IsGlassesNeedDistortionByName(szGlassesName);
	}

	struct CenterLineParam
	{
		CenterLineParam() :
		m_iWidth(4),
		m_iColR(255),
		m_iColG(255),
		m_iColB(255),
		m_iAlpha(255)
		{};
		int m_iWidth;
		int m_iColR;
		int m_iColG;
		int m_iColB;
		int m_iAlpha;
	}g_CenterLineParam;
	UNITY_EXPORT void Unity_SetCenterLine(int iWidth, int colR, int colG, int colB, int colA)
	{
		// 		MOJING_FUNC_TRACE(g_APIlogger);
		// 		MOJING_TRACE(g_APIlogger, "MojingSDK_SetCenterLine(" << iWidth << ", " << colR << ", " << colG << ", " << colB << ", " << colA);
		//		MojingSDK_SetCenterLine(iWidth, colR, colG, colB, colA);
		g_CenterLineParam.m_iWidth = iWidth;
		g_CenterLineParam.m_iColR = colR;
		g_CenterLineParam.m_iColG = colG;
		g_CenterLineParam.m_iColB = colB;
		g_CenterLineParam.m_iAlpha = colA;
	}

    /*
     未从 C# 直接调用，而是在 plugin event 中被调用
     */
	UNITY_EXPORT bool Unity_LeaveMojingWorld()
	{
		MOJING_FUNC_TRACE(g_APIlogger);
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
        Unity_DestroyMetalLayer();
        // need to reset overlay matrix when re-EnterMojingWorld
        g_LeftOverlay.m_bChanged = true;
        g_RightOverlay.m_bChanged = true;

        g_LeftOverlay.m_mtlTexture = nullptr;
        g_RightOverlay.m_mtlTexture = nullptr;
        g_LeftOverlay.m_fWidth = 0;
        g_LeftOverlay.m_fHeight = 0;
        g_RightOverlay.m_fWidth = 0;
        g_RightOverlay.m_fHeight = 0;
#endif
        
        g_iNextModelFrameIndex = 0;
        g_iFrameOutput = 0;
        g_iLeftTextureID = 0;
        g_iRightTextureID = 0;
        return MojingSDK_LeaveMojingWorld();
	}

	UNITY_EXPORT void Unity_FreeString(char * pReleaseString)
	{
		//		MOJING_FUNC_TRACE(g_APIlogger);
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
		//MOJING_FUNC_TRACE(g_APIlogger);
		return Unity_AllocString(MojingSDK_GetManufacturerList(strLanguageCodeByISO639));
	}

	UNITY_EXPORT const char* Unity_GetProductList(const char* strManufacturerKey, const char* strLanguageCodeByISO639)
	{
		//MOJING_FUNC_TRACE(g_APIlogger);
		return Unity_AllocString(MojingSDK_GetProductList(strManufacturerKey, strLanguageCodeByISO639));
	}

	UNITY_EXPORT const char* Unity_GetGlassList(const char* strProductKey, const char* strLanguageCodeByISO639)
	{
		//MOJING_FUNC_TRACE(g_APIlogger);
		return Unity_AllocString(MojingSDK_GetGlassList(strProductKey, strLanguageCodeByISO639));
	}

	UNITY_EXPORT const char* Unity_GetGlassInfo(const char* strGlassKey, const char* strLanguageCodeByISO639)
	{
		//MOJING_FUNC_TRACE(g_APIlogger);
		return Unity_AllocString(MojingSDK_GetGlassInfo(strGlassKey, strLanguageCodeByISO639));
	}

	UNITY_EXPORT const char* Unity_GenerationGlassKey(const char* strProductQRCode, const char* strGlassQRCode)
	{
		//MOJING_FUNC_TRACE(g_APIlogger);
		return Unity_AllocString(MojingSDK_GenerationGlassKey(strProductQRCode, strGlassQRCode));
	}

	UNITY_EXPORT const char* Unity_GetSDKVersion()
	{
		//MOJING_FUNC_TRACE(g_APIlogger);
		return Unity_AllocString(MojingSDK_GetSDKVersion());
	}


	UNITY_EXPORT const char* Unity_GetUserSettings()
	{
		return Unity_AllocString(MojingSDK_GetUserSettings());
	}

	UNITY_EXPORT bool Unity_SetUserSettings(const char * sUserSettings)
	{
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
        
        //		MOJING_FUNC_TRACE(g_APIlogger);
        
#ifndef MJ_OS_IOS
        // Unknown graphics device type? Do nothing.
        // MOJING_TRACE(g_APIlogger, "s_DeviceType = " << s_DeviceType);
        if (s_DeviceType == kUnityGfxRendererNull)
        {
            s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
            s_DeviceType = s_Graphics->GetRenderer();
            //			MOJING_TRACE(g_APIlogger, "Try get Device Type again: " << s_DeviceType);
            //	return;
        }
#endif
        // MOJING_TRACE(g_APIlogger, "UnityRenderEvent with " << eventID);
        Manager* pManager = Manager::GetMojingManager();
        if (pManager)
        {
            switch (eventID)
            {
                case DistortFrame:
                {
#ifdef _DEBUG
                    MOJING_TRACE(g_APIlogger, "DistortFrame");
#endif
                    
#ifndef MJ_OS_ANDROID
                    
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
                    if (g_UseMetal)
                    {
                        
                        Unity_DrawDistortion_Metal(g_mtlLeftEyeTexture,
                                                   g_mtlRightEyeTexture,
                                                   g_LeftOverlay.m_mtlTexture,
                                                   g_RightOverlay.m_mtlTexture
                                                   );
                        
                        
                    }
                    else
#endif
                    {
                        MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
                        if (pRender)
                            pRender->WarpToScreen();
                    }
                    
#else // Andorid
                    GLStateSave SaveGLState;
                    
                    // WORKAROUND: On Mali with static-batching enabled, Unity leaves
                    // ibo mapped entire frame. When we inject our vignette and timewarp
                    // rendering with the ibo mapped, rendering corruption will occur.
                    // Explicitly unbind here.
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                    
                    // Discard the depth buffer, so the tiler won't need to write it back out to memory
                    GL_InvalidateFramebuffer(INV_FBO, false, true);
                    // glFlush();
                    
                    MojingRenderBase *pRender = MojingRenderBase::GetCurrentRender();
                    if (pRender)
                    {
                        if (g_UnityVersion.m_bATW_ON)
                        {
                            g_iNextModelFrameIndex = pRender->UnityATW_WarpToScreen();
                        }
                        else
                        {
                            pRender->WarpToScreen();// 提交绘制完的场景
                        }
                    }
#endif
                }
                    break;
                    
                case EnterMojingWorld:
                    g_UnityVersion.m_bATW_ON = g_UnityVersion.m_bEnableATW && g_MultithreadingType;
                    MOJING_TRACE(g_APIlogger, "EnterMojingWorld: " << g_sGlassesName << " , Unity Version = " <<
                                 g_UnityVersion.m_MajorVersion << "." << g_UnityVersion.m_MinorVersion <<
                                 " , Enable ATW = " << ((g_UnityVersion.m_bEnableATW ) ? "True" : "False") <<
                                 " , Enable TW = " << ((g_TimeWarpType ) ? "True" : "False") <<
                                 " , Using ATW = " << (g_MultithreadingType && g_TimeWarpType ? "True" : "False"));
                    g_iNextModelFrameIndex = 0;
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
                    if (g_UseMetal)
                    {
                        if (g_mtlCommandQueue == nullptr)
                        {
                            if (s_GraphicsMetal == NULL) // unity < 5.4
                            {
                                g_mtlCommandQueue = [UnityGetMetalDevice() newCommandQueue];
                            }
                            else // unity >= 5.4
                            {
                                g_mtlCommandQueue = [s_GraphicsMetal->MetalDevice() newCommandQueue];
                            }
                            
                        }
                        
                        if (s_GraphicsMetal == NULL) // unity < 5.4
                        {
                            Unity_EnterMojingWorld_Metal(g_sGlassesName, g_UnityVersion.m_bATW_ON, g_TimeWarpType, UnityGetMetalDevice(), g_mtlCommandQueue);
                        }
                        else // unity >= 5.4
                        {
                            Unity_EnterMojingWorld_Metal(g_sGlassesName, g_UnityVersion.m_bATW_ON, g_TimeWarpType, s_GraphicsMetal->MetalDevice(), g_mtlCommandQueue);
                        }
                        
                        
                    }
                    else
#endif
                    {
                        MojingSDK_EnterMojingWorld(g_sGlassesName, g_UnityVersion.m_bATW_ON, g_TimeWarpType);// 在启用TW的同时，如果可能的话优先启动ATW
                    }
                    
                    break;
                    
                case ChangeMojingWorld:
                    //MOJING_TRACE(g_APIlogger, "ChangeMojingWorld: " << g_sGlassesName);
                    MojingSDK_ChangeMojingWorld(g_sGlassesName);
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
                    // 某些镜片不带畸变，使用 Unity 渲染；所以去除 Metal layer
                    Unity_DestroyMetalLayer();
#endif
                    break;
                    
                case LeaveMojingWorld:
                    //MOJING_TRACE(g_APIlogger, "LeaveMojingWorld");
//                    MojingSDK_LeaveMojingWorld();
                    Unity_LeaveMojingWorld();
                    break;
                    
                case SetTextureID:
                {
                    MOJING_TRACE(g_APIlogger, "SetTextureID");
                    Manager* pManager = Manager::GetMojingManager();
                    if (pManager)
                    {
						if (g_TimeWarpTypeChanged)
						{
							g_TimeWarpTypeChanged = false;
							MojingSDK_SetEnableTimeWarp(g_TimeWarpType);
						}
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
                        if (g_UseMetal)
                        {
                            MojingRenderMetal *pRender = MojingRenderMetal::GetInstance();
                            if (pRender)
                            {
                                if (g_LeftOverlay.m_bChanged)
                                {
                                    if (g_LeftOverlay.m_fDistanceInMetre > 0)
                                    {
                                        //MojingSDK_SetOverlayPosition3D(TEXTURE_LEFT_EYE, g_LeftOverlay.m_fWidth, g_LeftOverlay.m_fHeight, g_LeftOverlay.m_fDistanceInMetre);
                                        MojingSDK_SetOverlayPosition3D_V2(TEXTURE_LEFT_EYE, g_LeftOverlay.m_fLeft, g_LeftOverlay.m_fTop, g_LeftOverlay.m_fWidth, g_LeftOverlay.m_fHeight, g_LeftOverlay.m_fDistanceInMetre);
                                    }
                                    else
                                    {
                                        MojingSDK_SetOverlayPosition(g_LeftOverlay.m_fLeft, g_LeftOverlay.m_fTop, g_LeftOverlay.m_fWidth, g_LeftOverlay.m_fHeight);
                                    }
                                }
                                if (g_RightOverlay.m_bChanged)
                                {
                                    if (g_RightOverlay.m_fDistanceInMetre > 0)
                                    {
                                        //MojingSDK_SetOverlayPosition3D(TEXTURE_RIGHT_EYE, g_RightOverlay.m_fWidth, g_RightOverlay.m_fHeight, g_RightOverlay.m_fDistanceInMetre);
                                        MojingSDK_SetOverlayPosition3D_V2(TEXTURE_RIGHT_EYE, g_RightOverlay.m_fLeft, g_RightOverlay.m_fTop, g_RightOverlay.m_fWidth, g_RightOverlay.m_fHeight, g_RightOverlay.m_fDistanceInMetre);
                                    }
                                    else
                                    {
                                        MojingSDK_SetOverlayPosition(g_RightOverlay.m_fLeft, g_RightOverlay.m_fTop, g_RightOverlay.m_fWidth, g_RightOverlay.m_fHeight);
                                    }
                                }
                            }// pRender

                        }
                        else // GLES
#endif
                        {
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
                                    if (g_UnityVersion.m_bATW_ON)
                                    {// 同时设置绘制图像使用的陀螺仪数据
                                        pRender->UnityATW_SetEyeTexID(g_iLeftTextureID, g_iRightTextureID, g_ATW_FrameMatrix);
                                    }
                                    else
                                    {
                                        pRender->SetEyeTexID(g_iLeftTextureID, g_iRightTextureID);
                                    }
									pRender->SetOverlayTextureID(g_LeftOverlay.m_iTextureID, g_RightOverlay.m_iTextureID);
									if (g_LeftOverlay.m_bChanged)
									{
										MojingSDK_SetOverlayPosition3D_V2(TEXTURE_LEFT_EYE, g_LeftOverlay.m_fLeft, g_LeftOverlay.m_fTop, g_LeftOverlay.m_fWidth, g_LeftOverlay.m_fHeight, g_LeftOverlay.m_fDistanceInMetre);
										g_LeftOverlay.m_bChanged = false;
									}
									if (g_RightOverlay.m_bChanged)
									{
										MojingSDK_SetOverlayPosition3D_V2(TEXTURE_RIGHT_EYE, g_RightOverlay.m_fLeft, g_RightOverlay.m_fTop, g_RightOverlay.m_fWidth, g_RightOverlay.m_fHeight, g_RightOverlay.m_fDistanceInMetre);
										g_RightOverlay.m_bChanged = false;
									}
                                }
                            }// pRender
                        }
                    }
                    else
                    {
                        MOJING_ERROR(g_APIlogger, "Set texture ID failed.");
                    }
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
                    MojingSDK_SetCenterLine(
                                            g_CenterLineParam.m_iWidth,
                                            g_CenterLineParam.m_iColR,
                                            g_CenterLineParam.m_iColG,
                                            g_CenterLineParam.m_iColB,
                                            g_CenterLineParam.m_iAlpha);
                    break;
                    
                default:
                    break;
            }
        }
        return;
        
#ifndef MJ_OS_MAC
        if (EventContainsData(eventID))
        {
            int outEventId = 0;
            int outPos = 0;
            int outData = 0;
            DecodeDataEvent(eventID, outEventId, outPos, outData);
            
            g_eventData[outEventId * 2 + outPos] = outData;
            //LOG( "UnityRenderEvent %i %i %i", outEventId, outPos, outData );
        }
        
        //	LOG( "UnityRenderEvent %i", eventID );
        
        if (((UInt32)eventID) & EXEC_FLAG)
        {
            int eventType = ((UInt32)eventID) & EVENT_TYPE_MASK;
            switch (eventType)
            {
                case EVENT_INIT_RENDERTHREAD:
                    //		MJ_InitRenderThread();
                    break;
                case EVENT_SHUTDOWN_RENDERTHREAD:
                    //		MJ_ShutdownRenderThread();
                    break;
                case EVENT_PAUSE:
                    //		MJ_Pause();
                    break;
                case EVENT_RESUME:
                    //		MJ_Resume();
                    break;
                case EVENT_LEFTEYE_ENDFRAME:
                {
                    //		const int eventData = up.eventData[eventID * 2 + 0] + up.eventData[eventID * 2 + 1];
                    //		MJ_CameraEndFrame( ovrEye_Left, eventData );
                    break;
                }
                case EVENT_RIGHTEYE_ENDFRAME:
                {
                    //		const int eventData = up.eventData[eventID * 2 + 0] + up.eventData[eventID * 2 + 1];
                    //		MJ_CameraEndFrame( ovrEye_Right, eventData );
                    break;
                }
                case EVENT_TIMEWARP:
                {
                    const int nTextureID = g_eventData[eventType * 2 + 0] + (g_eventData[eventType * 2 + 1] << 16);
                    //	LOG( "MJ_TimeWarpEvent with view index %i", eventData );
                    // UNITY_DrawTexture();
                    //		MJ_TimeWarpEvent( eventData );
                    
                    // Update the movie surface, if active.
                    //up.VideoSurface.Update();
                    break;
                }
                case EVENT_PLATFORMUI_GLOBALMENU:
                    //		MJ_Platform_StartUI( PUI_GLOBAL_MENU );
                    break;
                case EVENT_PLATFORMUI_CONFIRM_QUIT:
                    //		MJ_Platform_StartUI( PUI_CONFIRM_QUIT );
                    break;
                case EVENT_RESET_VRMODEPARMS:
                    //		MJ_VrModeParms_Reset();
                    break;
                case EVENT_PLATFORMUI_TUTORIAL:
                    //		MJ_Platform_StartUI( PUI_GLOBAL_MENU_TUTORIAL );
                    break;
                default:
                    //		LOG( "Invalid Event ID %i", eventID );
                    break;
            }
        }
#endif
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
		sscanf(lpszEngine, "Unity %d.%d", &g_UnityVersion.m_MajorVersion, &g_UnityVersion.m_MinorVersion);
		g_UnityVersion.m_bEnableATW = g_UnityVersion.m_MajorVersion > 5 ;// 默认为6.0+都能支持
		if (!g_UnityVersion.m_bEnableATW && g_UnityVersion.m_MajorVersion == 5)
		{// 5.x的版本要求至少是5.2
			g_UnityVersion.m_bEnableATW = g_UnityVersion.m_MinorVersion >= 2;
		}
		
		return MojingSDK_SetEngineVersion(lpszEngine);
	}
	
	UNITY_EXPORT bool Unity_IsEnableATW()// 是否允许启用ATW
	{// g_UnityVersion.m_MajorVersion > 1 防止误报处理
#ifdef MJ_OS_ANDROID
		return true;
		// return (g_UnityVersion.m_MajorVersion > 1 )&& g_UnityVersion.m_bEnableATW;
#else
		return false;
#endif
	}
	
	bool Unity_IsInterFBRPlatform()
	{
		return false;// MojingPlatformBase::IsInterFBRPlatform();
	}

	UNITY_EXPORT bool Unity_IsATW_ON()// 是否启用了ATW
	{// g_UnityVersion.m_MajorVersion > 1 防止误报处理
		if (Unity_IsInterFBRPlatform())
			return false;
#ifdef MJ_OS_ANDROID
 		return g_UnityVersion.m_MajorVersion > 1 && g_UnityVersion.m_bATW_ON;
#else
		return false;
#endif
	}	
	
	UNITY_EXPORT bool Unity_IsLowPower()
	{
		return MojingSDK_IsLowPower();
	}

	void Unity_getProjectionMatrix(int eye, bool bVrMode, float fFOV, float fNear, float fFar, float* pfProjectionMatrix, int* pfViewRect)
	{
		MojingSDK_getProjectionMatrix(eye, bVrMode, fFOV, fNear, fFar, pfProjectionMatrix, pfViewRect);
	}

	UNITY_EXPORT void Unity_AppSetContinueInterval(int interval)
	{
		MojingSDK_AppSetContinueInterval(interval);
	}

	UNITY_EXPORT void Unity_AppSetReportInterval(int interval)
	{
		MojingSDK_AppSetReportInterval(interval);
	}

	UNITY_EXPORT void Unity_AppSetReportImmediate(bool bImmediate)
	{
		MojingSDK_AppSetReportImmediate(bImmediate);
	}

	UNITY_EXPORT void Unity_AppPageStart(const char* szPageName)
	{
		MojingSDK_AppPageStart(szPageName);
	}

	UNITY_EXPORT void Unity_AppPageEnd(const char* szPageName)
	{
		MojingSDK_AppPageEnd(szPageName);
	}

	UNITY_EXPORT void Unity_AppSetEvent(const char* szEventName, const char* szEventChannelID, const char* szEventInName, float eInData, const char* szEventOutName, float eOutData)
	{
		MojingSDK_AppSetEvent(szEventName, szEventChannelID, szEventInName, eInData, szEventOutName, eOutData);
	}
    
    UNITY_EXPORT const char* Unity_AppGetUserID()
    {
        return MojingPlatformBase::GetPlatform()->GetUserID();
    }
    
	UNITY_EXPORT const char* Unity_AppGetRunID()
	{
		return MojingPlatformBase::GetPlatform()->GetRunID();
	}

	UNITY_EXPORT int  Unity_GetMojingWorldDistortionMesh(const char * szGlassesName, int iScreenWidth, int iScreenHeight, int iWidthCells, int iHeightCells,
		float *pFOV, float *pGlassesSeparationInMeter,
		float * pVerts, float * pUV, int * pIndices)
	{
		return MojingSDK_GetDistortionMesh(szGlassesName, iScreenWidth, iScreenHeight, iWidthCells, iHeightCells,
			pFOV, pGlassesSeparationInMeter,
			pVerts, pUV, pIndices);

	}

	UNITY_EXPORT void Unity_AppReportLog(int iLogType, const char* szTypeName, const char* szLogContent)
	{
		MojingSDK_ReportLog(iLogType, szTypeName, szLogContent, false);
	}
}	// extern "C"
#ifdef MJ_OS_IOS
extern "C" void UnityRenderEvent(int eventID)
{
    OnRenderEvent(eventID);
}
#endif
	// --------------------------------------------------------------------------
	// GetRenderEventFunc, an example function we export which is used to get a rendering event callback function.
extern "C" UnityRenderingEvent UNITY_EXPORT Unity_GetRenderEventFunc()
	{
#ifndef MJ_OS_IOS
#ifdef _DEBUG
		MOJING_TRACE(g_APIlogger, "Unity_GetRenderEventFunc." << OnRenderEvent);
#endif
		return OnRenderEvent;
#endif
    return NULL;
}
