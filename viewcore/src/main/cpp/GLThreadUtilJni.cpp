//
// Created by mac on 16/8/19.
//

#include <string.h>
#include <inttypes.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "ThreadPool.h"
#include "GenTexTask.h"


EGLContext gShareContext = 0;
EGLDisplay gDisplay = 0;
EGLSurface gAuxSurface = 0;

CThreadPool gThreadPool(1);

void destroySharedContext(){
    if( gShareContext != 0 ){
        eglDestroySurface( gDisplay, gAuxSurface);
        eglDestroyContext( gDisplay, gShareContext);
    }
}


void createSharedContext(){
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    EGLint numConfigs = 0;
    EGLConfig config;

    const EGLint attribList[] = {
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_RENDERABLE_TYPE, 64,
            EGL_DEPTH_SIZE, 0,
            EGL_STENCIL_SIZE, 0,
            EGL_NONE
    };

    EGLint pbufferAttribs[] = {
            EGL_WIDTH, 1,
            EGL_HEIGHT, 1,
            EGL_TEXTURE_TARGET, EGL_NO_TEXTURE,
            EGL_TEXTURE_FORMAT, EGL_NO_TEXTURE,
            EGL_NONE
    };

    // Choose config
    EGLDisplay display = eglGetCurrentDisplay( );
    EGLContext context = eglGetCurrentContext();
    if ( !eglChooseConfig ( display, attribList, &config, 1, &numConfigs ) )
    {
        return;
    }

    gAuxSurface = eglCreatePbufferSurface(display, config, pbufferAttribs);
    if(gAuxSurface == NULL) {
        return;
    }

    gShareContext = eglCreateContext( display, config, context, contextAttribs );
    return;

}


JNIEXPORT void JNICALL
Java_com_bfmj_viewcore_util_GLThreadUtil_onSurfaceCreated(JNIEnv* env, jobject obj)
{
    gDisplay = eglGetCurrentDisplay();
    destroySharedContext();
    createSharedContext();
    gThreadPool.Create();
    return;
}

JNIEXPORT void JNICALL
Java_com_bfmj_viewcore_util_GLThreadUtil_onSurfaceChanged(JNIEnv* env, jobject obj, jint width, jint height)
{

}

JNIEXPORT void JNICALL
Java_com_bfmj_viewcore_util_GLThreadUtil_onDrawFrame(JNIEnv* env, jobject obj)
{

}

