//
// Created by liujiankang on 2016/11/18.
//

#ifndef TRUNK_MODEL3D_H
#define TRUNK_MODEL3D_H

#include <jni.h>

extern JavaVM *gs_JVM;

JNIEnv* AttachCurrentThreadJNI();
void DetachCurrentThreadJNI();

#endif //TRUNK_MODEL3D_H
