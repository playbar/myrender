//
// Created by mac on 16/8/19.
//

#ifndef TRUNK_GLTHREADUTILJNI_H
#define TRUNK_GLTHREADUTILJNI_H
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "viewcore.h"
#include "ThreadPool.h"

extern EGLContext gShareContext;
extern EGLDisplay gDisplay;
extern EGLSurface gAuxSurface;
extern CThreadPool gThreadPool;


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLThreadUtil_onSurfaceCreated(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLThreadUtil_onSurfaceChanged(JNIEnv* env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLThreadUtil_onDrawFrame(JNIEnv* env, jobject obj);

#ifdef __cplusplus
}
#endif

#endif //TRUNK_GLTHREADUTILJNI_H
