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
#include <Hook/Global/detour.h>
#include "elf_eglhook.h"

static elf_hooker __hooker;

static int gismaligpu = false;
int needswapbuffer = 0;
int rendertid = 0;
int gvrmajorversion = 0;
int gvrminorversion = 0;


int hook(uint32_t target_addr, uint32_t new_addr, uint32_t **proto_addr)
{
    if (registerInlineHook(target_addr, new_addr, proto_addr) != DETOUR_OK) {
        return -1;
    }
    if (inlineHook((uint32_t) target_addr) != DETOUR_OK) {
        return -1;
    }

    return 0;
}

int unHook(uint32_t target_addr)
{
    if (inlineUnHook(target_addr) != DETOUR_OK) {
        return -1;
    }

    return 0;
}

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


EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface) = NULL;
EGLBoolean mj_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
    LOGE("mj_eglSwapBuffers, tid=%d", gettid());
    EGLBoolean re = true;
//    eglMakeCurrent(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW), eglGetCurrentSurface(EGL_READ), eglGetCurrentContext());
    if( gismaligpu && rendertid != gettid())
    {
        glFinish();
        needswapbuffer = 1;
    }else{
        re = old_eglSwapBuffers(dpy, surface);
    }
    return re;
}

typedef void (*Fn_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
Fn_glViewport old_glViewport = NULL;
void mj_glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    LOGE("mj_glViewport, x=%d, tid=%d", x, gettid());
    return old_glViewport(x, y, width, height);

}


typedef void (*Fn_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
Fn_glDrawElements old_glDrawElements = NULL;
void mj_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
    LOGE("mj_glDrawElements, tid=%d", gettid());
    old_glDrawElements(mode, count, type, indices);
    if( needswapbuffer && rendertid != gettid())
    {
//        glFinish();
        old_eglSwapBuffers(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW));
//        LOGE("mj_glDrawElements, old_eglSwapBuffers, tid=%d", gettid());
        needswapbuffer = 0;
    }
    return;
}

//typedef void (*__eglMustCastToProperFunctionPointerType)(void);
EGLAPI __eglMustCastToProperFunctionPointerType (*old_eglGetProcAddress)(const char *procname) = NULL;
EGLAPI __eglMustCastToProperFunctionPointerType mj_eglGetProcAddress(const char *procname)
{

    LOGE("mj_eglGetProcAddress, procname=%s", procname);
    const char *glrender = (const char *)glGetString(GL_RENDERER);
    if(glrender && strstr(glrender, "Mali") != NULL ){
//        if( gvrmajorversion > 1 && gvrminorversion >= 20 )
            gismaligpu = true;
    }
    __eglMustCastToProperFunctionPointerType pfun = old_eglGetProcAddress(procname);

    if(strcmp(procname, "glViewport") == 0 )
    {
        old_glViewport = (Fn_glViewport)pfun;
        pfun = (__eglMustCastToProperFunctionPointerType)mj_glViewport;
    }
    if(gvrmajorversion >= 1 && gvrminorversion > 40 ) {
            if (strcmp(procname, "glDrawElements") == 0) {
                old_glDrawElements = (Fn_glDrawElements) pfun;
                pfun = (__eglMustCastToProperFunctionPointerType) mj_glDrawElements;
            }
        }

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
    if(   gvrmajorversion >= 1 && gvrminorversion <= 40 ) {
        hook((uint32_t) glDrawElements, (uint32_t) mj_glDrawElements, (uint32_t **) &old_glDrawElements);
    }
    __hooker.phrase_proc_maps();
    __hooker.dump_module_list();

    __hooker.hook_module("libandroid_runtime.so", "eglSwapBuffers", (void *) mj_eglSwapBuffers, (void **) &old_eglSwapBuffers);
    __hooker.hook_module("libgvr.so", "eglGetProcAddress", (void*)mj_eglGetProcAddress, (void**)&old_eglGetProcAddress);
//    __hooker.hook_module("libgvr.so", "glViewport", (void*)mj_glViewport, (void**)&old_glViewport);
//    __hooker.hook_module("libgvr.so", "glDrawElements", (void*)mj_glDrawElements, (void**)&old_glDrawElements);
//    __hooker.hook_module("libgvr.so", "eglSwapBuffers", (void*)mj_eglSwapBuffers, (void**)&old_eglSwapBuffers);
}



