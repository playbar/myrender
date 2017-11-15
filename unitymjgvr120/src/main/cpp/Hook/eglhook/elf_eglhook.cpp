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
#include <sys/system_properties.h>
#include <Hook/waithook.h>
#include <Hook/HookGVR/HookGVRTools.h>
#include "elf_eglhook.h"

static elf_hooker __hooker;

static int gismaligpu = false;
int needswapbuffer = 0;
int rendertid = 0;
int gvrmajorversion = 1;
int gvrminorversion = 20;

int gwidth = 0;
int gheight = 0;


void* get_module_base(pid_t pid,const char* module_name)
{
    FILE* fp;
    long addr = 0;
    char *pch;
    char filename[32];
    char line[1024];

    if(pid<0){
        snprintf(filename,sizeof(filename),"/proc/self/maps",pid);
    }else{
        snprintf(filename,sizeof(filename),"/proc/%d/maps",pid);
    }
    fp = fopen(filename,"r");
    if(fp!=NULL){
        while(fgets(line,sizeof(line),fp)){
            if(strstr(line,module_name)){
                pch = strtok(line,"-");
                addr = strtoul(pch,NULL,16);
                if(addr==0x8000)
                    addr = 0;
                break;
            }
        }
        fclose(fp);
    }
    return (void*)addr;
}

bool hookbase(int base, void * fpReplactToFunction, void ** fpOutRealFunction)
{
    bool bRet = false;
    void *pModule = get_module_base(getpid(), "libgvr.so");
    void *pFunc = (void*)((int)pModule + base + 1);
    if (registerInlineHook((uint32_t)pFunc, (uint32_t)fpReplactToFunction, (uint32_t **)fpOutRealFunction) == DETOUR_OK)
    {
        if (inlineHook((uint32_t)pFunc) == DETOUR_OK)
        {
            bRet = true;
        }
    }
    else
    {
        LOGE("Try registerInlineHook error!!, tid=%d", gettid());
    }

    return bRet;
}

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
    EGLint eglImgAttrs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE };
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
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
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
    static int count = 0;
    if( count < 20 )
    LOGE("mj_glViewport, x=%d, y=%d, w=%d, h=%d, tid=%d", x, y, width, height, gettid());
    ++count;
//    gwidth = width;
//    gheight = height;
    gwidth = 1295;
    gheight = 1528;
    return old_glViewport(x, y, width, height);
}

void  (*old_glBindBuffer) (GLenum target, GLuint buffer) = NULL;
void mj_glBindBuffer (GLenum target, GLuint buffer)
{
    static int count = 0;
    if( count < 20)
    LOGE("mj_glBindBuffer, target=%d, bufferid=%d, tid=%d", target, buffer, gettid());
    ++count;
    return old_glBindBuffer(target, buffer);
}

extern double m_dRotateSpeed;
double dRotateSpeed = 0;
typedef void (*Fn_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
Fn_glDrawElements old_glDrawElements = NULL;
void mj_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
    LOGE("mj_glDrawElements, tid=%d", gettid());
    old_glDrawElements(mode, count, type, indices);

//    if( dRotateSpeed > 0.1f )
//        glClearColor ( 1.0f, 1.0f, 1.0f, 1.0f );
//    else
//        glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
//    glClear ( GL_COLOR_BUFFER_BIT );
//    LOGE("rotatespeed=%f", dRotateSpeed);

    if( gismaligpu && needswapbuffer && rendertid != gettid())
    {
//        glFinish();
        dRotateSpeed = m_dRotateSpeed;
        old_eglSwapBuffers(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW));
        LOGE("mj_glDrawElements, old_eglSwapBuffers, tid=%d", gettid());
        needswapbuffer = 0;
    }
    return;
}

bool (*old_sub_71FE8)(int a1, int a2,  int a3) = NULL;
bool mj_sub_71FE8(int a1, int a2,  int a3)
{
    void *p = (void*)a1;
    void *p1 = *(void**)a1;
    char *pstr = *(char**)((char*)p + 4);
    LOGE("fun name: %s", pstr);
    bool re = old_sub_71FE8(a1, a2, a3);
    void *pfun = *(void**)a1;
    if(strcmp(pstr, "DrawElements") == 0)
    {
        old_glDrawElements = (Fn_glDrawElements)pfun;
        *(void**)a1 = (void*)mj_glDrawElements;
    }

    return re;
}

//typedef void (*__eglMustCastToProperFunctionPointerType)(void);
EGLAPI __eglMustCastToProperFunctionPointerType (*old_eglGetProcAddress)(const char *procname) = NULL;
EGLAPI __eglMustCastToProperFunctionPointerType mj_eglGetProcAddress(const char *procname)
{

    LOGE("mj_eglGetProcAddress, procname=%s", procname);
    const char *glrender = (const char *)glGetString(GL_RENDERER);
    // 1.40(包括1.40) mali gpu 出现单眼问题
    static bool gbfirst = false;
    if( gbfirst /*&&gvrminorversion>= 20 && glrender && strstr(glrender, "Mali") != NULL */)
    {
        LOGE("mj_eglGetProcAddress, hook mj_eglSwapBuffers", procname);
        gismaligpu = true;
        gbfirst = false;
        __hooker.phrase_proc_maps();
        __hooker.hook_module("libandroid_runtime.so", "eglSwapBuffers", (void *) mj_eglSwapBuffers, (void **) &old_eglSwapBuffers);
        if( gvrmajorversion == 1 && gvrminorversion == 40 ) {
            hookbase(0x71FE8, (void*)mj_sub_71FE8, (void**)&old_sub_71FE8);
        }
        if( gvrmajorversion == 1 && gvrminorversion == 20 ) {
            hookbase(0x80018, (void*)mj_sub_71FE8, (void**)&old_sub_71FE8);
        }

    }
    __eglMustCastToProperFunctionPointerType pfun = old_eglGetProcAddress(procname);

//    if(strcmp(procname, "glViewport") == 0 )
//    {
//        old_glViewport = (Fn_glViewport)pfun;
//        pfun = (__eglMustCastToProperFunctionPointerType)mj_glViewport;
//    }
    if(gismaligpu && gvrmajorversion >= 1 && gvrminorversion > 40 ) {
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

char*  (*old_strncpy)(char* __restrict, const char* __restrict, size_t) = NULL;
/*
 * daydream unity游戏，以cardboard模式运行，就替换一下string
 */
char*  my_strncpy(char* __restrict dest, const char* __restrict src, size_t s){
//    LOGE("my_strncpy, dest=%s, src=%s, size=%d", dest, src, s);
    if(strcmp("cardboard",src)==0){
        LOGI("my_strncpy,src:cardboard,new src daydream");
        return old_strncpy(dest,"daydream",s);
    }

    return old_strncpy(dest,src,s);
}

int (*old_strcmp)(const char *, const char *) = NULL;
int mj_strcmp(const char *, const char *)
{
    return 0;
}

void hookEglGetProcAddress()
{
    LOGE("hookEglGetProcAddress");
    char sdkIntStr[PROP_VALUE_MAX];
    memset(sdkIntStr,'\0',PROP_VALUE_MAX);
    __system_property_get("ro.build.version.sdk", sdkIntStr);
    int sdkInt=atoi(sdkIntStr);

//    if(   gvrmajorversion >= 1 && gvrminorversion <= 40 )
//    {
//        hook((uint32_t) glDrawElements, (uint32_t) mj_glDrawElements, (uint32_t **) &old_glDrawElements);
//    }
    if( sdkInt >= 20) {
        __hooker.phrase_proc_maps();
//    __hooker.hook_module("libandroid_runtime.so", "eglSwapBuffers", (void *) mj_eglSwapBuffers, (void **) &old_eglSwapBuffers);
        __hooker.hook_module("libgvr.so", "eglGetProcAddress", (void *) mj_eglGetProcAddress, (void **) &old_eglGetProcAddress);
//    __hooker.hook_module("libgvr.so", "glViewport", (void*)mj_glViewport, (void**)&old_glViewport);
//    __hooker.hook_module("libgvr.so", "glDrawElements", (void*)mj_glDrawElements, (void**)&old_glDrawElements);
//    __hooker.hook_module("libgvr.so", "eglSwapBuffers", (void*)mj_eglSwapBuffers, (void**)&old_eglSwapBuffers);
    }
//    if(sdkInt < 20) {
//        __hooker.hook_module("libunity.so", "strncpy", (void *) my_strncpy, (void **) &old_strncpy);
//    }

}

void hookGLFun()
{
    hook((uint32_t) glViewport, (uint32_t)mj_glViewport, (uint32_t **) &old_glViewport);
    hook((uint32_t) glBindBuffer, (uint32_t)mj_glBindBuffer, (uint32_t **)&old_glBindBuffer);
    return;
}

void hookUnityFun()
{
    LOGE("hookUnityFun");
    char sdkIntStr[PROP_VALUE_MAX];
    memset(sdkIntStr,'\0',PROP_VALUE_MAX);
    __system_property_get("ro.build.version.sdk", sdkIntStr);
    int sdkInt=atoi(sdkIntStr);
//    if(sdkInt < 20)
    {
        __hooker.phrase_proc_maps();
        __hooker.hook_module("libunity.so", "strncpy", (void *) my_strncpy, (void **) &old_strncpy);
    }
}


void *waitLoadingHook(void *data)
{
    LOGE("waitLoadingHook begin");
    bool isdd = (bool*)data;
    if( isdd) {
        LOGE("waitLoadingHook is dd");
    }
    else {
        LOGE("waitLoadingHook is not dd");
    }
    char sdkIntStr[PROP_VALUE_MAX];
    memset(sdkIntStr,'\0',PROP_VALUE_MAX);
    __system_property_get("ro.build.version.sdk", sdkIntStr);
    int sdkInt=atoi(sdkIntStr);

    const char *pchgvr = "libgvr.so";
    const char *pchunity = "libunity.so";

    bool bgvr = false;
    bool bunity = false;

    long gvrimage = (long)getimagebase(pchgvr);
    long unityimage = (long)getimagebase(pchunity);
    while (gvrimage == -1 || unityimage == -1) {
        usleep(5000);
//        if( !bunity )
//            unityimage = (long)getimagebase(pchunity);
//        if( !bunity && unityimage != -1)
//        {
//            if(sdkInt < 20) {
//                __hooker.phrase_proc_maps();
//                __hooker.hook_module("libunity.so", "strncpy", (void *) my_strncpy, (void **) &old_strncpy);
//                LOGE("hook libunity strncpy");
//            }
//            bunity = true;
//        }
        if(!bgvr)
            gvrimage = (long)getimagebase(pchgvr);
        if( !bgvr && gvrimage != -1 )
        {
            HookGVRTools::Init();
            if( !isdd && sdkInt >= 20 ) {
                __hooker.phrase_proc_maps();
                __hooker.hook_module("libgvr.so", "eglGetProcAddress", (void *) mj_eglGetProcAddress, (void **) &old_eglGetProcAddress);
                LOGE("hook libgvr eglGetProcAddress");
            }

            LOGE("hook libgvr mojing");
            bgvr = true;
            break;
        }

    }
    LOGE("waitLoadingHook end");
    return (void*)0;
}

void startHookThread(bool isdd)
{
    pthread_t   pthread_id;
    pthread_create(&pthread_id, NULL, waitLoadingHook, (void*)isdd);
    return;
}
