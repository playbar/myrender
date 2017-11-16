//
// Created by mac on 16/8/19.
//

#ifndef TRUNK_GENTEXTASK_H
#define TRUNK_GENTEXTASK_H

#include "ThreadPool.h"
#include "GLThreadUtilJni.h"
#include "viewcore.h"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLGenTexTask_NativeInit(JNIEnv* env, jobject thiz);
JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLGenTexTask_NativeUninit(JNIEnv* env, jobject thiz);
JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLGenTexTask_NativeQueueEvent(JNIEnv* env, jobject thiz);
JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLGenTexTask_NativeGenTexId(JNIEnv* env,
                jobject thiz, jobject bmp, jint width, jint height);

#ifdef __cplusplus
}
#endif

class GenTexTask: public CTask
{
public:
    GenTexTask(JNIEnv* env, jobject thiz);
    GenTexTask();
    ~GenTexTask();
    void SetJObject( jobject thiz);
    void QueueEvent();
    void GenTexID( jobject bmp, int width, int height );
    int Run();

public:
    GLuint CreateSimpleTexture2D( );
    GLuint CreateTexture2D( );

public:
    static jclass mThizClass;
    static jfieldID mClassID;
    static jmethodID mExportTextureId;

private:
    jobject mThiz;

};


#endif //TRUNK_GENTEXTASK_H
