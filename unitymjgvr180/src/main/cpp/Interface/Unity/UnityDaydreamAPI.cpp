//
//  MojingWinAPI.cpp
//  MojingSDK
//
//  Created by 汤申亮 on 16/1/25.
//  Copyright (c) 2016年 baofeng. All rights reserved.
//

#include <unistd.h>						// usleep, etc
#include "../../Base/MojingTypes.h"
#include "../../Base/MojingMath.h"
#include "../../MojingAPI.h"
#include "../../MojingManager.h"
#include "../../MojingSDKStatus.h"
#include "../../Platform/MojingPlatformBase.h"
#include "../../Parameters/MojingParameters.h"
#include "../../Render/MojingRenderBase.h"

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#ifdef MJ_OS_ANDROID
#include "../../Platform/Android/MojingAndroidDeviceInfo.h"
extern JavaVM* gJavaVM;
extern char *g_pPacketName;
extern char *g_pAPPName;
extern char *g_pUserID;
#define UNITY_EXPORT
#endif

/*+++++ most interface define in UnityPlugin.cpp +++++*/
//#include "UnityPlugin.cpp"
/*+++++++++++++++++++++++++++++++++++++++++++++++*/

extern "C"
{   
	static String GetUUID()
	{
		String strUUID = "000000000-000-000-00000000000";
		if (gJavaVM)
		{
			JNIEnv	* Jni = NULL;
			const jint rtn = gJavaVM->AttachCurrentThread(&Jni, 0);
			if (rtn == JNI_OK && Jni != NULL)
			{
				jclass objclass = Jni->FindClass("com/baofeng/mojing/MojingSDKReport");
				jmethodID getUUID = Jni->GetStaticMethodID(objclass, "getUUID", "()Ljava/lang/String;");
				jstring jstrUUID = (jstring)Jni->CallStaticObjectMethod(objclass, getUUID);
				const char* szUUID = Jni->GetStringUTFChars(jstrUUID, 0);
				strUUID = szUUID;
				Jni->ReleaseStringUTFChars(jstrUUID, szUUID);
				gJavaVM->DetachCurrentThread();
			}
		}
	
		return strUUID;
	}
	/*为调试新接口暂时改名*/
	UNITY_EXPORT bool Unity_Init_DayDream(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szChannelID, int nWidth, int nHeight, float xdpi, float ydpi, const char* szProfilePath)
	{    
		MOJING_FUNC_TRACE(g_APIlogger);
		MOJING_TRACE(g_APIlogger, "Init under Android for Daydream...");
		bool bRet = false;
    
		char Brand[PROP_VALUE_MAX], Model[PROP_VALUE_MAX], Serial[PROP_VALUE_MAX];
		AndroidProperties::GetBrand(Brand);
		MOJING_TRACE(g_APIlogger, "Brand: " << Brand);
		AndroidProperties::GetModel(Model);
		MOJING_TRACE(g_APIlogger, "Model: " << Model);
		AndroidProperties::GetSerialNo(Serial);
		MOJING_TRACE(g_APIlogger, "Serial: " << Serial);
       
		const char * szAppName = g_pAPPName;
		MOJING_TRACE(g_APIlogger, "AppName: " << szAppName);
		const char* szPackageName = g_pPacketName;
		MOJING_TRACE(g_APIlogger, "PackageName: " << szPackageName);
		const char * szUserID = g_pUserID;
		MOJING_TRACE(g_APIlogger, "UserID: " << szUserID);
    
	#ifdef _DEBUG
		MOJING_TRACE(g_APIlogger, "MerchantID: " << szMerchantID << ", AppID: " << szAppID << ", AppKey: " << szAppKey << ", ChannelID: " << szChannelID);
	#endif

		MOJING_TRACE(g_APIlogger, "Screen Size: " << nWidth << " X " << nHeight);
		MOJING_TRACE(g_APIlogger, "Screen DPI:  " << xdpi << " X " << ydpi);
    
		if(szProfilePath != NULL)
			MOJING_TRACE(g_APIlogger, "ProfilePath: " << szProfilePath);
    
		bRet = MojingSDK_Init(nWidth, nHeight, xdpi, ydpi, Brand, Model, Serial, szMerchantID, szAppID, szAppKey, szAppName, szPackageName, szUserID, szChannelID, szProfilePath);

		MOJING_TRACE(g_APIlogger, "Unity_Init. ret: " << bRet);

		return bRet;
	}
    
	UNITY_EXPORT void Unity_AppResume()
	{
		MojingSDK_AppResume(GetUUID().ToCStr());
	}

	UNITY_EXPORT void Unity_AppPause()
	{
		MojingSDK_AppPause();
	}
}	// extern "C"     
