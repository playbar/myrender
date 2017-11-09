#include <jni.h>
#include <stddef.h>
#include "../include/ktx.h"





#ifndef _Included_cn_mojing_sdk_glhelper_KtxLoader
#define _Included_cn_mojing_sdk_glhelper_KtxLoader
#define _Included_cn_mojing_sdk_glhelper_KtxLoader
#ifdef __cplusplus
extern "C" {
#endif


/*
 * Class:     cn_mojing_sdk_glhelper_KtxLoader
 * Method:    LoadTextureFromMemory
 * Signature: (Ljava/nio/ByteBuffer;Lcn/mojing/sdk/glhelper/Texture;)I
 */
	JNIEXPORT jint JNICALL  Java_com_baofeng_mojing_sdk_glhelper_KtxLoader_LoadTextureFromMemory(JNIEnv *env, jobject, jbyteArray byteBuffer, jobject texture)
{
    //return (jint)1;
    /*
	jclass cls = env->FindClas("java/nio/ByteBuffer");
	jfieldID fid = env->GetFieldID(cls, "data", "Ljava/nio/ByteBuffer;");
	jobject bar = env->GetObjectField(byteBuffer, fid);
	
	void *data = (env)->GetDirectBufferAddress(byteBuffer);
	GLsizei size = (env)->GetDirectBufferCapacity(byteBuffer);
	
	jclass cls = env->GetObjectClass(byteBuffer);
	jfieldID fid = env->GetFieldID(cls, "data", "Ljava/nio/ByteBuffer;");
	jobject bar = env->GetObjectField(byteBuffer, fid);
	void *data= (void*)env->GetDirectBufferAddress(bar);

    // buffer.array()
    jmethodID ArrayMethodId = env->GetMethodID(cls,"array","()[B");
    jbyteArray OutByteArray = (jbyteArray)env->CallObjectMethod(byteBuffer, ArrayMethodId);

    // buffer.array().length
    GLsizei size = env->GetArrayLength(OutByteArray);//*/

	jbyte* data = env->GetByteArrayElements(byteBuffer, NULL);
	GLsizei size = env->GetArrayLength(byteBuffer);

    KTX_dimensions ktx_dimensions = {0, 0, 0};
    GLboolean isMipmapped = GL_FALSE;
    GLenum glError = 0;

    jclass clsTexture = env->GetObjectClass(texture);

    jfieldID fid = env->GetFieldID(clsTexture, "textureID", "I");

	GLenum target = 0;
	GLuint Texture = env->GetIntField(texture, fid);

    KTX_error_code error_code = ktxLoadTextureM(data, size, &Texture, &target,
                                                &ktx_dimensions, &isMipmapped,
                                                &glError,
                                                NULL, NULL);

    // gl error code
	jfieldID fidLastGlerror = env->GetFieldID(clsTexture, "lastglerror", "I");
	env->SetIntField(texture, fidLastGlerror, glError);

    if (error_code == KTX_SUCCESS)
    {
	    // set textureID
	    jfieldID fidtextureID = env->GetFieldID(clsTexture, "textureID", "I");
	    env->SetIntField(texture, fidtextureID, Texture);
	    
        // set width
        jfieldID fidwidth = env->GetFieldID(clsTexture, "width", "I");
        env->SetIntField(texture, fidwidth, ktx_dimensions.width);

        // set height
        jfieldID fidheight = env->GetFieldID(clsTexture, "height", "I");
        env->SetIntField(texture, fidheight, ktx_dimensions.height);

        // set depth
        jfieldID fiddepth = env->GetFieldID(clsTexture, "depth", "I");
        env->SetIntField(texture, fiddepth, ktx_dimensions.depth);

        // set isMipmapped
        jfieldID fidisMipmapped = env->GetFieldID(clsTexture, "isMipmapped", "Z");
	    env->SetBooleanField(texture, fidisMipmapped, isMipmapped);
    }

    return  (jint)error_code; //*/

}

/*
 * Class:     cn_mojing_sdk_glhelper_KtxLoader
 * Method:    LoadTextureFromFile
 * Signature: (Ljava/lang/String;Lcn/mojing/sdk/glhelper/Texture;)I
 */
    JNIEXPORT jint JNICALL Java_com_baofeng_mojing_sdk_glhelper_KtxLoader_LoadTextureFromFile(JNIEnv *env, jobject, jstring jsPath, jobject texture)

{

    //return 1;
    ///*
    
    KTX_dimensions ktx_dimensions = {0, 0, 0};
    GLboolean isMipmapped = GL_FALSE;
    GLenum glError = 0;
    jclass clsTexture = env->GetObjectClass(texture);

    jfieldID fid = env->GetFieldID(clsTexture, "textureID", "I");

	GLenum target = 0;
	GLuint Texture = env->GetIntField(texture, fid);
    const char * szPath = env->GetStringUTFChars(jsPath, 0);
    KTX_error_code error_code = ktxLoadTextureN(szPath, &Texture, &target,
                                                &ktx_dimensions, &isMipmapped,
                                                &glError,
                                                NULL, NULL);

    env->ReleaseStringUTFChars(jsPath, szPath);

    // gl error code
   jfieldID fidLastGlerror = env->GetFieldID(clsTexture, "lastglerror", "I");
   env->SetIntField(texture, fidLastGlerror, glError);

    if (error_code == KTX_SUCCESS)
    {
	    // set textureID
	    jfieldID fidtextureID = env->GetFieldID(clsTexture, "textureID", "I");
	    env->SetIntField(texture, fidtextureID, Texture);
	    
        // set width
        jfieldID fidwidth = env->GetFieldID(clsTexture, "width", "I");
        env->SetIntField(texture, fidwidth, ktx_dimensions.width);

        // set height
        jfieldID fidheight = env->GetFieldID(clsTexture, "height", "I");
        env->SetIntField(texture, fidheight, ktx_dimensions.height);

        // set depth
        jfieldID fiddepth = env->GetFieldID(clsTexture, "depth", "I");
        env->SetIntField(texture, fiddepth, ktx_dimensions.depth);

        // set isMipmapped
        jfieldID fidisMipmapped = env->GetFieldID(clsTexture, "isMipmapped", "Z");
	    env->SetBooleanField(texture, fidisMipmapped, isMipmapped);
    }

    return  (jint)error_code;
    //*/
}  	

/*
 * Class:     cn_mojing_sdk_glhelper_KtxLoader
 * Method:    SaveTextureToFile
 * Signature: (Ljava/lang/String;Lcn/mojing/sdk/glhelper/Texture;)I
 */
JNIEXPORT jint JNICALL Java_cn_mojing_sdk_glhelper_KtxLoader_SaveTextureToFile (JNIEnv *env, jobject, jstring jsPath, jobject texture)
{
    //KTX_texture_info texture_info;
    //texture_info.
    return 0;
}
#ifdef __cplusplus
}
#endif
#endif
