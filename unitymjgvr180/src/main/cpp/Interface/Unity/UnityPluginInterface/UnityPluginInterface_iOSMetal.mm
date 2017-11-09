#import "UnityPluginInterface_iOSMetal.h"

#import <UIKit/UIKit.h>
#import "MojingIOSBase.h"
#import "MJGamepad.h"
#import "MJTuner.h"
//#import "MJiCadePad.h"
#import "MJiCadeTransfor.h"
#import "MJGameController.h"
#import "MojingGamepad.h"

#include "../../../MojingAPI.h"
#include "../../../MojingSDKStatus.h"
#include "../../iOS/MojingIOSAPI.h"
#include "../../../MojingManager.h"
#include "../../../Parameters/MojingParameters.h"
#include "../../../Parameters/MojingDeviceParameters.h"
#ifdef LOG4CPLUS_IMPORT
#include "3rdPart/log4cplus/LogInterface.h"
#else
#include "../../../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#include "../UnityIOSAPI.h"
#include "../../../Render/Metal/MojingRenderMetal.h"
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
/* Unity 5.2, 5.3 Metal Interface */
#include "../Unity/UnityInterface.h" // we use 1.UnityGetMetalDevice, 2.UnityRenderBufferMTLTexture
/* Unity 5.4 Metal Interface */
#include "../Unity/IUnityGraphicsMetal.h"

extern int g_GamePad_AxisMode;
namespace Baofeng
{
	namespace Mojing
	{
        
		UnityPluginInterface_iOSMetal::UnityPluginInterface_iOSMetal()
		{
			SetClassName(__FUNCTION__);
		}


		UnityPluginInterface_iOSMetal::~UnityPluginInterface_iOSMetal()
		{
		}

		// 1、OnEnable中，先调用Unity_SetMojingWorld改变GlassName和TimeWarpStatus，然后触发
		void UnityPluginInterface_iOSMetal::OnEvent_EnterMojingWorld()
		{
			ENTER_MINIDUMP_FUNCTION;
			MOJING_FUNC_TRACE(g_APIlogger);
            
            MojingSDK_SetEngineVersion(m_strEngineVersion.c_str());
            
			MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
			if (!pStatus->IsMojingSDKEnbaled())
			{
				MOJING_ERROR(g_APIlogger, "EnterMojingWorld without Init SDK!");
				return ;
			}
        	
       
			if (MojingSDK_ChangeMojingWorld(GetGlassName().c_str()))
			{
				if (pStatus->GetEngineStatus() != ENGINE_UNREAL)
				{
//					OnEvent_HideMetalLayer();
                    
                    MTLDeviceRef pMetalDevice = g_UnityPluginStatus.GetMetalDevice();
                    MOJING_TRACE(g_APIlogger , "pMetalDevice = " << pMetalDevice);
					MojingRenderMetal::CreateCurrentRenderMetal(false, false, g_UnityPluginStatus.GetGraphicsMetal(), nullptr, nullptr, nullptr);
                    
                    SetIsInMojingWorld(true);
				}
			}
            else
            {
                MOJING_ERROR(g_APIlogger, "MojingSDK_ChangeMojingWorld ERROR!");
            }
        	return ;
		}

			// 2、WaitForEndOfFrame 后调用SetTextureID，随后触发本事件
		void UnityPluginInterface_iOSMetal::OnEvent_SetTextureID()
		{// 这个函数不需要工作
			MOJING_FUNC_TRACE(g_APIlogger);
		}

			// 3、SetTextureID 后立即调用SetTextureID
		void UnityPluginInterface_iOSMetal::OnEvent_DistortFrame()
		{
            MOJING_FUNC_TRACE(g_APIlogger);
			MojingRenderMetal* metalRender = MojingRenderMetal::GetInstance();
            if (metalRender == NULL)
            {
                MOJING_ERROR(g_APIlogger, "Can not get MojingRenderMetal!");
                return;
            }
			OnEvent_CheckTimeWarpState();
            //OnEvent_CheckMetalLayer();
			OnEvent_SetOverlay(metalRender);// 这个函数中设置Overlay的矩形
			OnEvent_SetCenterLine();
            MOJING_TRACE(g_APIlogger,
                         "LeftTextureID = " << m_TextureID.m_mtlLeftTextureID << " , " <<
                          "RightTextureID = " << m_TextureID.m_mtlRightTextureID << " , " <<
                         "LeftOverlay = " << m_LeftOverlay.m_mtlTextureID << " , " <<
                         "RightOverlay = " << m_RightOverlay.m_mtlTextureID
                         );
			metalRender->WarpToScreen(UnityGetMetalDrawable(), //m_pMojingMetalLayer.currentDrawable,
                                  m_TextureID.m_mtlLeftTextureID,
                                  m_TextureID.m_mtlRightTextureID,
                                  m_LeftOverlay.m_mtlTextureID,
                                  m_RightOverlay.m_mtlTextureID);
		}
			
		void UnityPluginInterface_iOSMetal::OnEvent_LeaveMojingWorld()
		{
            MOJING_FUNC_TRACE(g_APIlogger);
			UnityPluginInterface_iOSGL::OnEvent_LeaveMojingWorld();
		}	
			// 5、用户修改镜片的时候调用
		void UnityPluginInterface_iOSMetal::OnEvent_ChangeMojingWorld()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
			if (!pStatus->IsMojingSDKEnbaled())
			{
				MOJING_ERROR(g_APIlogger, "EnterMojingWorld without Init SDK!");
				return ;
			}
        
			if (MojingSDK_ChangeMojingWorld(GetGlassName().c_str()))
			{
				if (pStatus->GetEngineStatus() != ENGINE_UNREAL)
				{
					// OnEvent_HideMetalLayer();
					// MojingRenderMetal::CreateCurrentRenderMetal(false, false, g_UnityPluginStatus.GetGraphicsMetal(), nullptr, nullptr, nullptr);
                
				}
			}
		}


	}
}
#endif // #if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
