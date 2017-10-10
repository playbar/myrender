#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <dlfcn.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include "elf_hooker.h"

#include <jni.h>
#include <EGL/eglext.h>
#include <Base/MojingLog.h>
#include "elf_eglhook.h"

static elf_hooker __hooker;

typedef EGLImageKHR (*FP_eglCreateImageKHR)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
FP_eglCreateImageKHR pfun_eglCreateImageKHR = NULL;
EGLImageKHR mjeglCreateImageKHR(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list)
{
    LOGE("mjeglCreateImageKHR");
    EGLint eglImgAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE, EGL_NONE };
    EGLImageKHR img = pfun_eglCreateImageKHR(dpy, eglGetCurrentContext(), EGL_GL_TEXTURE_2D_KHR, buffer, eglImgAttrs);
    return img;
}

typedef EGLClientBuffer (*FP_eglCreateNativeClientBufferANDROID)(const EGLint *attrib_list);
FP_eglCreateNativeClientBufferANDROID pfun_eglCreateNativeClientBufferANDROID = NULL;
EGLClientBuffer mjeglCreateNativeClientBufferANDROID (const EGLint *attrib_list)
{
    LOGE("mjeglCreateNativeClientBufferANDROID");
    GLuint textureId;
    glGenTextures ( 1, &textureId );
    glBindTexture ( GL_TEXTURE_2D, textureId );
    int width = attrib_list[1];
    int height = attrib_list[3];
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    return (EGLClientBuffer)textureId;
}


//typedef void (*__eglMustCastToProperFunctionPointerType)(void);
EGLAPI __eglMustCastToProperFunctionPointerType (*old_eglGetProcAddress)(const char *procname) = NULL;
EGLAPI __eglMustCastToProperFunctionPointerType mj_eglGetProcAddress(const char *procname)
{

    __eglMustCastToProperFunctionPointerType pfun = old_eglGetProcAddress(procname);

    if(strcmp(procname, "eglCreateImageKHR") == 0)
    {
        LOGE("mj_eglGetProcAddress, eglCreateImageKHR");
        pfun_eglCreateImageKHR = (FP_eglCreateImageKHR)pfun;
        pfun = (__eglMustCastToProperFunctionPointerType)mjeglCreateImageKHR;
    }
    if(strcmp(procname, "eglCreateNativeClientBufferANDROID") == 0 )
    {
        LOGE("mj_eglGetProcAddress, eglCreateNativeClientBufferANDROID");
        pfun_eglCreateNativeClientBufferANDROID = (FP_eglCreateNativeClientBufferANDROID)pfun;
        pfun = (__eglMustCastToProperFunctionPointerType)mjeglCreateNativeClientBufferANDROID;
    }
    return pfun;
}


void hookEglGetProcAddress()
{
    LOGE("hookEglGetProcAddress");
    __hooker.phrase_proc_maps();
    __hooker.dump_module_list();
    __hooker.hook_module("libgvr.so", "eglGetProcAddress", (void*)mj_eglGetProcAddress, (void**)&old_eglGetProcAddress);
}



