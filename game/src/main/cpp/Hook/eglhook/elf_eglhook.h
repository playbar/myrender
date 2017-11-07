#ifndef __ELF_EGLHOOK_H__
#define __ELF_EGLHOOK_H__


#ifdef __cplusplus
extern "C" {
#endif

void hookEglGetProcAddress();
void hookGLFun();
void hookUnityFun();
void startHookThread(bool isdd);

#ifdef __cplusplus
}
#endif

#endif

