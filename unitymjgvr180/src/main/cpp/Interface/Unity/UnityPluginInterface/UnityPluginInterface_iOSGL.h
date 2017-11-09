#pragma once
#include "UnityPluginInterfaceBase.h"
namespace Baofeng
{
	namespace Mojing
	{
		class UnityPluginInterface_iOSGL :
			public UnityPluginInterfaceBase
		{
            friend class UnityPluginInterfaceBase;
        protected:
            UnityPluginInterface_iOSGL();
			virtual ~UnityPluginInterface_iOSGL();
            virtual bool Init(__tagUnityInterfaceInitParams *pInitParameters);
        public:
            virtual void AppResume();

			virtual void AppPause();
            
            virtual Matrix4f GetLastHeadView();
		};


	}
}
