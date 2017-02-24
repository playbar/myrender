#pragma once
#include "MojingPlatformBase.h"

#ifdef MJ_OS_ANDROID
namespace Baofeng
{
	namespace Mojing
	{
		class MojingPlatformAndroid : public MojingPlatformBase
		{
			char * GetBuildFieldString(JNIEnv *env , jclass	BuildClass, const char * name);
		public:
			MojingPlatformAndroid(JNIEnv *pEnv , jobject DisplayMetrics);
			virtual ~MojingPlatformAndroid();
			
 			virtual MojingDisplayParameters * CreateDisplayParameters();
 			virtual MojingDeviceParameters * CreateDeviceParameters();
			
#if 0
			virtual String LoadProfileString(String szProfilePath, String szProfileName);
#endif
			CLASS_INTERFACE(JNIEnv * , m_p , Env);
			CLASS_INTERFACE(jobject  , m_ , DisplayMetrics);
		};
	};// end of namespace mojing
};// end of namespace baofeng
#endif
