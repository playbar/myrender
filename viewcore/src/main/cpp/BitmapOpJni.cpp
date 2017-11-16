
#include <string.h>
#include <inttypes.h>
#include "viewcore.h"



extern "C" JNIEXPORT jstring JNICALL
Java_com_bfmj_viewcore_util_BitmapOp_stringFromJNI(JNIEnv *env, jobject thiz) {
    LOGI( "log" );
    return env->NewStringUTF("Hello from JNI!");
}