#pragma once
#include "../../../Base/MojingTypes.h"

#include "../Unity/IUnityGraphics.h"
#include "../Unity/IUnityInterface.h"
#include "../Unity/UnityInterface.h"
#include "../MetalForwardDecls.h"
#ifdef MJ_OS_IOS
#import <Metal/Metal.h>
#include "../Unity/IUnityGraphicsMetal.h"
#endif

class UnityPluginStatus
{
public:	
	UnityPluginStatus();
	virtual	~UnityPluginStatus();
	void Init(IUnityInterfaces* unityInterfaces);
	void Release();
	void OnEvent_GraphicsDeviceEvent(UnityGfxDeviceEventType eventType);
    MTLTextureRef ConvertTexture(long & outTexture,
                                 void * TID, EyeTextureType TextureType = TEXTURE_OPEN_GL);
    

#ifdef MJ_OS_IOS
    MTLDeviceRef GetMetalDevice(){return m_pGraphicsMetal == NULL ? UnityGetMetalDevice(): m_pGraphicsMetal->MetalDevice();};
    MTLTextureRef ConvertTexture_Metal(void * TID, EyeTextureType TextureType = TEXTURE_OPEN_GL);

	CLASS_MEMBER(IUnityGraphicsMetal *, m_p, GraphicsMetal);
#endif
    CLASS_MEMBER(IUnityInterfaces *, m_p, UnityInterfaces);
    CLASS_MEMBER(IUnityGraphics *, m_p, UnityGraphics);
	CLASS_MEMBER(UnityGfxRenderer , m_, DeviceType);
};

extern UnityPluginStatus g_UnityPluginStatus;
