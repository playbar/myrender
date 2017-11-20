#include "MojingAPI.h"
#include "MojingAndroipAPI.h"
#include "MojingManager.h"
#include "MojingSDKStatus.h"

#include "Tracker/MojingTracker.h"
#include "Tracker/AndroidInternalSensorChecker.h"
#include "Base/MojingSystem.h"
#include "Platform/MojingPlatformBase.h"
#include "Render/MojingRenderBase.h"
#include "Profile/GlassesConfigProfile.h"
#include "Profile/MobileConfigProfile.h"

#include "Reporter/GyroTempCalibrationReporter.h"
#include "Reporter/MobileConfigUpdate.h"
#include "Distortion/MojingDistortion.h"
#include "Base/MojingString.h"
#include "Base/MojingMath.h"
#include "Platform/MojingPlatformBase.h"

#include <unistd.h>
#include <android/native_window_jni.h>	// for native window JNI
#include <sys/system_properties.h>
#include <Hook/HookGVR/HookGVRTools.h>
#include <Hook/eglhook/elf_eglhook.h>

//#include <android/log.h>

#include "3rdPart/MD5/MD5.h"
#include "3rdPart/AES/AES.h"
#include "../../Platform/Android/MojingAndroidDeviceInfo.h"

#include "Profile/ProfileThreadMGR.h"
#include "Profile/JoystickProfile.h"
using namespace Baofeng::Mojing;

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

JavaVM* gJavaVM = NULL;
char *g_pPacketName = NULL;
char *g_pAPPName = NULL;
char *g_pUserID = NULL;
//jobject gobject_MojingSDKServiceManager;

//void initClassHelper(JNIEnv *env, const char *path, jobject *objptr) {
//    jclass cls = env->FindClass(path);
//    if(!cls) {
//    	__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "initClassHelper: failed to get %s class reference", path);
//        return;
//    }
//    jmethodID constr = env->GetMethodID(cls, "<init>", "()V");
//    if(!constr) {
//    	__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "initClassHelper: failed to get %s constructor", path);
//        return;
//    }
//    jobject obj = env->NewObject(cls, constr);
//    if(!obj) {
//    	__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "initClassHelper: failed to create a %s object", path);
//        return;
//    }
//    (*objptr) = env->NewGlobalRef(obj);
//}



JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	MOJING_TRACE(g_APIlogger, "JNI_OnLoad");
	gJavaVM = vm;
	if (gJavaVM == NULL)
	{
		MOJING_TRACE(g_APIlogger, "===> gJavaVM == NULL");
	}
	else
	{
		MOJING_TRACE(g_APIlogger, "===> gJavaVM = " << gJavaVM);
	}

	JNIEnv *pEnv = NULL;
	if (g_pPacketName == NULL && JNI_OK == vm->GetEnv((void**)&pEnv, JNI_VERSION_1_6))
	{
		jclass activityThread = pEnv->FindClass( "android/app/ActivityThread");

		jmethodID currentActivityThread = (pEnv)->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");

		jobject at = (pEnv)->CallStaticObjectMethod(activityThread, currentActivityThread);

		jmethodID getApplication = (pEnv)->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");

		jobject contextObj = (pEnv)->CallObjectMethod(at, getApplication);

		jclass contextCls = (pEnv)->GetObjectClass(contextObj);
		// String pkName = context.getPackageName();
		jmethodID getPackageName = (pEnv)->GetMethodID(contextCls, "getPackageName", "()Ljava/lang/String;");
		jstring jsPackageName = (jstring)(pEnv)->CallObjectMethod(contextObj, getPackageName);
		const char* szPackageName = pEnv->GetStringUTFChars(jsPackageName, 0);
		g_pPacketName = (char *)malloc(strlen(szPackageName) + 1);
		strcpy(g_pPacketName, szPackageName);
		pEnv->ReleaseStringUTFChars(jsPackageName, szPackageName);

		MOJING_TRACE(g_APIlogger, "JNI_OnLoad get package name : " << g_pPacketName);

		jclass objclass = pEnv->FindClass("com/baofeng/mojing/MojingSDK");
		jmethodID getAPPName = pEnv->GetStaticMethodID(objclass, "getApplicationName", "(Landroid/content/Context;)Ljava/lang/String;");
		jstring jsAPPName = (jstring)(pEnv)->CallStaticObjectMethod(objclass, getAPPName, contextObj);
		const char* szAPPName = pEnv->GetStringUTFChars(jsAPPName, 0);
		g_pAPPName = (char *)malloc(strlen(szAPPName) + 1);
		strcpy(g_pAPPName, szAPPName);
		pEnv->ReleaseStringUTFChars(jsAPPName, szAPPName);	

		jmethodID getUserID = pEnv->GetStaticMethodID(objclass, "getUserID", "(Landroid/content/Context;)Ljava/lang/String;");
		jstring jsUserID = (jstring)(pEnv)->CallStaticObjectMethod(objclass, getUserID, contextObj);
		const char* szUserID = pEnv->GetStringUTFChars(jsUserID, 0);
		g_pUserID = (char *)malloc(strlen(szUserID) + 1);
		strcpy(g_pUserID, szUserID);
		pEnv->ReleaseStringUTFChars(jsUserID, szUserID);

		MOJING_TRACE(g_APIlogger, "JNI_OnLoad get app name : " << g_pAPPName << " UserID : " << g_pUserID);

//		initClassHelper(pEnv, "com/baofeng/mojing/MojingSDKServiceManager", &gobject_MojingSDKServiceManager);
	}

	return JNI_VERSION_1_6;
}

JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_GetSystemIntProperty(JNIEnv *jEnv, jclass, jstring sProperty, jint defaultValue)
{
	const char* jProperty = jEnv->GetStringUTFChars(sProperty, 0);
	char value[PROP_VALUE_MAX];
	int length = __system_property_get(jProperty, value);
	jEnv->ReleaseStringUTFChars(sProperty, jProperty);

	if (length > 0)
	{
		return atoi(value);
	}
	else
	{
		return defaultValue;
	}
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_Log(JNIEnv *jEnv, jclass, jint logLevel, jstring sInfo, jstring sFileName, jint line)
{
	// USING_MINIDUMP;
#ifdef ENABLE_LOGGER
	if (g_APIlogger.m_nLogLevel <= logLevel) 
	{
		const char* jInfo = jEnv->GetStringUTFChars(sInfo, 0);
		const char* jFileName = jEnv->GetStringUTFChars(sFileName, 0);
		g_APIlogger.Log(logLevel, jInfo, jFileName, line);
		jEnv->ReleaseStringUTFChars(sInfo, jInfo);
		jEnv->ReleaseStringUTFChars(sFileName, jFileName);
	}
#endif
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_VrFuncInit(JNIEnv *jEnv, jclass, jboolean isdd)
{
    HookGVRTools::Init();
//    hookGLFun();
//    startHookThread(isdd);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_ReprojFuncInit(JNIEnv *jEnv, jclass)
{
	hookEglGetProcAddress();
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeUnityFuncInit(JNIEnv *jEnv, jclass)
{
    hookUnityFun();
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_Init(JNIEnv *env, jclass jClass, jstring merchantID, jstring appID, jstring appKey, jstring appName, jstring packageName,
					jstring userID, jstring channelID, jint nWidth, jint nHeight, jfloat xdpi, jfloat ydpi, jstring ProfilePath)
{
	// USING_MINIDUMP;
	
#ifdef _DEBUG
	sleep(2);
#else
	// 注意：这个地方如果不加sleep 500ms的话，在S7 edge上会卡死,或者五代无效
	usleep(400 * 1000);
#endif

	MOJING_FUNC_TRACE(g_APIlogger);
	MOJING_TRACE(g_APIlogger, "Init under ANDROID");
	bool bRet = false;

	char Brand[PROP_VALUE_MAX], Model[PROP_VALUE_MAX], Serial[PROP_VALUE_MAX];
	AndroidProperties::GetBrand(Brand);
	MOJING_TRACE(g_APIlogger, "Brand: " << Brand);
	AndroidProperties::GetModel(Model);
	MOJING_TRACE(g_APIlogger, "Model: " << Model);
	AndroidProperties::GetSerialNo(Serial);
	MOJING_TRACE(g_APIlogger, "Serial: " << Serial);
	const char * szProfilePath = env->GetStringUTFChars(ProfilePath, 0);
	const char * szMerchantID = env->GetStringUTFChars(merchantID, 0);
	const char * szAppID = env->GetStringUTFChars(appID, 0);
	const char * szAppKey = env->GetStringUTFChars(appKey, 0);
	const char * szAppName = env->GetStringUTFChars(appName, 0);
	const char * szPackageName = env->GetStringUTFChars(packageName, 0);
#ifdef _DEBUG
	MOJING_TRACE(g_APIlogger, "MerchantID: " << szMerchantID << ", AppID: " << szAppID << ", AppKey: " << szAppKey << ", AppName: " << szAppName << ", PackageName: " << szPackageName);
#endif
	const char * szUserID = env->GetStringUTFChars(userID, 0);
	const char * szChannelID = env->GetStringUTFChars(channelID, 0);
	MOJING_TRACE(g_APIlogger, "ChannelID: " << szChannelID << ", UserID: " << szUserID);

	bRet = MojingSDK_Init(nWidth, nHeight, xdpi, ydpi, Brand, Model, Serial, szMerchantID, szAppID, szAppKey, szAppName, szPackageName, szUserID, szChannelID, szProfilePath);

	env->ReleaseStringUTFChars(ProfilePath, szProfilePath);
	env->ReleaseStringUTFChars(merchantID, szMerchantID);
	env->ReleaseStringUTFChars(appID, szAppID);
	env->ReleaseStringUTFChars(appKey, szAppKey);
	env->ReleaseStringUTFChars(packageName, szPackageName);
	env->ReleaseStringUTFChars(appName, szAppName);
	env->ReleaseStringUTFChars(userID, szUserID);
	env->ReleaseStringUTFChars(channelID, szChannelID);
	return bRet;
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_CheckPackage(JNIEnv *env, jclass, jstring jstrAppName, jstring jstrCaseCode)
{
	const char * szAppName = env->GetStringUTFChars(jstrAppName, 0);
	const char * szCaseCode = env->GetStringUTFChars(jstrCaseCode, 0);

	MojingSDK_CheckPackage(szAppName, szCaseCode);

	env->ReleaseStringUTFChars(jstrAppName, szAppName);
	env->ReleaseStringUTFChars(jstrCaseCode, szCaseCode);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetEngineVersion(JNIEnv *env, jclass, jstring jstrEngineVersion)
{
	const char * szEngineVersion = env->GetStringUTFChars(jstrEngineVersion, 0);
	MojingSDK_SetEngineVersion(szEngineVersion);
	env->ReleaseStringUTFChars(jstrEngineVersion, szEngineVersion);
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_IsUseUnityForSVR(JNIEnv *env, jclass)
{
	bool bRet = MojingSDK_IsUseUnityForSVR();
	MOJING_TRACE(g_APIlogger, "IsUseUnityForSVR: " << bRet);
	
	return bRet;
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_IsInMachine(JNIEnv *env, jclass)
{
	bool bRet = MojingSDK_IsInMachine();
	MOJING_TRACE(g_APIlogger, "IsInMachine: " << bRet);

	return bRet;
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_IsUseForDayDream(JNIEnv *env, jclass)
{
    bool bRet = MojingSDK_IsUseForDayDream();
    MOJING_TRACE(g_APIlogger, "IsUseForDayDream: " << bRet);

    return bRet;
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_ReportReInit(JNIEnv *env, jclass)
{
	return MojingSDK_ReportReInit();
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppExit(JNIEnv *env, jclass)
{
	MojingSDK_AppExit();
}

JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_AppGetRunID(JNIEnv *env, jclass)
{
	Baofeng::Mojing::MojingPlatformBase *pPlatform = Baofeng::Mojing::MojingPlatformBase::GetPlatform();
	return env->NewStringUTF(pPlatform->GetRunID());
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_AppResume(JNIEnv *env, jclass, jstring uniqueID)
{
	const char * szUniqueID = env->GetStringUTFChars(uniqueID, 0);
	bool bRet = MojingSDK_AppResume(szUniqueID);
	env->ReleaseStringUTFChars(uniqueID, szUniqueID);

	return bRet;
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppPause(JNIEnv *env, jclass)
{
	MojingSDK_AppPause();
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppPageStart(JNIEnv *env, jclass, jstring pageName)
{
	const char * szPageName = env->GetStringUTFChars(pageName, 0);
	MojingSDK_AppPageStart(szPageName);
	env->ReleaseStringUTFChars(pageName, szPageName);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppPageEnd(JNIEnv *env, jclass, jstring pageName)
{
	const char * szPageName = env->GetStringUTFChars(pageName, 0);
	MojingSDK_AppPageEnd(szPageName);
	env->ReleaseStringUTFChars(pageName, szPageName);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppSetEvent(JNIEnv *env, jclass, jstring eventName, jstring eChannelID, jstring eInName, jfloat eInData, jstring eOutName, jfloat eOutData)
{
	const char * szEventName = env->GetStringUTFChars(eventName, 0);
	const char * szEventChannelID = env->GetStringUTFChars(eChannelID, 0);
	const char * szEventInName = env->GetStringUTFChars(eInName, 0);
	const char * szEventOutName = env->GetStringUTFChars(eOutName, 0);
//	MOJING_TRACE(g_APIlogger, "EventName: " << szEventName << ", ChanneldID: " << szEventChannelID << ", InName: " << szEventInName << ", InData: " << eInData << ", OutData: " << szEventOutName << ", OutData: " << eOutData );

	MojingSDK_AppSetEvent(szEventName, szEventChannelID, szEventInName, eInData, szEventOutName, eOutData);

	env->ReleaseStringUTFChars(eventName, szEventName);
	env->ReleaseStringUTFChars(eChannelID, szEventChannelID);
	env->ReleaseStringUTFChars(eInName, szEventInName);
	env->ReleaseStringUTFChars(eOutName, szEventOutName);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppReportLog(JNIEnv *env, jclass, jint iLogType, jstring typeName, jstring logContent)
{
	const char * szTypeName = env->GetStringUTFChars(typeName, 0);
	const char * szLogContent = env->GetStringUTFChars(logContent, 0);
	//MOJING_TRACE(g_APIlogger, "TypeName: " << szTypeName << ", LogType: " << iLogType << ", LogContent: " << szLogContent);

	MojingSDK_ReportLog(iLogType, szTypeName, szLogContent, false);

	env->ReleaseStringUTFChars(typeName, szTypeName);
	env->ReleaseStringUTFChars(logContent, szLogContent);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppReportUserAction(JNIEnv *env, jclass, jstring strActionType, jstring strItemID, jstring strJsonValue)
{
	const char * szActionType = env->GetStringUTFChars(strActionType, 0);
	const char * szItemID = env->GetStringUTFChars(strItemID, 0);
	const char * szJsonValue = env->GetStringUTFChars(strJsonValue, 0);

	MojingSDK_ReportUserAction(szActionType, szItemID, szJsonValue);

	env->ReleaseStringUTFChars(strActionType, szActionType);
	env->ReleaseStringUTFChars(strItemID, szItemID);
	env->ReleaseStringUTFChars(strJsonValue, szJsonValue);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppSetContinueInterval(JNIEnv *env, jclass, jint interval)
{
	MojingSDK_AppSetContinueInterval(interval);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppSetReportInterval(JNIEnv *env, jclass, jint interval)
{
	MojingSDK_AppSetReportInterval(interval);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppSetReportImmediate(JNIEnv *env, jclass, jboolean bImmdediate)
{
	MojingSDK_AppSetReportImmediate(bImmdediate);
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_StartTracker(JNIEnv *jEnv, jclass, jint nSampleFrequence)
{
	// USING_MINIDUMP;
	MOJING_FUNC_TRACE(g_APIlogger);
	return MojingSDK_StartTracker(nSampleFrequence);
}


JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_StartGlassTracker(JNIEnv *jEnv, jclass, jstring glassName)
{
	// USING_MINIDUMP;
	bool bRet;

	const char * szGlassName = jEnv->GetStringUTFChars(glassName, 0);
	MOJING_TRACE(g_APIlogger, "StartGlassTracker GlassName: " << szGlassName);
	int nSampleFrequence = 250; //for mojing5
	if ( MJ_stricmp(szGlassName, "FromJAVA") == 0 || MJ_stricmp(szGlassName, "FromNative") == 0)
	{
		//for self-device
		nSampleFrequence = 100;
	}
	bRet = MojingSDK_StartTracker(nSampleFrequence, szGlassName);
	jEnv->ReleaseStringUTFChars(glassName, szGlassName);

	return bRet;
}

JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_CheckSensors(JNIEnv *, jclass)
{
	// USING_MINIDUMP;
	return MojingSDK_CheckSensors();
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_StartTrackerCalibration(JNIEnv *jEnv, jclass)
{
	// USING_MINIDUMP;
	return MojingSDK_StartTrackerCalibration() >= 0;
}

JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_IsTrackerCalibrated(JNIEnv *jEnv, jclass)
{
	// USING_MINIDUMP;
	return MojingSDK_IsTrackerCalibrated();
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_ResetSensorOrientation(JNIEnv *jEnv, jclass)
{
	// USING_MINIDUMP;
	MojingSDK_ResetSensorOrientation();
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_ResetTracker(JNIEnv *jEnv, jclass)
{
	// USING_MINIDUMP;
	MojingSDK_ResetTracker();
}

JNIEXPORT jdouble JNICALL Java_com_baofeng_mojing_MojingSDK_getLastSensorState(JNIEnv *jEnv, jclass, jfloatArray jArray)
{
	// Copy to jflotArray
	jdouble dRet = 0;
	jfloat* pArray = jEnv->GetFloatArrayElements(jArray, NULL);
	jsize length = (jEnv)->GetArrayLength(jArray);
	if (length < 10)
	{
		MOJING_ERROR(g_APIlogger, "The array for Euler Angle is too small. Need 10 while only %d" << length);
	}
	else
	{
		dRet = MojingSDK_getLastSensorState(pArray);
	}

	jEnv->ReleaseFloatArrayElements(jArray, pArray, 0);

	return dRet;
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_getLastHeadView(JNIEnv *jEnv, jclass, jfloatArray jViewMatrix)
{
	// USING_MINIDUMP;
	// Copy to jflotArray
	jfloat* pMatrixArray = jEnv->GetFloatArrayElements(jViewMatrix, NULL);
	jsize length = (jEnv)->GetArrayLength(jViewMatrix);
	if (length < 16)
	{
		MOJING_ERROR(g_APIlogger, "The array for Euler Angle is too small. Need 16 while only %d" << length);
		for (jsize i = 0; i < length; i++)
		{
			pMatrixArray[i] = -1.0f;
		}
	}
	else
	{
		MojingSDK_getLastHeadView(pMatrixArray);
	}
	
	jEnv->ReleaseFloatArrayElements(jViewMatrix, pMatrixArray, 0);
}

JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_getPredictionHeadView(JNIEnv *jEnv, jclass, jfloatArray jViewMatrix, jdouble time)
{
	jint result = -1;
	// USING_MINIDUMP;
	// Copy to jflotArray
	jfloat* pMatrixArray = jEnv->GetFloatArrayElements(jViewMatrix, NULL);
	jsize length = (jEnv)->GetArrayLength(jViewMatrix);
	if (length < 16)
	{
		MOJING_ERROR(g_APIlogger, "The array for Euler Angle is too small. Need 16 while only %d" << length);
		for (jsize i = 0; i < length; i++)
		{
			pMatrixArray[i] = -1.0f;
		}
	}
	else
	{
		result = MojingSDK_getPredictionHeadView(pMatrixArray, time);
	}
	
	jEnv->ReleaseFloatArrayElements(jViewMatrix, pMatrixArray, 0);
	return result;
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_getLastHeadEulerAngles(JNIEnv *jEnv, jclass, jfloatArray jEulerAngles)
{	
	// USING_MINIDUMP;
	jfloat* pEulerAngles = jEnv->GetFloatArrayElements(jEulerAngles, NULL); 
	jsize length = (jEnv)->GetArrayLength(jEulerAngles);
	if (length < 3)
	{
		MOJING_ERROR(g_APIlogger, "The array for Euler Angle is too small. Need 3 while only %d" << length);
		for (jsize i = 0; i < length; i++)
		{
			pEulerAngles[i] = -1.0f;
		}
	}
	else
	{
		MojingSDK_getLastHeadEulerAngles(pEulerAngles);
	}
	jEnv->ReleaseFloatArrayElements(jEulerAngles, pEulerAngles, 0);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_getLastHeadQuarternion(JNIEnv *jEnv, jclass, jfloatArray jUnitQuarternion)
{
	// USING_MINIDUMP;
	// Copy to jflotArray
	jfloat* pUnitQuarternion  = jEnv->GetFloatArrayElements(jUnitQuarternion, NULL);
	jsize length = (jEnv)->GetArrayLength(jUnitQuarternion);
	if (length < 4)
	{
		MOJING_ERROR(g_APIlogger, "The array for quarternion is too small. Need 4 while only %d" << length);
		for (jsize i = 0; i < length; i++)
		{
			pUnitQuarternion[i] = -1.0f;
		}
	}
	else
	{
		float x = 0, y = 0, z = 0, w = 1;
		MojingSDK_getLastHeadQuarternion(&w, &x, &y, &z);
		pUnitQuarternion[0] = w;
		pUnitQuarternion[1] = x;
		pUnitQuarternion[2] = y;
		pUnitQuarternion[3] = z;
	}
	jEnv->ReleaseFloatArrayElements(jUnitQuarternion, pUnitQuarternion, 0);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_getDirectionalInScreen(JNIEnv *jEnv, jclass, jfloatArray jfArray)
{
	jfloat* fArray = jEnv->GetFloatArrayElements(jfArray, NULL);
	jsize length = (jEnv)->GetArrayLength(jfArray);
	if (length == 10) // 4 + 4 + 2
	{
		MojingSDK_Math_DirectionalInScreen(fArray);
	}
	jEnv->ReleaseFloatArrayElements(jfArray, fArray, 0);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_StopTracker(JNIEnv *, jclass)
{
	// USING_MINIDUMP;
	MojingSDK_StopTracker();
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SendSensorData(JNIEnv *jEnv, jclass, jfloatArray jSensortMatrix, jdouble dLastSampleTime)
{
	jfloat* pMatrixArray = jEnv->GetFloatArrayElements(jSensortMatrix, NULL);
	jsize length = (jEnv)->GetArrayLength(jSensortMatrix);
	if (length != 12)
	{
		MOJING_ERROR(g_APIlogger, "The array for sensor datalength is not equal 12, length is %d" << length);
		return;
	}
	MojingSDK_SendSensorData(pMatrixArray, dLastSampleTime);

	jEnv->ReleaseFloatArrayElements(jSensortMatrix, pMatrixArray, 0);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SendControllerData(JNIEnv *jEnv, jclass, jbyteArray jControllerData, jint dataLen)
{
	jbyte* pControllerData = jEnv->GetByteArrayElements(jControllerData, NULL);

	MojingSDK_SendControllerData((const Baofeng::UByte* )pControllerData, dataLen);

	jEnv->ReleaseByteArrayElements(jControllerData, pControllerData, 0);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SendControllerDataV2(JNIEnv *jEnv, jclass, jfloatArray jControllerData, jdouble jTimestamp, jboolean bRecenter)
{
	jfloat* pControllerData = jEnv->GetFloatArrayElements(jControllerData, NULL);
	
	MojingSDK_SendControllerDataV2(pControllerData, jTimestamp, bRecenter);

	jEnv->ReleaseFloatArrayElements(jControllerData, pControllerData, 0);
}


JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSurfaceView_EnterMojingWorld(JNIEnv *env, jclass, jstring GlassesName, jboolean bEnableMultiThread, jboolean bEnableTimeWarp)
{

	// USING_MINIDUMP;
	const char * szGlassesName = env->GetStringUTFChars(GlassesName, 0);
	
	int iSize = strlen(szGlassesName);
	char * szGlassesName2 = new char[iSize / 16 * 16 + 16];
	memset(szGlassesName2, 0, iSize / 16 * 16 + 16);
	strcpy(szGlassesName2 , szGlassesName);
	bool ret = MojingSDK_EnterMojingWorld(szGlassesName2, bEnableMultiThread, bEnableTimeWarp);
	delete[] szGlassesName2;
	env->ReleaseStringUTFChars(GlassesName, szGlassesName);
	return ret;
}

JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_GetMojingWorldFOV(JNIEnv *, jclass)
{
	// USING_MINIDUMP;
	return MojingSDK_GetFOV();
}

JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSurfaceView_ChangeMojingWorld(JNIEnv *env, jclass, jstring GlassesName)
{
	// USING_MINIDUMP;
	MOJING_FUNC_TRACE(g_APIlogger);
	const char * szGlassesName = env->GetStringUTFChars(GlassesName, 0);
	bool ret = MojingSDK_ChangeMojingWorld(szGlassesName);
	env->ReleaseStringUTFChars(GlassesName, szGlassesName);
	return ret;
}

JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSurfaceView_LeaveMojingWorld(JNIEnv *, jclass)
{
	// USING_MINIDUMP;
	return MojingSDK_LeaveMojingWorld();
}

JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSDK_SetDefaultMojingWorld(JNIEnv *jEnv, jclass, jstring strGlassesKey)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	const char * szKey = jEnv->GetStringUTFChars(strGlassesKey, 0);
	bool  bRet = MojingSDK_SetDefaultMojingWorld(szKey);
	jEnv->ReleaseStringUTFChars(strGlassesKey, szKey);

	return bRet;
}

JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetDefaultMojingWorld(JNIEnv *jEnv, jclass, jstring strLanguageCodeByISO639)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	const char * szLanguageName = jEnv->GetStringUTFChars(strLanguageCodeByISO639, 0);
	String  strRet = MojingSDK_GetDefaultMojingWorld(szLanguageName);
	jstring jsRet = jEnv->NewStringUTF(strRet.ToCStr());
	jEnv->ReleaseStringUTFChars(strLanguageCodeByISO639, szLanguageName);

	return jsRet;
}

JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetLastMojingWorld(JNIEnv *jEnv, jclass, jstring strLanguageCodeByISO639)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	const char * szLanguageName = jEnv->GetStringUTFChars(strLanguageCodeByISO639, 0);
	String  strRet = MojingSDK_GetLastMojingWorld(szLanguageName);
	jstring jsRet = jEnv->NewStringUTF(strRet.ToCStr());
	jEnv->ReleaseStringUTFChars(strLanguageCodeByISO639, szLanguageName);

	return jsRet;
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetEnableTimeWarp(JNIEnv *jEnv, jclass, jboolean bEnable)
{
	MojingSDK_SetEnableTimeWarp(bEnable);
}

JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSurfaceView_OnSurfaceChanged(JNIEnv *, jclass, jint newWidth, jint newHeight)
{
	// USING_MINIDUMP;
	MOJING_FUNC_TRACE(g_APIlogger);
	
	MojingSDK_OnSurfaceChanged(newWidth, newHeight);

	return true	;
}
JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_GetEyeTexture(JNIEnv *jEnv, jclass, jint eyeTextureType, jintArray outParams)
{
	// USING_MINIDUMP;
	jint Ret = 0;
	jint* poutParams = jEnv->GetIntArrayElements(outParams, NULL);
	jsize length = (jEnv)->GetArrayLength(outParams);
	if (length < 3)
	{
		MOJING_ERROR(g_APIlogger, "The array for Texture Params is too small. Need 3 while only %d" << length);
		for (jsize i = 0; i < length; i++)
		{
			poutParams[i] = -1.0f;
		}
	}
	else
	{
		unsigned uiWidth, uiHeight, uiFormat;
		Ret = MojingSDK_GetEyeTexture(eyeTextureType, uiWidth, uiHeight, uiFormat);

		poutParams[0] = uiWidth;
		poutParams[1] = uiHeight;
		poutParams[2] = uiFormat;
	}

	jEnv->ReleaseIntArrayElements(outParams, poutParams, 0);
	return Ret;
}

JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_GetGlassesSeparationInPix(JNIEnv *, jclass)
{
	// USING_MINIDUMP;
	MOJING_FUNC_TRACE(g_APIlogger);
	return MojingSDK_GetGlassesSeparationInPix();
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetCenterLine(JNIEnv *, jclass, jint iWidth, jint colR, jint colG, jint colB, jint colA)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDK_SetCenterLine(iWidth, colR,  colG, colB, colA);
}
JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSDK_DrawTexture(JNIEnv *, jclass, jint LeftTexID, jint RightTexID)
{
	// USING_MINIDUMP;
	return MojingSDK_DrawTexture(LeftTexID , RightTexID , 0 , 0);
}

JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSDK_DrawTextureWithOverlay(JNIEnv *, jclass, jint LeftTexID, jint RightTexID, jint LeftLayoutTexID, jint RightLayoutTexID)
{
	return MojingSDK_DrawTexture(LeftTexID, RightTexID, LeftLayoutTexID , RightLayoutTexID);
}
//jxb ADD
JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetOverlayPosition(JNIEnv *jEnv, jclass, jfloat fLeft, jfloat fTop, jfloat fWidth, jfloat fHeight)
{
	MojingSDK_SetOverlayPosition(fLeft, fTop, fWidth, fHeight);
}

void JNICALL Java_com_baofeng_mojing_MojingSDK_SetOverlayPosition3D(JNIEnv *jEnv, jclass, jint eyeTextureType, /*jfloat fLeft, jfloat fTop,*/ jfloat fWidth, jfloat fHeight, jfloat fDistanceInMetre)
{
	MojingSDK_SetOverlayPosition3D(eyeTextureType, /*fLeft, fTop,*/ fWidth, fHeight, fDistanceInMetre);
}
JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetOverlayPosition3DV2(JNIEnv *jEnv, jclass, jint eyeTextureType, jfloat fLeft, jfloat fTop, jfloat fWidth, jfloat fHeight, jfloat fDistanceInMetre)
{
	MojingSDK_SetOverlayPosition3D_V2(eyeTextureType, fLeft, fTop, fWidth, fHeight, fDistanceInMetre);
}


// 获取当前用户设置
JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetUserSettings(JNIEnv *jEnv, jclass)
{
	jstring jsRet = jEnv->NewStringUTF(MojingSDK_GetUserSettings().ToCStr());
	return jsRet;
}

// 修改当前用户设置
JNIEXPORT bool JNICALL  Java_com_baofeng_mojing_MojingSDK_SetUserSettings(JNIEnv *jEnv, jclass, jstring strUserSettings)
{
	const char * szUserSettings = jEnv->GetStringUTFChars(strUserSettings, 0);
	bool bRet = MojingSDK_SetUserSettings(szUserSettings);
	jEnv->ReleaseStringUTFChars(strUserSettings, szUserSettings);
	return bRet;
}

// 获取当前陀螺仪数据来源
JNIEXPORT int JNICALL Java_com_baofeng_mojing_MojingSDK_GetSensorOriginStatus(JNIEnv *jEnv, jclass)
{
	return MojingSDK_GetSensorOrigin();
}

//当前陀螺仪数据来源
JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSDK_SetSensorOriginStatus(JNIEnv *jEnv, jclass, jint jSensorOrigin)
{
	return MojingSDK_SetSensorOrigin(jSensorOrigin);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetImageYOffset(JNIEnv *jEnv, jclass, jfloat fYOffset)
{
	// USING_MINIDUMP;
	MOJING_FUNC_TRACE(g_APIlogger);
	MOJING_TRACE(g_APIlogger, "Set YOffset = " << fYOffset);
	Manager::GetMojingManager()->GetDistortion()->SetYOffset(fYOffset);
	MojingRenderBase::SetModify();
}

/************************************************************************/
/* 厂商-产品-镜片-APP管理                                               */
/************************************************************************/
JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetManufacturerList(JNIEnv *jEnv, jclass, jstring strLanguageCodeByISO639)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	const char * szLanguageName = jEnv->GetStringUTFChars(strLanguageCodeByISO639, 0);
	String  strRet = MojingSDK_GetManufacturerList(szLanguageName);
	jstring jsRet = jEnv->NewStringUTF(strRet.ToCStr());
	jEnv->ReleaseStringUTFChars(strLanguageCodeByISO639, szLanguageName);
//	MOJING_TRACE(g_APIlogger, "strRet = " << strRet);
	return jsRet;
}
JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetProductList(JNIEnv *jEnv, jclass, jstring strManufacturerKey, jstring strLanguageCodeByISO639)
{
	//MOJING_FUNC_TRACE(g_APIlogger);
	const char * szLanguageName = jEnv->GetStringUTFChars(strLanguageCodeByISO639, 0);
	const char * szKey = jEnv->GetStringUTFChars(strManufacturerKey, 0);
	String  strRet = MojingSDK_GetProductList(szKey, szLanguageName);
	jstring jsRet = jEnv->NewStringUTF(strRet.ToCStr());
	jEnv->ReleaseStringUTFChars(strLanguageCodeByISO639, szLanguageName);
	jEnv->ReleaseStringUTFChars(strManufacturerKey, szKey);
//	MOJING_TRACE(g_APIlogger, "strRet = " << strRet);
	return jsRet;
}
JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetGlassList(JNIEnv *jEnv, jclass, jstring strProductKey, jstring strLanguageCodeByISO639)
{
// 	MOJING_FUNC_TRACE(g_APIlogger);
	const char * szLanguageName = jEnv->GetStringUTFChars(strLanguageCodeByISO639, 0);
	const char * szKey = jEnv->GetStringUTFChars(strProductKey, 0);
	String  strRet = MojingSDK_GetGlassList(szKey, szLanguageName);
	jstring jsRet = jEnv->NewStringUTF(strRet.ToCStr());
	jEnv->ReleaseStringUTFChars(strLanguageCodeByISO639, szLanguageName);
	jEnv->ReleaseStringUTFChars(strProductKey, szKey);

//	MOJING_TRACE(g_APIlogger, "strRet = " << strRet);
	return jsRet;
}

JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetGlassInfo(JNIEnv *jEnv, jclass, jstring strGlassKey, jstring strLanguageCodeByISO639)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	const char * szLanguageName = jEnv->GetStringUTFChars(strLanguageCodeByISO639, 0);
	const char * szKey = jEnv->GetStringUTFChars(strGlassKey, 0);
	String  strRet = MojingSDK_GetGlassInfo(szKey, szLanguageName);
	jstring jsRet = jEnv->NewStringUTF(strRet.ToCStr());
	jEnv->ReleaseStringUTFChars(strLanguageCodeByISO639, szLanguageName);
	jEnv->ReleaseStringUTFChars(strGlassKey, szKey);

//	MOJING_TRACE(g_APIlogger , "strRet = " << strRet);
	return jsRet;
}

JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GenerationGlassKey(JNIEnv *jEnv, jclass, jstring strProductQRCode, jstring strGlassQRCode)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	const char * szProductQRCode = jEnv->GetStringUTFChars(strProductQRCode, 0);
	const char * szGlassQRCode = jEnv->GetStringUTFChars(strGlassQRCode, 0);

	jstring jsRet = jEnv->NewStringUTF(MojingSDK_GenerationGlassKey(szProductQRCode, szGlassQRCode));
	jEnv->ReleaseStringUTFChars(strProductQRCode, szProductQRCode);
	jEnv->ReleaseStringUTFChars(strGlassQRCode, szGlassQRCode);
	return jsRet;
}

JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetEliminateBuiltin(JNIEnv *jEnv, jclass)
{	
	jstring jsRet = jEnv->NewStringUTF(MobileConfigProfile::GetEliminateBuiltin());
	return jsRet;
}
JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_GetInitSDK(JNIEnv *env, jclass)
{
	// USING_MINIDUMP;
	return MojingSDK_GetInitSDK();
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_GetStartTracker(JNIEnv *env, jclass)
{
	// USING_MINIDUMP;
	return MojingSDK_GetStartTracker();
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_GetInMojingWorld(JNIEnv *env, jclass)
{
	// USING_MINIDUMP;
	return MojingSDK_GetInMojingWorld();
}

JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetSDKVersion(JNIEnv *env, jclass)
{
	// USING_MINIDUMP;
	return env->NewStringUTF(MojingSDK_GetSDKVersion());
}

JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetGlasses(JNIEnv *env, jclass)
{
	// USING_MINIDUMP;
	return env->NewStringUTF(MojingSDK_GetGlasses());
}
/*
JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_AddTextureLayout(JNIEnv* env,
	jclass clazz, jint nLeftEyeTextureId, jint nRightEyeTextureId, jint zorder)
{
	return MojingSDK_AddTextureLayout(nLeftEyeTextureId, nRightEyeTextureId, zorder);
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_DeleteTextureLayout(JNIEnv* env,
	jclass clazz, jint zorder)
{
	return MojingSDK_DeleteTextureLayout(zorder);
}
*/

JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_GetCpuName(JNIEnv *env, jclass)
{
	// USING_MINIDUMP;
	return env->NewStringUTF(MojingSDK_GetCpuName());
}

JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_GetGpuName(JNIEnv *env, jclass)
{
	// USING_MINIDUMP;
	return env->NewStringUTF(MojingSDK_GetGpuName());
}

JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_GetScreenPPI(JNIEnv *env, jclass)
{
	Baofeng::Mojing::MojingPlatformBase *pPlatform = Baofeng::Mojing::MojingPlatformBase::GetPlatform();
	Baofeng::Mojing::MojingDisplayParameters * pDisplay = pPlatform->CreateDisplayParameters();
	return pDisplay->GetScreenPPI();
}

// 返回反畸变桶的半径，单位为米
JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_GetDistortionR(JNIEnv *jEnv, jclass, jstring strGlassKey)
{
	const char * szKey = jEnv->GetStringUTFChars(strGlassKey, 0);
	jfloat r = MojingSDK_GetDistortionR(szKey);
	jEnv->ReleaseStringUTFChars(strGlassKey, szKey);
	return r;
}

JNIEXPORT jstring  JNICALL Java_com_baofeng_mojing_MojingSDK_GetJoystickFileName(JNIEnv *jEnv, jclass)
{
	JoystickProfile* pProfile =	Manager::GetMojingManager()->GetParameters()->GetJoystickProfile();
	{// 第一次启动
		MojingPlatformBase *pPlatform = MojingPlatformBase::GetPlatform();
		pProfile->UpdateFromProfile(pPlatform->GetPacketProfilePath());
		pProfile->CheckUpdate();// 启动网络更新
	}
	return jEnv->NewStringUTF(pProfile->GetProfilePathName());
}

jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_DirectionalRadiaInRect(JNIEnv *jEnv, jclass, jfloatArray jm4Direction, jfloatArray jv2TopLeft, jfloatArray jv2BottomRight, jfloat fZ, jfloatArray jv2Point)
{
	jfloat* pMatrixArray = jEnv->GetFloatArrayElements(jm4Direction, NULL);
	jfloat* pTopLeft = jEnv->GetFloatArrayElements(jv2TopLeft, NULL);
	jfloat* pBottomRight = jEnv->GetFloatArrayElements(jv2BottomRight, NULL);
	jfloat* pPoint = jEnv->GetFloatArrayElements(jv2Point, NULL);

	jsize lengthMatrixArray = (jEnv)->GetArrayLength(jm4Direction);
	jsize lengthTopLeft = (jEnv)->GetArrayLength(jv2TopLeft);
	jsize lengthBottomRight = (jEnv)->GetArrayLength(jv2BottomRight);
	jsize lengthPoint = (jEnv)->GetArrayLength(jv2Point);
	jboolean bRet = false;
	if (lengthMatrixArray < 16 ||
		lengthTopLeft < 2 ||
		lengthBottomRight< 2||
		lengthPoint < 2)
	{
		for (jsize i = 0; i < lengthPoint; i++)
		{
			pPoint[i] = -1.0f;
		}
	}
	else
	{
		Matrix4f m4;
		for (int i = 0; i < 16; i++)
		{
			m4.M[i / 4][i % 4] = pMatrixArray[i];
		}
		Vector2f v2TopLeft(pTopLeft[0], pTopLeft[1]);
		Vector2f v2BottomRight(pBottomRight[0], pBottomRight[1]);
		Vector2f v2Ret = MojingSDK_Math_DirectionalRadiaInRect(m4, v2TopLeft, v2BottomRight , fZ);
		pPoint[0] = v2Ret.x;
		pPoint[1] = v2Ret.y;
		bRet = pPoint[0] > 0 && pPoint[1] > 0;
	}

	jEnv->ReleaseFloatArrayElements(jm4Direction, pMatrixArray, 0);
	jEnv->ReleaseFloatArrayElements(jv2TopLeft, pTopLeft, 0);
	jEnv->ReleaseFloatArrayElements(jv2BottomRight, pBottomRight, 0);
	jEnv->ReleaseFloatArrayElements(jv2Point, pPoint, 0);

	return bRet;
}

jint JNICALL Java_com_baofeng_mojing_MojingSDK_SelectRectByDirectional(JNIEnv *jEnv, jclass, jfloatArray jm4Direction, jfloatArray jv3TopLeft, jfloatArray jv3BottomRight)
{
	jint iRet = -1;
	jfloat* pMatrixArray = jEnv->GetFloatArrayElements(jm4Direction, NULL);
	jfloat* pTopLeft = jEnv->GetFloatArrayElements(jv3TopLeft, NULL);
	jfloat* pBottomRight = jEnv->GetFloatArrayElements(jv3BottomRight, NULL);
	jsize lengthMatrixArray = (jEnv)->GetArrayLength(jm4Direction);
	jsize lengthTopLeft = (jEnv)->GetArrayLength(jv3TopLeft);
	jsize lengthBottomRight = (jEnv)->GetArrayLength(jv3BottomRight);

	if (lengthMatrixArray == 16 &&
		lengthTopLeft == lengthBottomRight &&
		0 == (lengthTopLeft % 3))
	{
		Matrix4f m4;
		for (int i = 0; i < 16; i++)
		{
			m4.M[i / 4][i % 4] = pMatrixArray[i];
		}
		int iRectCounts = lengthTopLeft / 3;
		iRet = MojingSDK_Math_SelectRectByDirectional(m4,iRectCounts , pTopLeft ,pBottomRight);
	}
	jEnv->ReleaseFloatArrayElements(jm4Direction, pMatrixArray, 0);
	jEnv->ReleaseFloatArrayElements(jv3TopLeft, pTopLeft, 0);
	jEnv->ReleaseFloatArrayElements(jv3BottomRight, pBottomRight, 0);
	
	return iRet;
}

/************************************************************************/
/* 以下是为跑分软件VrMark特制的接口                                     */
/************************************************************************/
JNIEXPORT int JNICALL Java_com_baofeng_mojing_VrMark_StartTrackerChecker(JNIEnv *env, jclass, jint nSampleFrequence)
{
	return MojingSDK_StartTrackerChecker(nSampleFrequence);
}
// 返回[0...500]，500表示完成
JNIEXPORT int JNICALL Java_com_baofeng_mojing_VrMark_GetTrackerCheckerResult(JNIEnv *jEnv, jclass, jfloatArray Samples, jfloatArray Parmats)
{
	jfloat* pJSamples = jEnv->GetFloatArrayElements(Samples, NULL);
	jfloat* pJParmats = jEnv->GetFloatArrayElements(Parmats, NULL);
	
	jsize lR = (jEnv)->GetArrayLength(Samples);
	jsize lG = (jEnv)->GetArrayLength(Parmats);
	
	__tagSampleCheckeResult SampleCheckeResult;
	int iRet = MojingSDK_GetTrackerCheckerResult(&SampleCheckeResult);
	memcpy(pJSamples , SampleCheckeResult.m_fSampleOffset , 500 * sizeof(float));
	memcpy(pJParmats, &SampleCheckeResult.m_fMaxContiguousOffset, sizeof(__tagSampleCheckeResult)-500 * sizeof(float) - sizeof(int));

	jEnv->ReleaseFloatArrayElements(Samples, pJSamples, 0);
	jEnv->ReleaseFloatArrayElements(Parmats, pJParmats, 0);
	return iRet;
}

JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSDK_NativektxLoadTextureN(JNIEnv * env, jclass, jstring jsPath, jobject ktxInfo)
{
	GLuint id = 0;
	GLint w = 0;
	GLint h = 0;
	GLint d = 0;
	GLint glError = 0;
	GLuint target = 0;
	GLenum glerror = 0;
	bool isMipmapped = false;
	GLint ektx = 0;

	jclass cls = env->GetObjectClass(ktxInfo);

	jfieldID fid = env->GetFieldID(cls, "iTextureID", "I");
	id = env->GetIntField(ktxInfo, fid);

	const char * szPath = env->GetStringUTFChars(jsPath, 0);
	bool bRet = MojingSDK_ktxLoadTextureN(szPath, &id, &target, &h, &w, &d, &isMipmapped, &glerror, &ektx);
	env->ReleaseStringUTFChars(jsPath, szPath);

	if (bRet)
	{
		env->SetIntField(ktxInfo, fid, id);

		fid = env->GetFieldID(cls, "iTarget", "I");
		env->SetIntField(ktxInfo, fid, target);

		fid = env->GetFieldID(cls, "iHeight", "I");
		env->SetIntField(ktxInfo, fid, h);

		fid = env->GetFieldID(cls, "iWidth", "I");
		env->SetIntField(ktxInfo, fid, w);

		fid = env->GetFieldID(cls, "iDepth", "I");
		env->SetIntField(ktxInfo, fid, d);

		fid = env->GetFieldID(cls, "iGLError", "I");
		env->SetIntField(ktxInfo, fid, glerror);

		fid = env->GetFieldID(cls, "iKTXError", "I");
		env->SetIntField(ktxInfo, fid, ektx);

		fid = env->GetFieldID(cls, "bIsMipmapped", "Z");
		env->SetBooleanField(ktxInfo, fid, isMipmapped);
	}

	return bRet;
}


/************************************************************************/
/* 以上是为跑分软件VrMark特制的接口                                     */
/************************************************************************/

// 获取光感数据
JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_IsLowPower(JNIEnv *, jclass)
{
	return MojingSDK_IsLowPower();
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetHDMWorking(JNIEnv *, jclass, jboolean bHDMWorking)
{
	MojingSDK_SetHDMWorking(bHDMWorking);
}

JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetGlassesSN(JNIEnv *env, jclass, jstring jstrGlassesSN)
{
	const char * szGlassesSN = env->GetStringUTFChars(jstrGlassesSN, 0);
	MojingSDK_SetGlassesSerialNumber(szGlassesSN);
	env->ReleaseStringUTFChars(jstrGlassesSN, szGlassesSN);
}

JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_GetSocketPort(JNIEnv *, jclass)
{
	return MojingSDK_GetSocketPort();
}

JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_DeviceGetKeymask(JNIEnv *env, jclass, jint iID, jintArray KeyMask)
{
	jint* pKeyMask = env->GetIntArrayElements(KeyMask, NULL);

	jint iRet = MojingSDK_Device_GetKeymask(iID, pKeyMask);
	env->ReleaseIntArrayElements(KeyMask, pKeyMask, 0);

	return iRet;
}

JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_DeviceGetInfo(JNIEnv *env, jclass, jint iID/*设备ID*/,
	jfloatArray QuartArray/*四元数表示的旋转，依次为XYZW*/,
	jfloatArray AngularAccelArray/*角加速度，依次为XYZ*/,
	jfloatArray LinearAccelArray/*线加速度，依次为XYZ*/,
	jfloatArray PositionArray,/*设备的空间位置，以米为单位，默认是0,0,0。*/
	jintArray KeystatusArray/*设备上的按键状态，默认是0表示没有按键被按下*/)
{	
	jfloat* pQuart = NULL;
	if (QuartArray != nullptr)
	{
		pQuart = env->GetFloatArrayElements(QuartArray, NULL);
	};
	jfloat* pAngularAccel = NULL;
	if (AngularAccelArray != nullptr)
	{
		pAngularAccel = env->GetFloatArrayElements(AngularAccelArray, NULL);
	};
	jfloat* pLinearAccel = NULL;
	if (LinearAccelArray != nullptr)
	{
		pLinearAccel = env->GetFloatArrayElements(LinearAccelArray, NULL);
	};
	jfloat* pPosition = NULL;
	if (PositionArray != nullptr)
	{
		pPosition = env->GetFloatArrayElements(PositionArray, NULL);
	};
	jint* pKeystatus = NULL;
	if (KeystatusArray != nullptr)
	{
		pKeystatus = env->GetIntArrayElements(KeystatusArray, NULL);
	};

	jfloat fRet = MojingSDK_Device_GetCurrentPoaseInfo(iID, pQuart, pAngularAccel, pLinearAccel, pPosition, (unsigned int* )pKeystatus);

	if (pQuart)
		env->ReleaseFloatArrayElements(QuartArray, pQuart, 0);
	if (pAngularAccel)
		env->ReleaseFloatArrayElements(AngularAccelArray, pAngularAccel, 0);
	if (pLinearAccel)
		env->ReleaseFloatArrayElements(LinearAccelArray, pLinearAccel, 0);
	if (pPosition)
		env->ReleaseFloatArrayElements(PositionArray, pPosition, 0);
	if (pKeystatus)
		env->ReleaseIntArrayElements(KeystatusArray, pKeystatus, 0);

	return fRet;
}

JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_DeviceGetFixInfo(JNIEnv *env, jclass, jint iID/*设备ID*/,
	jfloatArray QuartArray/*四元数表示的旋转，依次为XYZW*/,
	jfloatArray AngularAccelArray/*角加速度，依次为XYZ*/,
	jfloatArray LinearAccelArray/*线加速度，依次为XYZ*/,
	jfloatArray PositionArray/*设备的空间位置，以米为单位，默认是0,0,0。*/ )
{
	jfloat* pQuart = NULL;
	if (QuartArray != nullptr)
	{
		pQuart = env->GetFloatArrayElements(QuartArray, NULL);
	};
	jfloat* pAngularAccel = NULL;
	if (AngularAccelArray != nullptr)
	{
		pAngularAccel = env->GetFloatArrayElements(AngularAccelArray, NULL);
	};
	jfloat* pLinearAccel = NULL;
	if (LinearAccelArray != nullptr)
	{
		pLinearAccel = env->GetFloatArrayElements(LinearAccelArray, NULL);
	};
	jfloat* pPosition = NULL;
	if (PositionArray != nullptr)
	{
		pPosition = env->GetFloatArrayElements(PositionArray, NULL);
	};

	jfloat fRet = MojingSDK_Device_GetFixPoaseInfo(iID, pQuart, pAngularAccel, pLinearAccel, pPosition);

	if (pQuart)
		env->ReleaseFloatArrayElements(QuartArray, pQuart, 0);
	if (pAngularAccel)
		env->ReleaseFloatArrayElements(AngularAccelArray, pAngularAccel, 0);
	if (pLinearAccel)
		env->ReleaseFloatArrayElements(LinearAccelArray, pLinearAccel, 0);
	if (pPosition)
		env->ReleaseFloatArrayElements(PositionArray, pPosition, 0);

	return fRet;
}


JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_DeviceGetControlFixCurrentInfo(JNIEnv *env, jclass, jint iID/*设备ID*/,
    jfloatArray QuartArray/*四元数表示的旋转，依次为XYZW*/,
    jfloatArray AngularAccelArray/*角加速度，依次为XYZ*/,
    jfloatArray LinearAccelArray/*线加速度，依次为XYZ*/,
    jfloatArray PositionArray,/*设备的空间位置，以米为单位，默认是0,0,0。*/
    jintArray KeystatusArray/*设备上的按键状态，默认是0表示没有按键被按下*/
    )
{
	jfloat* pQuart = NULL;
	if (QuartArray != nullptr)
	{
		pQuart = env->GetFloatArrayElements(QuartArray, NULL);
	};
	jfloat* pAngularAccel = NULL;
	if (AngularAccelArray != nullptr)
	{
		pAngularAccel = env->GetFloatArrayElements(AngularAccelArray, NULL);
	};
	jfloat* pLinearAccel = NULL;
	if (LinearAccelArray != nullptr)
	{
		pLinearAccel = env->GetFloatArrayElements(LinearAccelArray, NULL);
	};
	jfloat* pPosition = NULL;
	if (PositionArray != nullptr)
	{
		pPosition = env->GetFloatArrayElements(PositionArray, NULL);
	};
	jint* pKeystatus = NULL;
	if (KeystatusArray != nullptr)
	{
		pKeystatus = env->GetIntArrayElements(KeystatusArray, NULL);
	};

    jfloat fRet = MojingSDK_Device_GetControlFixCurrentInfo(iID, pQuart, pAngularAccel, pLinearAccel, pPosition, (unsigned int*)pKeystatus);

	if (pQuart)
		env->ReleaseFloatArrayElements(QuartArray, pQuart, 0);
	if (pAngularAccel)
		env->ReleaseFloatArrayElements(AngularAccelArray, pAngularAccel, 0);
	if (pLinearAccel)
		env->ReleaseFloatArrayElements(LinearAccelArray, pLinearAccel, 0);
	if (pPosition)
		env->ReleaseFloatArrayElements(PositionArray, pPosition, 0);
	if (pKeystatus)
		env->ReleaseIntArrayElements(KeystatusArray, pKeystatus, 0);

    return fRet;
}
#ifdef _DEBUG
/************************************************************************/
/* 以下为私有代码，给SDK用的代码放到前面去			                    */
/************************************************************************/

// 设置和获取瞳距，以米为单位
JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionSetLensSeparation(JNIEnv *, jclass, jfloat fNewValue)
{
	return MojingSDK_DistortionSetLensSeparation(fNewValue);
}
JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionGetLensSeparation(JNIEnv *, jclass)
{
	
	return MojingSDK_DistortionGetLensSeparation();
}
// 设置和获取K值，Get函数的返回值为畸变引导点的数量
JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionSetParamet(JNIEnv *jEnv, jclass, jint iSegment, jfloatArray jKR, jfloatArray jKG, jfloatArray jKB)
{

	jfloat* pKR  = jEnv->GetFloatArrayElements(jKR, NULL);
	jfloat* pKG  = jEnv->GetFloatArrayElements(jKG, NULL);
	jfloat* pKB  = jEnv->GetFloatArrayElements(jKB, NULL);
	jsize lR = (jEnv)->GetArrayLength(jKR);
	jsize lG = (jEnv)->GetArrayLength(jKG);
	jsize lB = (jEnv)->GetArrayLength(jKB);
	bool bRet = MojingSDKPrivate_DistortionSetParamet(iSegment, pKR, pKG, pKB);
	jEnv->ReleaseFloatArrayElements(jKR, pKR, 0);
	jEnv->ReleaseFloatArrayElements(jKG, pKG, 0);
	jEnv->ReleaseFloatArrayElements(jKB, pKB, 0);

	return bRet ;
}

JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionGetParamet(JNIEnv *jEnv, jclass, jfloatArray jKR, jfloatArray jKG, jfloatArray jKB)
{
	jfloat* pKR  = jEnv->GetFloatArrayElements(jKR, NULL);
	jfloat* pKG  = jEnv->GetFloatArrayElements(jKG, NULL);
	jfloat* pKB  = jEnv->GetFloatArrayElements(jKB, NULL);
	
	jsize lR = (jEnv)->GetArrayLength(jKR);
	jsize lG = (jEnv)->GetArrayLength(jKG);
	jsize lB = (jEnv)->GetArrayLength(jKB);


	int iRet = MojingSDKPrivate_DistortionGetParamet(pKR, pKG, pKB);
	jEnv->ReleaseFloatArrayElements(jKR, pKR, 0);
	jEnv->ReleaseFloatArrayElements(jKG, pKG, 0);
	jEnv->ReleaseFloatArrayElements(jKB, pKB, 0);

	return iRet;
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionSaveParamet(JNIEnv *, jclass)
{
	return MojingSDKPrivate_DistortionSaveParamet();
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionResetParamet(JNIEnv *, jclass)
{
	return MojingSDKPrivate_DistortionResetParamet();
}

#include "Reporter/CrashReporter.h"
JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_FuncTest(JNIEnv *, jclass)
{
	// USING_MINIDUMP;

	MOJING_FUNC_TRACE(g_APIlogger);

	Manager* pManager = Manager::GetMojingManager();
	if (pManager)
	{
		//		MojingFactoryCalibrationParameters A;
		//		A.Save();
		//		pManager->GetParameters()->GetGyroOffsetReportSet()->Save();
		AES A((unsigned char*)"1234567980abcdef");
		char S1[] = "1234567980abcdef";
		char S2[] = "1234567980abcdef";
		A.Cipher(S1 , 16);
		A.Cipher(S2, 16);
		int iV = memcmp(S1 ,S2 , 16);
		//String szUploadFile = "/sdcard/MojingSDK/log/MojingSDK_API.log";
		//CrashReporter *pReport = CrashReporter::GetCrashReporter(szUploadFile);
		//pReport->DoReport();
	}
}

/************************************************************************/
/* 以上为私有代码，给SDK用的代码放到前面去			                    */
/************************************************************************/
#endif
