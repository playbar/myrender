#include "UnityPluginInterfaceWin32.h"
#include "../../../MojingAPI.h"
#include "../../../MojingManager.h"
#include "../../../Parameters/MojingParameters.h"
#include "../../../Parameters/MojingDisplayParameters.h"

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif


#ifdef MJ_OS_WIN32
namespace Baofeng
{
	namespace Mojing
	{

		UnityPluginInterfaceWin32::UnityPluginInterfaceWin32()
		{
			SetClassName(__FUNCTION__);
		}


		UnityPluginInterfaceWin32::~UnityPluginInterfaceWin32()
		{
		}

// 		bool UnityPluginInterfaceWin32::Init(__tagUnityInterfaceInitParams *pInitParameters)
// 		{
// 			MOJING_FUNC_TRACE(g_APIlogger);
// 			return UnityPluginInterfaceBase::Init(pInitParameters);
// 		}

		void UnityPluginInterfaceWin32::AppResume()
		{
			MojingSDK_AppResume("000000000-000-000-00000000000");
		}

		void UnityPluginInterfaceWin32::AppPause()
		{
			MojingSDK_AppPause();
		}

		bool UnityPluginInterfaceWin32::IsInMojingWorld(const char * szGlassesName)
		{
			return 0 == strcmp(GetGlassName().c_str() , szGlassesName);
		}
	}
}
#endif