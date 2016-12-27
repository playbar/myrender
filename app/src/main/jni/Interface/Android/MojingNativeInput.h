#pragma once
#include <jni.h>
#include "../../Base/MojingTypes.h"
#ifdef MJ_OS_ANDROID

#ifdef __cplusplus
extern "C" {
#endif
	// java端通知有多少个Mojing2设备，只有在Mojing2只有一个情况下下会通知。
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeSetMojing2Number(JNIEnv *env, jclass, jint iNumber);
	// 清空所有的已连接设备信息
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeCleanDeviceMap(JNIEnv *, jclass);
	// 开始更新设备前调用
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeBeginUpdateDeviceMap(JNIEnv *, jclass);
	// 结束更新设备后调用
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeEndUpdateDeviceMap(JNIEnv *, jclass);
	// 更新一个已连接的设备信息
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_NativeAddDeviceToMap(JNIEnv *, jclass, jint jiDeviceID, jstring jsDeviceName);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_OnKeyEvent(JNIEnv *env, jclass, jstring jsDeviceName, jint jiButtonId, jboolean jbButtonDown);
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingSDK_OnAxisEvent(JNIEnv *env, jclass, jstring jsDeviceName, jint jiAxisID, jfloat jfValue);

#ifdef __cplusplus
}
#endif

#endif // MJ_OS_ANDROID