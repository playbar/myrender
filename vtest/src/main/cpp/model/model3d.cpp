//
// Created by liujiankang on 2016/11/18.
//

#include <myLogger.h>
#include "model3d.h"

JavaVM *gs_JVM;
static int gneedDetach = 0;

JNIEnv* AttachCurrentThreadJNI()
{
    JNIEnv *env = 0;
    jint result = -1;
    if (gs_JVM->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        int status = gs_JVM->AttachCurrentThread( &env, 0 );
        if( status < 0 )
        {
            return 0;
        }
        gneedDetach = 1;
    }
    return env;
}

void DetachCurrentThreadJNI()
{
    if( gneedDetach )
    {
        gs_JVM->DetachCurrentThread();
        gneedDetach = 0;
    }

}


jint JNI_OnLoad( JavaVM* vm, void *reserved){
    gs_JVM = vm;

    jint result = JNI_VERSION_1_6;
    MyLOGI("Model3d JNI_OnLoad vm=[%p]", vm);
    return  result;
}
