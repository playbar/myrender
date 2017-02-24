#include "UnityPluginInterface_iOSGL.h"
#import <UIKit/UIKit.h>
#import "MojingIOSBase.h"
#import "MJGamepad.h"
#import "MJTuner.h"
//#import "MJiCadePad.h"
#import "MJiCadeTransfor.h"
#import "MJGameController.h"
#import "MojingGamepad.h"
#include "../../../MojingAPI.h"
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

extern int g_GamePad_AxisMode;
namespace Baofeng
{
	namespace Mojing
	{
		UnityPluginInterface_iOSGL::UnityPluginInterface_iOSGL()
		{
			SetClassName(__FUNCTION__);
		}


		UnityPluginInterface_iOSGL::~UnityPluginInterface_iOSGL()
		{
		}

		bool UnityPluginInterface_iOSGL::Init(__tagUnityInterfaceInitParams *pInitParameters)
		{
            MOJING_FUNC_TRACE(g_APIlogger);
            MOJING_TRACE(g_APIlogger, "Init under iOS...");
            bool bRet = UnityPluginInterfaceBase::Init(pInitParameters);
            
            g_GamePad_AxisMode = GAMEPAD_AXIS_DIRECTION;
            
            return bRet;
        }
        
        void UnityPluginInterface_iOSGL::AppResume()
        {
			MojingSDK_AppResume(GetUUID());
		}

		void UnityPluginInterface_iOSGL::AppPause()
		{
			MojingSDK_AppPause();
		}

        Matrix4f UnityPluginInterface_iOSGL::GetLastHeadView()
        {
            Matrix4f Ret;
            float f[16];
            MojingSDK_API_getLastHeadView(f);
            memcpy(&Ret.M[0][0], f, sizeof(float)* 16);
            return Ret;
        }
	}
}

