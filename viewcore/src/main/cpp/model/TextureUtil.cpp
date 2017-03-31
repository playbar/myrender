//
// Created by liujiankang on 2016/11/14.
//

#include <jni.h>
#include <myLogger.h>
#include "TextureUtil.h"

extern JavaVM *gs_JVM;
static bool  g_bAttatedT = false;
static jclass g_textureUtil;

JNIEnv *GetEnv()
{
    int status;
    JNIEnv *envnow = nullptr;
    status = gs_JVM->GetEnv((void **)&envnow, JNI_VERSION_1_6);
    if(status < 0)
    {
        status = gs_JVM->AttachCurrentThread(&envnow, nullptr);
        if(status < 0)
        {
            return nullptr;
        }
        g_bAttatedT = true;
    }
    return envnow;
}

int LoadImage2Texture(const char* filePath) {
    if(filePath == nullptr) {
        return 0;
    }
    JNIEnv *env = GetEnv();
    int textureId = 0;
    if(env != nullptr)
    {
        MyLOGI("TextureUtil : [1]");
        jstring _filePath = env->NewStringUTF(filePath);
        if( g_textureUtil == nullptr) {
            g_textureUtil = env->FindClass("baofeng/mojing/model3d/TextureUtil");
        }
        jmethodID mid = env->GetStaticMethodID( g_textureUtil,"CreateTextureFromPic", "(Ljava/lang/String;)I");
        textureId = env->CallStaticIntMethod( g_textureUtil, mid, _filePath);
        env->DeleteLocalRef ( _filePath);
        MyLOGI("TextureUtil Load OK : [2]");
    }
    MyLOGI("TextureUtil textureID : %d [3]", textureId);
    return textureId;
}

void UnloadImage(int textureId) {
    if( textureId <= 0) {
        return;
    }
    JNIEnv *env = GetEnv();
    if(env != nullptr) {
        if( g_textureUtil == nullptr) {
            g_textureUtil = env->FindClass("baofeng.mojing.model3d.TextureUtil");
        }
        jmethodID mid = env->GetStaticMethodID( g_textureUtil,"DeleteTextureId", "(I)V");
        env->CallStaticVoidMethod(g_textureUtil, mid, textureId);
    }
}
