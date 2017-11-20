#pragma once
#include "UnityPluginInterfaceBase.h"
#ifdef MJ_OS_WIN32
namespace Baofeng
{
	namespace Mojing
	{
		class UnityPluginInterfaceWin32 :
			public UnityPluginInterfaceBase
		{
			//virtual bool Init(__tagUnityInterfaceInitParams *pInitParameters);
		public:
			UnityPluginInterfaceWin32();
			virtual ~UnityPluginInterfaceWin32();

			virtual void AppResume();

			virtual void AppPause();

			virtual bool IsInMojingWorld(const char * szGlassesName);
		};


	}
}
#endif