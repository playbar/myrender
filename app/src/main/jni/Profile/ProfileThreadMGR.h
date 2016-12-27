#pragma once
#ifdef MJ_OS_ANDROID
#include <jni.h>
#endif
#include "../Base/MojingList.h"
#include "../Base/MojingThreads.h"
#include "../Base/MojingString.h"
#include "../Base/MojingJson.h"
#include "InternetProfile.h"
#include <list>

#define REPORTER_URL_ENCRYPT  //是否做报数加密

namespace Baofeng
{
	namespace Mojing
	{
		class MojingJsonElement;
		extern unsigned char g_EncKey[16];
		enum ProfileIO
		{
			Profile_SAVE,
			Profile_LOAD,
		};		
		
		class ProfileThreadMGR :public Thread
		{
			static Mutex* g_mtAllReporter;
			static std::list<Thread*> g_AllReporter;
			

			ProfileThreadMGR();
			virtual ~ProfileThreadMGR();
			static bool CreateLoaclProfileThread(String szURL, String szClassName, MojingJsonElement *pElement, ProfileIO IO);
			static String GetLocalProfileName(String szClassName);
			static String GetInternetURL(String szClassName , ProfileIO IO);
			static ProfileThreadMGR* g_pReporterMGR;
			static void AppendThreadToRun(Thread*);
		public:
			virtual int Run();
			
			static bool UpdateProfile(String szClassName, MojingJsonElement *pElement, ProfileIO IO);
			static void UpdateInternetProfile(String szClassName, String strParamData, ProfileIO IO, FP_InternetProfileCallBack fpCallBack, void * CallBackParam, const char* szTransportFiles = NULL , bool bWait = false);
		};
	}
}