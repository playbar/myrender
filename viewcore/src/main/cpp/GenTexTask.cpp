//
// Created by mac on 16/8/19.
//

#include "GenTexTask.h"
#include "android/bitmap.h"

#define GENTEXTASKCLASS "com/bfmj/viewcore/util/GLGenTexTask"

jclass GenTexTask::mThizClass = NULL;
jfieldID GenTexTask::mClassID = NULL;
jmethodID GenTexTask::mExportTextureId = NULL;

//GenTexTask gTexTask[4];

static GenTexTask* getGenTexTask( JNIEnv* env, jobject thiz)
{
    GenTexTask *p = (GenTexTask*)env->GetIntField(thiz, GenTexTask::mClassID );
    return p;
}

JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLGenTexTask_NativeInit(JNIEnv* env, jobject thiz)
{
    jobject objThiz = env->NewGlobalRef(thiz); //need DeleteGlobalRef

    if( GenTexTask::mThizClass == NULL ){
        GenTexTask::mThizClass = env->FindClass(GENTEXTASKCLASS);
        GenTexTask::mClassID = env->GetFieldID( GenTexTask::mThizClass, "mClassID", "I");
        GenTexTask::mExportTextureId = env->GetMethodID( GenTexTask::mThizClass, "ExportTextureId", "()V");
    }

    GenTexTask *pTask = new GenTexTask(env, objThiz);
//    static int index = 0;
//    if( index >= 10 ){
//        index = 0;
//    }
//    GenTexTask *pTask = &gTexTask[index];
//    ++index;
//    pTask->SetJObject( objThiz );
    gThreadPool.AddTask( pTask );
//    GenTexTask *pTmp = (GenTexTask*)env->GetIntField(objThiz, GenTexTask::mClassID );
//    env->SetIntField( objThiz, GenTexTask::mClassID, (int)pTask);
    return;
}


JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLGenTexTask_NativeUninit(JNIEnv* env, jobject thiz)
{
    GenTexTask *pTmp = getGenTexTask( env, thiz);
    delete pTmp;
}

JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLGenTexTask_NativeQueueEvent(JNIEnv* env, jobject thiz)
{
//    GenTexTask *pTmp = getGenTexTask( env, thiz );
//    pTmp->QueueEvent();
    return;
}

JNIEXPORT void JNICALL Java_com_bfmj_viewcore_util_GLGenTexTask_NativeGenTexId(JNIEnv* env,
            jobject thiz, jobject bmp, jint width, jint height)
{
    GenTexTask *pTmp = getGenTexTask( env, thiz );
    pTmp->GenTexID( bmp, width, height);
    return;
}


GenTexTask::GenTexTask(JNIEnv* env, jobject thiz)
{
    mThiz = thiz;
}

GenTexTask::GenTexTask()
{
    mThiz = NULL;
}

GenTexTask::~GenTexTask()
{
//    if( mThiz != NULL ){
//        JNIEnv *env=0;
//        gs_jvm->AttachCurrentThread(&env, NULL);
//        env->DeleteGlobalRef( mThiz );
////        gs_jvm->DetachCurrentThread();
//    }
}

void GenTexTask::SetJObject( jobject thiz)
{
    mThiz = thiz;
}

void GenTexTask::QueueEvent(){
    gThreadPool.AddTask( this );
}

void GenTexTask::GenTexID( jobject bmp, int width, int height )
{
//    mpData = new unsigned char[mWidth * mHeight * 4];
//    AndroidBitmapInfo infocolor;
//    void *pixels = 0;
//    AndroidBitmap_getInfo(mEnv, mBitmap, &infocolor);
//    AndroidBitmap_lockPixels(mEnv, mBitmap, &pixels);
//    memcpy( mpData, pixels, mWidth * mHeight * 4);
//    AndroidBitmap_unlockPixels(mEnv, mBitmap);

    gThreadPool.AddTask( this );
}

int GenTexTask::Run()
{
//    if( !eglMakeCurrent( gDisplay, gAuxSurface, gAuxSurface, gShareContext )){
//        LOGI("make current error");
//    }

//    LOGE("Run");

//    pthread_t seft = pthread_self();
//    char chId[32] = {0};
//    sprintf( chId, "task thread id=%u", seft );
//    LOGE( chId );

//    GLuint textureId = CreateTexture2D();
    if( mThiz != NULL ){
        JNIEnv *env=0;
        gs_jvm->AttachCurrentThread(&env, NULL);
//        jclass cls = env->GetObjectClass(mThiz);
//        jmethodID fieldPtr = env->GetMethodID(cls,"ExportTextureId", "()V");
//        env->CallVoidMethod( mThiz, fieldPtr, 0 );
        env->CallVoidMethod( mThiz, GenTexTask::mExportTextureId, 0 );
        env->DeleteGlobalRef( mThiz );
        gs_jvm->DetachCurrentThread();
    }

    return 0;
}


GLuint GenTexTask::CreateSimpleTexture2D( )
{
    // Texture object handle
    GLuint textureId =0;

    // 2x2 Image, 3 bytes per pixel (R, G, B)
    GLubyte pixels[4 * 3] = {
        255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
        255, 255,   0  // Yellow
    };

    // Use tightly packed data
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

    // Generate a texture object
    glGenTextures ( 1, &textureId );


    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, textureId );

    // Load the texture
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

    // Set the filtering mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    return textureId;

}


GLuint GenTexTask::CreateTexture2D( )
{
    GLuint textureId =0;
    // Use tightly packed data
    glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

    // Generate a texture object
    glGenTextures ( 1, &textureId );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, textureId );

    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

    // Set the filtering mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    return textureId;

}

