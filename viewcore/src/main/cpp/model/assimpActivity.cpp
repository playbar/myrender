/*
 *    Copyright 2016 Anand Muralidhar
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <jni.h>
#include "./modelAssimp/modelAssimp.h"
#include "./common/myJNIHelper.h"

#ifdef __cplusplus
extern "C" {
#endif

// global pointer is used in JNI calls to reference to same object of type Cube
ModelAssimp *gAssimpObject =NULL;

// global pointer to instance of MyJNIHelper that is used to read from assets
MyJNIHelper * gHelperObject=NULL;

/**
 * Create the persistent native object and also initialize the single helper object
 */

JNIEXPORT void JNICALL
Java_baofeng_mojing_model3d_ModelObj_CreateObjectNative(JNIEnv *env, jobject instance,
                                                        jobject assetManager,
                                                        jstring pathToInternalDir) {
    // TODO
    gHelperObject = new MyJNIHelper(env, instance, assetManager, pathToInternalDir);
    gAssimpObject = new ModelAssimp();
}

JNIEXPORT void JNICALL
Java_baofeng_mojing_model3d_ModelObj_DeleteObjectNative(JNIEnv *env, jobject instance) {
    // TODO
    if (gAssimpObject != NULL) {
        delete gAssimpObject;
    }
    gAssimpObject = NULL;

    if (gHelperObject != NULL) {
        delete gHelperObject;
    }
    gHelperObject = NULL;
}

JNIEXPORT void JNICALL
Java_baofeng_mojing_model3d_ModelObj_DrawFrameNative(JNIEnv *env, jobject instance,
                                                     jfloatArray mvpMatrix_) {
    if(mvpMatrix_ == NULL){
        return ;
    }
    jint arrayLen = env->GetArrayLength(mvpMatrix_);
    if(arrayLen != 16) {
        return;
    }
    jfloat *mvpMatrix = env->GetFloatArrayElements(mvpMatrix_, NULL);

    // TODO
    if (gAssimpObject == NULL) {
        return;
    }
    gAssimpObject->Render(mvpMatrix);
    env->ReleaseFloatArrayElements(mvpMatrix_, mvpMatrix, 0);
}

JNIEXPORT jboolean JNICALL
Java_baofeng_mojing_model3d_ModelObj_LoadModel(JNIEnv *env, jobject instance, jstring modelName_) {
    const char *modelName = env->GetStringUTFChars(modelName_, 0);
    // TODO
    if (gAssimpObject == NULL) {
        return JNI_FALSE;
    }

    gAssimpObject->LoadModel(modelName);
    env->ReleaseStringUTFChars(modelName_, modelName);
    return JNI_TRUE;
}

#ifdef __cplusplus
}
#endif
