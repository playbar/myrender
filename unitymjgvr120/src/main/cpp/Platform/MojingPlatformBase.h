#pragma once
#include "../Base/MojingTypes.h"
#include "../Base/MojingString.h"
#include "../Parameters/MojingDisplayParameters.h"
#include "../Parameters/MojingDeviceParameters.h"

#ifdef MJ_OS_ANDROID
#include <jni.h>
#endif

namespace Baofeng
{
	namespace Mojing
	{
		class MojingPlatformBase
		{
			static MojingPlatformBase* g_pPlatform;
		protected:
			MojingPlatformBase();
			MojingPlatformBase(int nWidth, int nHeight, float xdpi, float ydpi, const char* Brand, const char* Model, const char* Serial);
		public:			
			virtual ~MojingPlatformBase();
			static MojingPlatformBase* GetPlatform();
			
			virtual MojingDisplayParameters * CreateDisplayParameters() { return &m_DisplayParam; };
			virtual MojingDeviceParameters * CreateDeviceParameters() { return &m_DeviceParam; };
			

			static char* GetDefaultLocalLogPath();
			static unsigned char GetPlatformID();
			CLASS_INTERFACE_STR(String, m_str, OSString);
			CLASS_MEMBER_STR(String, m_str, CPUName);
			CLASS_MEMBER_STR(String, m_str, GPUName);
			CLASS_MEMBER_STR(String, m_str, DefaultLocalProfilePath);
			CLASS_MEMBER_STR(String, m_str, PacketProfilePath);
			CLASS_MEMBER_STR(String, m_str, MobileConfigVersion);
			CLASS_MEMBER_STR(String, m_str, GlassesConfigVersion);
			CLASS_MEMBER_STR(String, m_str, MerchantID);
			CLASS_MEMBER_STR(String, m_str, AppID);
			CLASS_MEMBER_STR(String, m_str, AppKey);
			CLASS_MEMBER_STR(String, m_str, AppName);
			CLASS_MEMBER_STR(String, m_str, PackageName);
			CLASS_MEMBER_STR(String, m_str, UserID);
			CLASS_MEMBER_STR(String, m_str, RunID);
			CLASS_MEMBER_STR(String, m_str, ChannelID);
			static bool  GetIsMachine(){ return GetPlatform()->m_DeviceParam.GetIsMachine(); };
		private:
			MojingDisplayParameters m_DisplayParam;
			MojingDeviceParameters  m_DeviceParam;
			void SetDefaultLocalProfilePath();
			void ReadCPUName();

		public:
			static void InitPlatform(int nWidth, int nHeight, float xdpi, float ydpi, const char* Brand, const  char* Model, const  char* Serial, const char* szMerchantID, const char* szAppID,
				const char* szAppKey, const char* szAppName, const char* szPackageName, const char* szUserID, const char* szChannelID, const char* PacketProfilePath);

// #ifdef MJ_OS_ANDROID
// 			static void InitPlatform(JNIEnv *env, jobject displayMetrics);
// #endif

		};
	};// end of namespace mojing
};// end of namespace baofeng