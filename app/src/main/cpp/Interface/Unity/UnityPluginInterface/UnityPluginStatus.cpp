#include "UnityPluginStatus.h"
#include <string.h>
#include <stdio.h>
#ifdef LOG4CPLUS_IMPORT
#include "../../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../../LogTraker/LogInterface.h"
#endif

#ifdef MJ_OS_IOS
#include "MojingSDKStatus.h"
using namespace Baofeng;
using namespace Baofeng::Mojing;
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

UnityPluginStatus g_UnityPluginStatus;

UnityPluginStatus::UnityPluginStatus()
{
	m_pUnityInterfaces = NULL;
	m_pUnityGraphics = NULL;
	m_DeviceType = kUnityGfxRendererNull;
#ifdef MJ_OS_IOS
	m_pGraphicsMetal = NULL;
#endif
}


UnityPluginStatus::~UnityPluginStatus()
{
}

void  UnityPluginStatus::OnEvent_GraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
	MOJING_TRACE(g_APIlogger, "UnityPluginStatus::OnEvent_GraphicsDeviceEvent : EventType = " << eventType);
}

void UnityPluginStatus::Init(IUnityInterfaces* unityInterfaces/*, IUnityGraphicsDeviceEventCallback callBackFunction*/)
{
    MOJING_FUNC_TRACE(g_APIlogger);
	SetUnityInterfaces(unityInterfaces);
	SetUnityGraphics(m_pUnityInterfaces->Get<IUnityGraphics>());

	// m_pUnityGraphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
	SetDeviceType(m_pUnityGraphics->GetRenderer());
#ifdef MJ_OS_IOS
	SetGraphicsMetal(m_pUnityInterfaces->Get<IUnityGraphicsMetal>());
#endif
	MOJING_TRACE(g_APIlogger, "UnityPluginStatus::Init : Render Type = " << m_DeviceType);
    
#ifdef MJ_OS_IOS
    MOJING_TRACE(g_APIlogger, "UnityPluginStatus::Init : m_pUnityGraphics = " << m_pUnityGraphics );
    MOJING_TRACE(g_APIlogger, "UnityPluginStatus::Init : m_pGraphicsMetal = " << m_pGraphicsMetal );
#endif
}

void UnityPluginStatus::Release()
{
// 	if (m_pUnityGraphics)
// 		m_pUnityGraphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
	m_pUnityInterfaces = NULL;
	m_pUnityGraphics = NULL;
	m_DeviceType = kUnityGfxRendererNull;
#ifdef MJ_OS_IOS
	m_pGraphicsMetal = NULL;
#endif
}
#ifdef MJ_OS_IOS

MTLTextureRef UnityPluginStatus::ConvertTexture_Metal( void * TID , EyeTextureType TextureType /*= TEXTURE_OPEN_GL*/)
{
//    MOJING_FUNC_TRACE(g_APIlogger);
//    MOJING_TRACE(g_APIlogger, "m_pGraphicsMetal = " << m_pGraphicsMetal << " ,  TID ptr = " << TID);
    MTLTextureRef pRet = nullptr;

    if ((long long)TID < 0x100000000 || (long long)TID > 0x200000000)
    {
        String strEngine = MojingSDKStatus::GetSDKStatus()->GetEngine();
        if(strncasecmp(strEngine.ToCStr(), "Unity 5.5", 9) < 0)
        {
            return pRet;
        }
    }

	if (TextureType & TEXTURE_IS_UNITY_METAL_RENDER_TEXTURE)
	{
        // NSLog(@"metal rendertexture: %p, graphicMetal: %p", TID, m_pGraphicsMetal);
		if (m_pGraphicsMetal == NULL)
		{
			//pRet = UnityRenderBufferMTLTexture((UnityRenderBuffer)TID);
		}
		else
		{
            try {
                pRet = m_pGraphicsMetal->TextureFromRenderBuffer((UnityRenderBuffer)TID);
            } catch (std::exception e) {
                // NSLog(@"--Metal-- there's error when converting texture %s", e.what());
            }
		}
	}
	else
	{
        // NSLog(@"metal texture: %p", TID);
		pRet = ((__bridge MTLTextureRef)TID);
	}
    return pRet;
};
#endif

MTLTextureRef UnityPluginStatus::ConvertTexture(long & outTexture,void * TID, EyeTextureType TextureType /*= TEXTURE_OPEN_GL*/)
{
	MTLTextureRef Ret = NULL;
#if defined(MJ_OS_IOS) || defined(MJ_OS_MAC)
	if (m_DeviceType == kUnityGfxRendererMetal)
	{
		return ConvertTexture_Metal(TID , TextureType);
	}
	else
#endif
	{//程序执行到这里，表面不是Metal
#ifdef MJ_OS_WIN32		
		long long Temp = (long long)TID;
		outTexture = Temp;
#else
		outTexture = (long)TID;
#endif		
	}
	return NULL;
}
