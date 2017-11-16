//
// Created by mac on 16/8/19.
//

#ifndef TRUNK_VIEWCORE_H
#define TRUNK_VIEWCORE_H

#include <jni.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "hell-libs::", __VA_ARGS__))

extern JavaVM *gs_jvm;

JNIEnv* AttachCurrentThreadJNI();
void DetachCurrentThreadJNI();


#endif //TRUNK_VIEWCORE_H
