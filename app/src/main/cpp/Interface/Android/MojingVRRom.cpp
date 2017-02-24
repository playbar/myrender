#include <jni.h>
#include "../../Base/BuildType.h"

#include "../../MojingAPI.h"
#include "MojingVRRomAPI.h"
#include "../../MojingManager.h"
#include "../../Parameters/MojingParameters.h"
#include "../../Base/GlUtils.h"
#include "../../Base/MojingMath.h"
#include "../../Platform/MojingPlatformBase.h"

#include "../../Render/MojingRenderMultiThread_3288.h"
#include "../../Render/Models/Model.h"
#include "../../Render/Models/SkyboxModel.h"
#include "../../Render/Models/SphereModel.h"
#include "../../Render/Models/RectangleModel.h"

#include "../../Profile/AtlasProfile.h"

using namespace Baofeng::Mojing;

extern "C"
{
	// 绘制默认几个模型：天空盒、天空球
	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingVRRom_CreateModel(
		JNIEnv * env, jobject obj, jint modelType, jint textureType, jstring atlasJson);
	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingVRRom_DrawModel(
		JNIEnv * env, jobject obj, jint iModelKey, jint eye, jint textureId, jfloatArray jViewMatrix);

	JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingVRRom_SetRectModelPosition(JNIEnv * env, jobject obj, jint iModelKey, jfloatArray jRectPosition);

	JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingVRRom_DrawOverlay(JNIEnv * env, jobject obj, jint OverlayIndex, jint LeftEyeTexID, jint RightEyeTexID, jfloat fLeft, jfloat fTop, jfloat fWidth, jfloat fHeight, jfloat fDistanceInMetre);

	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingVRRom_DirectionalRadiaInRect(JNIEnv *jEnv, jclass, jfloatArray jm4Direction, jfloatArray jv2TopLeft, jfloatArray jv2BottomRight, jfloat fZ, jfloatArray jv2Point);
	JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingVRRom_SelectRectByDirectional(JNIEnv *jEnv, jclass, jfloatArray jm4Direction, jfloatArray jv3TopLeft, jfloatArray jv3BottomRight);

	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingVRRom_CreateDistortion(JNIEnv *, jclass);
	JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingVRRom_DrawDistortion(JNIEnv *, jclass, jint iBothEyeTexture);

};


// 创建一个模型
JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingVRRom_CreateModel(
	JNIEnv * env, jobject obj, jint modelType, jint textureType, jstring atlasJson)
{
	const char * charJson = env->GetStringUTFChars(atlasJson, 0);

	return MojingVRRomAPI_CreateModel(modelType, textureType, charJson);

	env->ReleaseStringUTFChars(atlasJson, charJson);
}
// 绘制模型
JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingVRRom_DrawModel(
	JNIEnv * env, jobject obj, jint iModelKey, jint jEye, jint jTextureId, jfloatArray jViewMatrix)
{
	jfloat* pMatrixArray = env->GetFloatArrayElements(jViewMatrix, NULL);
	
	if (pMatrixArray && env->GetArrayLength(jViewMatrix) == 16)
	{
		MojingVRRomAPI_DrawModel(iModelKey, jEye, jTextureId, pMatrixArray);
	}
	env->ReleaseFloatArrayElements(jViewMatrix, pMatrixArray, 0);
}
// 设置方片模型的位置
JNIEXPORT void JNICALL Java_com_baofeng_mojing_MojingVRRom_SetRectModelPosition(JNIEnv * env, jobject obj, jint iModelKey, jfloatArray jRectPosition)
{
	jfloat* pRectPosition = env->GetFloatArrayElements(jRectPosition, NULL);

	if (pRectPosition && env->GetArrayLength(jRectPosition) == 12)
	{
		MojingVRRomAPI_SetRectModelPosition(iModelKey, pRectPosition);
	}

	env->ReleaseFloatArrayElements(jRectPosition, pRectPosition, 0);
}



/*以下为VR_ROM平台专有函数*/
JNIEXPORT bool JNICALL Java_com_baofeng_mojing_MojingVRRom_DrawOverlay(JNIEnv * env, jobject obj, jint OverlayIndex, jint LeftEyeTexID, jint RightEyeTexID, jfloat fLeft, jfloat fTop, jfloat fWidth, jfloat fHeight, jfloat fDistanceInMetre)
{
	return MojingVRRomAPI_DrawOverlay(OverlayIndex, LeftEyeTexID, RightEyeTexID, fLeft, fTop, fWidth, fHeight, fDistanceInMetre);
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingVRRom_DirectionalRadiaInRect(JNIEnv *jEnv, jclass, jfloatArray jm4Direction, jfloatArray jv2TopLeft, jfloatArray jv2BottomRight, jfloat fZ, jfloatArray jv2Point)
{
	jfloat* pMatrixArray = jEnv->GetFloatArrayElements(jm4Direction, NULL);
	jfloat* pTopLeft = jEnv->GetFloatArrayElements(jv2TopLeft, NULL);
	jfloat* pBottomRight = jEnv->GetFloatArrayElements(jv2BottomRight, NULL);
	jfloat* pPoint = jEnv->GetFloatArrayElements(jv2Point, NULL);

	jsize lengthMatrixArray = (jEnv)->GetArrayLength(jm4Direction);
	jsize lengthTopLeft = (jEnv)->GetArrayLength(jv2TopLeft);
	jsize lengthBottomRight = (jEnv)->GetArrayLength(jv2BottomRight);
	jsize lengthPoint = (jEnv)->GetArrayLength(jv2Point);
	jboolean bRet = false;
	if (lengthMatrixArray < 16 ||
		lengthTopLeft < 2 ||
		lengthBottomRight < 2 ||
		lengthPoint < 2)
	{
		for (jsize i = 0; i < lengthPoint; i++)
		{
			pPoint[i] = -1.0f;
		}
	}
	else
	{
		Matrix4f m4;
		for (int i = 0; i < 16; i++)
		{
			m4.M[i / 4][i % 4] = pMatrixArray[i];
		}
		Vector2f v2TopLeft(pTopLeft[0], pTopLeft[1]);
		Vector2f v2BottomRight(pBottomRight[0], pBottomRight[1]);
		Vector2f v2Ret = MojingSDK_Math_DirectionalRadiaInRect(m4, v2TopLeft, v2BottomRight, fZ);
		pPoint[0] = v2Ret.x;
		pPoint[1] = v2Ret.y;
		bRet = pPoint[0] > 0 && pPoint[1] > 0;
	}

	jEnv->ReleaseFloatArrayElements(jm4Direction, pMatrixArray, 0);
	jEnv->ReleaseFloatArrayElements(jv2TopLeft, pTopLeft, 0);
	jEnv->ReleaseFloatArrayElements(jv2BottomRight, pBottomRight, 0);
	jEnv->ReleaseFloatArrayElements(jv2Point, pPoint, 0);

	return bRet;
}

JNIEXPORT jint JNICALL Java_com_baofeng_mojing_MojingVRRom_SelectRectByDirectional(JNIEnv *jEnv, jclass, jfloatArray jm4Direction, jfloatArray jv3TopLeft, jfloatArray jv3BottomRight)
{
	jint iRet = -1;
	jfloat* pMatrixArray = jEnv->GetFloatArrayElements(jm4Direction, NULL);
	jfloat* pTopLeft = jEnv->GetFloatArrayElements(jv3TopLeft, NULL);
	jfloat* pBottomRight = jEnv->GetFloatArrayElements(jv3BottomRight, NULL);
	jsize lengthMatrixArray = (jEnv)->GetArrayLength(jm4Direction);
	jsize lengthTopLeft = (jEnv)->GetArrayLength(jv3TopLeft);
	jsize lengthBottomRight = (jEnv)->GetArrayLength(jv3BottomRight);

	if (lengthMatrixArray == 16 &&
		lengthTopLeft == lengthBottomRight &&
		0 == (lengthTopLeft % 3))
	{
		Matrix4f m4;
		for (int i = 0; i < 16; i++)
		{
			m4.M[i / 4][i % 4] = pMatrixArray[i];
		}
		int iRectCounts = lengthTopLeft / 3;
		iRet = MojingSDK_Math_SelectRectByDirectional(m4, iRectCounts, pTopLeft, pBottomRight);
	}
	jEnv->ReleaseFloatArrayElements(jm4Direction, pMatrixArray, 0);
	jEnv->ReleaseFloatArrayElements(jv3TopLeft, pTopLeft, 0);
	jEnv->ReleaseFloatArrayElements(jv3BottomRight, pBottomRight, 0);

	return iRet;
}

JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingVRRom_CreateDistortion(JNIEnv *, jclass)
{
	return MojingVRRomAPI_CreateDistortion(NULL , 0);
}
JNIEXPORT jboolean JNICALL Java_com_baofeng_mojing_MojingVRRom_DrawDistortion(JNIEnv *, jclass, jint iBothEyeTexture)
{
	return	MojingVRRomAPI_DrawDistortion(iBothEyeTexture , NULL , NULL);
}