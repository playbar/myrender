#pragma once

#include <jni.h>
#include "Base/MojingTypes.h"
#ifdef LOG4CPLUS_IMPORT
#include "3rdPart/log4cplus/LogInterface.h"
#else
#include "LogTraker/LogInterface.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
	JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved);
	
	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_GetSystemIntProperty(JNIEnv *jEnv, jclass, jstring sProperty, jint defaultValue);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_Log(JNIEnv *jEnv, jclass, jint logLevel, jstring sInfo, jstring sFileName, jint line);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_VrFuncInit(JNIEnv *jEnv, jclass, jboolean isdd);
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_ReprojFuncInit(JNIEnv *jEnv, jclass);
    JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeUnityFuncInit(JNIEnv *jEnv, jclass);
	/*
	* Class:     com_baofeng_mojing_MojingSDK
	* Method:    Init
	* Signature: ()Z
	*/
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_Init(JNIEnv *env, jclass, jstring merchantID, jstring appID, jstring appKey, jstring appName, jstring packageName,
		jstring userID, jstring channelID, jint nWidth, jint nHeight, jfloat xdpi, jfloat ydpi, jstring ProfilePath);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_CheckPackage(JNIEnv *env, jclass, jstring jstrAppName, jstring jstrCaseCode);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetEngineVersion(JNIEnv *env, jclass, jstring jstrEngineVersion);
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_IsUseUnityForSVR(JNIEnv *env, jclass);
    JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_IsUseForDayDream(JNIEnv *env, jclass);
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_IsInMachine(JNIEnv *env, jclass);

	/*
	* Class:     com_baofeng_mojing_App
	* Method:    Init/Exit/AppResume/AppPause/AppPageStart/AppPageEnd/AppSetEvent
	* Signature: ()Z
	*/
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_ReportReInit(JNIEnv *env, jclass);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppExit(JNIEnv *env, jclass);

	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_AppGetRunID(JNIEnv *env, jclass);

	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_AppResume(JNIEnv *env, jclass, jstring uniqueID);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppPause(JNIEnv *env, jclass);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppPageStart(JNIEnv *env, jclass, jstring pageName);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppPageEnd(JNIEnv *env, jclass, jstring pageName);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppSetEvent(JNIEnv *env, jclass, jstring eventName, jstring eChannelID, jstring eInName, jfloat eInData, jstring eOutName, jfloat eOutData);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppReportLog(JNIEnv *env, jclass, jint iLogType, jstring typeName, jstring logContent);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppReportUserAction(JNIEnv *env, jclass, jstring strActionType, jstring strItemID, jstring strJsonValue);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppSetContinueInterval(JNIEnv *env, jclass, jint interval);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppSetReportInterval(JNIEnv *env, jclass, jint interval);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_AppSetReportImmediate(JNIEnv *env, jclass, jboolean bImmdediate);

	/*
	* Class:     com_baofeng_mojing_MojingSDK
	* Method:    StartTracker
	* Signature: (ILjava/lang/String;)Z
	*/
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_StartTracker(JNIEnv *, jclass, jint nSampleFrequence);
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_StartGlassTracker(JNIEnv *, jclass, jstring glassName);

	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_CheckSensors(JNIEnv *, jclass);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_ResetSensorOrientation(JNIEnv *jEnv, jclass);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_ResetTracker(JNIEnv *jEnv, jclass);

	JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_IsTrackerCalibrated(JNIEnv *jEnv, jclass);
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_StartTrackerCalibration(JNIEnv *jEnv, jclass);
	/*
	* Class:     com_baofeng_mojing_MojingSDK
	* Method:    getLastHeadView
	* Signature: ([F)V
	*/
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_getLastHeadView(JNIEnv *, jclass, jfloatArray jViewMatrix);
	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_getPredictionHeadView(JNIEnv *jEnv, jclass, jfloatArray jViewMatrix, jdouble time);
	JNIEXPORT jdouble JNICALL Java_com_baofeng_mojing_MojingSDK_getLastSensorState(JNIEnv *jEnv, jclass, jfloatArray jArray);
	/*
	* Class:     com_baofeng_mojing_MojingSDK
	* Method:    getLastHeadEulerAngles
	* Signature: ([F)V
	*/	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_getLastHeadEulerAngles(JNIEnv *, jclass, jfloatArray jEulerAngles);

	/*
	* Class:     com_baofeng_mojing_MojingSDK
	* Method:    getLastHeadUnitQuarternion
	* Signature: ([F)V
	*/
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_getLastHeadQuarternion(JNIEnv *, jclass, jfloatArray jUnitQuarternion);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_getDirectionalInScreen(JNIEnv *, jclass, jfloatArray jArray);
	/*
	* Class:     com_baofeng_mojing_MojingSDK
	* Method:    StopTracker
	* Signature: ()V
	*/
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_StopTracker(JNIEnv *, jclass);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SendSensorData(JNIEnv *jEnv, jclass, jfloatArray jSensortMatrix, jdouble dLastSampleTime);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SendControllerData(JNIEnv *jEnv, jclass, jbyteArray jControllerData, jint dataLen);
	
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SendControllerDataV2(JNIEnv *jEnv, jclass, jfloatArray jControllerData, jdouble jTimestamp, jboolean bRecenter);	

	// 进入魔镜世界
	JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSurfaceView_EnterMojingWorld(JNIEnv *env, jclass, jstring GlassesName, jboolean  bEnableMultiThread, jboolean  bEnableTimeWarp);
	// 改变魔镜世界
	JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSurfaceView_ChangeMojingWorld(JNIEnv *env, jclass, jstring GlassesName);
	// 离开魔镜世界
	JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSurfaceView_LeaveMojingWorld(JNIEnv *, jclass);
	// 设置默认魔镜世界
	JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSDK_SetDefaultMojingWorld(JNIEnv *jEnv, jclass, jstring strGlassesKey);
	// 取得默认魔镜世界信息
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetDefaultMojingWorld(JNIEnv *jEnv, jclass, jstring strLanguageCodeByISO639);
	// 取得最后一次进入的魔镜世界信息
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetLastMojingWorld(JNIEnv *jEnv, jclass, jstring strLanguageCodeByISO639);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetEnableTimeWarp(JNIEnv *jEnv, jclass, jboolean bEnable);

	// 获取魔镜世界的视角
	JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_GetMojingWorldFOV(JNIEnv *, jclass);
	// 魔镜世界场景变化
	JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSurfaceView_OnSurfaceChanged(JNIEnv *, jclass, jint newWidth, jint newHeight);
	// 绘制魔镜世界
	JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSDK_DrawTexture(JNIEnv *, jclass, jint, jint);
	
	JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSDK_DrawTextureWithOverlay(JNIEnv *, jclass, jint, jint, jint, jint);// 3D Overlay
//	UNITY_EXPORT void UNITY_DrawTexture(int LeftTexID, int RightTexID);
	// 生成一个纹理对象用于绘制单眼/双眼的图像
	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_GetEyeTexture(JNIEnv *, jclass, jint eyeTextureType, jintArray outParams);
	// 获取眼镜的中心距离映射到手机屏幕上的物理尺寸
	JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_GetGlassesSeparationInPix(JNIEnv *, jclass);
	// 增加一条参考线用来定位屏幕中央位置。必须在EnterMojingWorld之后调用
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetCenterLine(JNIEnv *, jclass,jint iWidth , jint colR, jint colG, jint colB, jint colA);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetImageYOffset(JNIEnv *jEnv, jclass, jfloat fYOffset);
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetOverlayPosition(JNIEnv *jEnv, jclass, jfloat fLeft, jfloat fTop, jfloat fWidth, jfloat fHeight);
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetOverlayPosition3D(JNIEnv *jEnv, jclass, jint eyeTextureType, /*jfloat fLeft, jfloat fTop, */jfloat fWidth, jfloat fHeight, jfloat fDistanceInMetre);
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetOverlayPosition3DV2(JNIEnv *jEnv, jclass, jint eyeTextureType, jfloat fLeft, jfloat fTop, jfloat fWidth, jfloat fHeight, jfloat fDistanceInMetre);
	// 获取当前用户设置
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetUserSettings(JNIEnv *jEnv, jclass);
	// 修改当前用户设置
	JNIEXPORT bool JNICALL  Java_com_baofeng_mojing_MojingSDK_SetUserSettings(JNIEnv *jEnv, jclass, jstring strUserSettings);

	// 获取当前陀螺仪数据来源
	JNIEXPORT int JNICALL Java_com_baofeng_mojing_MojingSDK_GetSensorOriginStatus(JNIEnv *jEnv, jclass);
	//当前陀螺仪数据来源
	JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSDK_SetSensorOriginStatus(JNIEnv *jEnv, jclass, jint jSensorOrigin);

	/************************************************************************/
	/* 厂商-产品-镜片-APP管理                                               */
	/************************************************************************/
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetManufacturerList(JNIEnv *jEnv, jclass, jstring strLanguageCodeByISO639);
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetProductList(JNIEnv *jEnv, jclass, jstring strManufacturerKey, jstring strLanguageCodeByISO639);
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetGlassList(JNIEnv *jEnv, jclass, jstring strProductKey, jstring strLanguageCodeByISO639);
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetGlassInfo(JNIEnv *jEnv, jclass, jstring strGlassKey, jstring strLanguageCodeByISO639);
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GenerationGlassKey(JNIEnv *jEnv, jclass, jstring strProductQRCode, jstring strGlassQRCode);
	// 获取需要排除掉的手机内部数据列表
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetEliminateBuiltin(JNIEnv *jEnv, jclass);

	JNIEXPORT
	// 取得状态函数
	// 获取SDK版本号
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetSDKVersion(JNIEnv *env, jclass);
	// 获取SDK 是否已经启动
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_GetInitSDK(JNIEnv *env, jclass);
	// 获取陀螺仪传感器 是否已经启动
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_GetStartTracker(JNIEnv *env, jclass);
	// 获取魔镜畸变适配的眼睛版本(是否已进入魔镜世界)
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDK_GetGlasses(JNIEnv *env, jclass);
	// 获取魔镜畸变适配的眼睛版本(是否已进入魔镜世界)
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_GetInMojingWorld(JNIEnv *env, jclass);
// 	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_AddTextureLayout(JNIEnv* env, 
// 		jclass clazz, jint nLeftEyeTextureId, jint nRightEyeTextureId, jint zorder);
// 	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_DeleteTextureLayout(JNIEnv* env,
// 		jclass clazz, jint zorder);

	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_GetCpuName(JNIEnv *env, jclass);
	JNIEXPORT jstring JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_GetGpuName(JNIEnv *env, jclass);

	JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_GetScreenPPI(JNIEnv *env, jclass);
	JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_GetDistortionR(JNIEnv *jEnv, jclass, jstring strGlassKey);
	// 获取最新的手柄配置文件（也会根据需要去检查云端更新）
	JNIEXPORT jstring  JNICALL Java_com_baofeng_mojing_MojingSDK_GetJoystickFileName(JNIEnv *jEnv, jclass);

	/************************************************************************/
	/* 以下为辅助函数                                                       */
	/************************************************************************/
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_DirectionalRadiaInRect(JNIEnv *jEnv, jclass, jfloatArray jDirection, jfloatArray v2TopLeft, jfloatArray v2BottomRight, jfloat fZ, jfloatArray jPoint);
	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_SelectRectByDirectional(JNIEnv *jEnv, jclass, jfloatArray jm4Direction, jfloatArray jv3TopLeft, jfloatArray jv3BottomRight);
	/************************************************************************/
	/* 以下是为跑分软件VrMark特制的接口                                     */
	/************************************************************************/
	JNIEXPORT int JNICALL Java_com_baofeng_mojing_VrMark_StartTrackerChecker(JNIEnv *env, jclass, jint nSampleFrequence);
	// 返回[0...500]，500表示完成
	JNIEXPORT int JNICALL Java_com_baofeng_mojing_VrMark_GetTrackerCheckerResult(JNIEnv *, jclass, jfloatArray Samples, jfloatArray Parmats);

	JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingSDK_NativektxLoadTextureN(JNIEnv * env, jclass, jstring jsPath, jobject ktxInfo);

	/************************************************************************/
	/* 以上是为跑分软件VrMark特制的接口                                     */
	/************************************************************************/

	// 获取光感数据
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDK_IsLowPower(JNIEnv *, jclass);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetHDMWorking(JNIEnv *, jclass, jboolean bHDMWorking);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_SetGlassesSN(JNIEnv *env, jclass, jstring jstrGlassesSN);

	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_GetSocketPort(JNIEnv *, jclass);

	// 体感手柄接口
	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDK_DeviceGetKeymask(JNIEnv *env, jclass, jint iID, jintArray KeyMask);
	JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_DeviceGetInfo(JNIEnv *env, jclass, jint iID/*设备ID*/,
		jfloatArray QuartArray/*四元数表示的旋转，依次为XYZW*/,
		jfloatArray AngularAccelArray/*角加速度，依次为XYZ*/,
		jfloatArray LinearAccelArray/*线加速度，依次为XYZ*/,
		jfloatArray PositionArray,/*设备的空间位置，以米为单位，默认是0,0,0。*/
		jintArray KeystatusArray/*设备上的按键状态，默认是0表示没有按键被按下*/);
	JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_DeviceGetFixInfo(JNIEnv *env, jclass, jint iID/*设备ID*/,
		jfloatArray QuartArray/*四元数表示的旋转，依次为XYZW*/,
		jfloatArray AngularAccelArray/*角加速度，依次为XYZ*/,
		jfloatArray LinearAccelArray/*线加速度，依次为XYZ*/,
		jfloatArray PositionArray/*设备的空间位置，以米为单位，默认是0,0,0。*/);

    JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDK_DeviceGetControlFixCurrentInfo(JNIEnv *env, jclass, jint iID/*设备ID*/,
        jfloatArray QuartArray/*四元数表示的旋转，依次为XYZW*/,
        jfloatArray AngularAccelArray/*角加速度，依次为XYZ*/,
        jfloatArray LinearAccelArray/*线加速度，依次为XYZ*/,
        jfloatArray PositionArray,/*设备的空间位置，以米为单位，默认是0,0,0。*/
        jintArray KeystatusArray/*设备上的按键状态，默认是0表示没有按键被按下*/);

	/************************************************************************/
	/* 以下是SDK私有接口，禁止发布于MojingSDK                               */
	/************************************************************************/
#ifdef _DEBUG
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_FuncTest(JNIEnv *, jclass);
	// 设置和获取瞳距，以米为单位
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionSetLensSeparation(JNIEnv *, jclass, jfloat);
	JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionGetLensSeparation(JNIEnv *, jclass);
	// 设置和获取K值，Get函数的返回值为畸变引导点的数量
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionSetParamet(JNIEnv *, jclass, jint, jfloatArray jKR, jfloatArray jKG, jfloatArray jKB);
	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionGetParamet(JNIEnv *, jclass, jfloatArray jKR, jfloatArray jKG, jfloatArray jKB);

	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionSaveParamet(JNIEnv *, jclass);
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionResetParamet(JNIEnv *, jclass);
#else
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_FuncTest(JNIEnv *, jclass){return;};
	// 设置和获取瞳距，以米为单位
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionSetLensSeparation(JNIEnv *, jclass, jfloat){return false;};
	JNIEXPORT jfloat JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionGetLensSeparation(JNIEnv *, jclass){return 0;};
	// 设置和获取K值，Get函数的返回值为畸变引导点的数量
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionSetParamet(JNIEnv *, jclass, jint, jfloatArray jKR, jfloatArray jKG, jfloatArray jKB) {return 0;};
	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionGetParamet(JNIEnv *, jclass, jfloatArray jKR, jfloatArray jKG, jfloatArray jKB) {return 0;};

	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionSaveParamet(JNIEnv *, jclass) {return false;};
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingSDKPrivate_DistortionResetParamet(JNIEnv *, jclass) {return false;};
#endif


	

#ifdef __cplusplus
}
#endif


