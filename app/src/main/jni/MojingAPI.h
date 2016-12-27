#pragma once

#include "Base/MojingTypes.h"
#include "Base/MojingString.h"
#include "Base/MojingMath.h"

#ifdef LOG4CPLUS_IMPORT
#include "3rdPart/log4cplus/LogInterface.h"
#else
#include "LogTraker/LogInterface.h"
#endif

using namespace Baofeng::Mojing;

#ifdef MJ_OS_ANDROID
#ifndef __tagSampleCheckeResult
namespace Baofeng
{
	namespace Mojing
	{
		struct  __tagSampleCheckeResult;
	}
}
#endif
#endif

// Init for all
bool MojingSDK_Init(int nWidth,
	int nHeight,
	float xdpi,
	float ydpi,
	char* Brand,
	char* Model,
	char* Serial,
	const char* szMerchantID,
	const char* szAppID,
	const char* szAppKey,
	const char* szAppName,
	const char* szPackageName,
	const char* szUserID,
	const char* szChannelID,
	const char* ProfilePath);

bool MojingSDK_SetEngineVersion(const char* lpszEngine);
void MojingSDK_Validate(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szChannelID);

//for report
void MojingSDK_AppExit();
bool MojingSDK_AppResume(const char* szUniqueID);
void MojingSDK_AppPause();
void MojingSDK_AppPageStart(const char* szPageName);
void MojingSDK_AppPageEnd(const char* szPageName);
void MojingSDK_AppSetEvent(const char* szEventName, const char* szEventChannelID, const char* szEventInName, float eInData, const char* szEventOutName, float eOutData);
void MojingSDK_ReportLog(int iLogType, const char* szTypeName, const char* szLogContent, bool pd);
void MojingSDK_AppSetContinueInterval(int interval);
void MojingSDK_AppSetReportInterval(int interval);
void MojingSDK_AppSetReportImmediate(bool bReportImmediate);

// API for distortion
bool MojingSDK_EnterMojingWorld(const char * szGlassesName, bool bEnableMultiThread, bool bEnableTimeWarp);

// 注意：Unreal的UV和顶点是混在一起的，Unity(DayDream) 是分开的。所以当Unreal调用的时候并不会返回pOutUV。
// 返回值是顶点的数量
int MojingSDK_GetDistortionMesh(const char * szGlassesName, int iScreenWidth, int iScreenHeight, const int iWidthCells, const int iHeightCells,
	float *pFOV, float *pGlassesSeparationInMeter,
	void * pOutVectex, void * pOutUV, void * pOutIndex);

// int MojingSDK_GetMojingWorldDistortionMesh(int iScreenWidth, int iScreenHeight, int iWidthCells, int iHeightCells, void * pVerts, void * pIndices);

void MojingSDK_SetEnableTimeWarp(bool bEnable);
int  MojingSDK_GetTextureSize(void);
void MojingSDK_SetImageYOffset(float fYOffset);
void MojingSDK_SetTextureID(int nLeftEyeTextureID, int nrightEyeTextureID);
bool MojingSDK_DrawTexture(void);
bool MojingSDK_DrawTexture(int nLeftEyeTextureID, int nRightEyeTextureID, int iLeftLayerTextureID = 0, int iRightLayerTextureID = 0);
//设置叠加涂层的位置，以单眼X|Y 的范围[0,1]区间,左上角为（0， 0）点，右下角为（1,1)点
void MojingSDK_SetOverlayPosition(float fLeft, float fTop, float fWidth, float fHeight);
void MojingSDK_SetOverlayPosition3D(unsigned int eyeTextureType, /*float fLeft, float fTop, */float fWidth, float fHeight, float fDistanceInMetre);
void MojingSDK_SetOverlayPosition3D_V2(unsigned int eyeTextureType, float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre);
bool MojingSDK_ChangeMojingWorld(const char * szGlassesName);
bool MojingSDK_LeaveMojingWorld();
bool MojingSDK_SetDefaultMojingWorld(const char * szGlassesName);
bool MojingSDK_OnSurfaceChanged(int newWidth, int newHeight);
String MojingSDK_GetDefaultMojingWorld(const char* strLanguageCodeByISO639);
String MojingSDK_GetLastMojingWorld(const char* strLanguageCodeByISO639);
float MojingSDK_GetFOV();
// 获取指定镜片的畸变框物理尺寸
float MojingSDK_GetDistortionR(const char* strGlassesKey);
float MojingSDK_GetGlassesSeparation();
float MojingSDK_GetGlassesSeparationInPix();
void MojingSDK_GetScreenSize(float* pSize);
unsigned int MojingSDK_GetEyeTexture(unsigned int eyeTextureType, unsigned int &iWidth, unsigned int &iHeight, unsigned int &format);
void MojingSDK_SetCenterLine(int iWidth, int colR, int colG, int colB, int colA);
bool MojingSDK_DistortionSetLensSeparation(float fNewValue);
float MojingSDK_DistortionGetLensSeparation();

/* NOT USED
bool MojingSDK_AddTextureLayout(unsigned int nLeftEyeTextureId, unsigned int nRightEyeTextureId, int zorder);
bool MojingSDK_DeleteTextureLayout(int zorder);
*/

// API for tracking head
bool MojingSDK_StartTracker(int nSampleFrequence, const char* szGlassName = NULL);
int MojingSDK_CheckSensors();
int MojingSDK_GetMaxSensorsSampleRate(); // 返回最大的采样率或者-1表示出错
#ifdef MJ_OS_ANDROID
int MojingSDK_StartTrackerChecker(int nSampleFrequence);
int MojingSDK_GetTrackerCheckerResult(__tagSampleCheckeResult *pOutCheckeResult);
#endif
void MojingSDK_ResetSensorOrientation(void);
void MojingSDK_ResetSensorOrientation2(void);
void MojingSDK_ResetTracker(void);
int MojingSDK_StartTrackerCalibration();
float MojingSDK_IsTrackerCalibrated();
uint64_t  MojingSDK_getLastHeadView(float* pfViewMatrix);
int MojingSDK_getPredictionHeadView(float* pfViewMatrix, double time);
void MojingSDK_getLastHeadEulerAngles(float* pfEulerAngles);
void MojingSDK_getLastHeadQuarternion(float *w, float *x, float *y, float *z);
void MojingSDK_Math_DirectionalInScreen(float* fArray);

void MojingSDK_StopTracker(void);
void MojingSDK_SendSensorData(float* pArray, double dLastSampleTime);

/************************************************************************/
/* 厂商-产品-镜片-APP管理                                               */
/************************************************************************/
String MojingSDK_GetManufacturerList(const char* strLanguageCodeByISO639);
String MojingSDK_GetProductList(const char* strManufacturerKey, const char* strLanguageCodeByISO639);
String MojingSDK_GetGlassList(const char* strProductKey, const char* strLanguageCodeByISO639);
String MojingSDK_GetGlassInfo(const char* strGlassKey, const char* strLanguageCodeByISO639);
String MojingSDK_GenerationGlassKey(const char* strProductQRCode, const char* strGlassQRCode);

/************************************************************************/
/* Texture Backer                                                       */ 
/************************************************************************/
#ifndef DO_NOT_USING_TEXTURE_BACKER
bool MojingSDK_StartBacker(int width, int height, int texID = 0/*0表示由低层生成TextureID*/);
bool MojingSDK_BackerTexture(int texID, int x, int y, int width, int height, int desX, int desY);
bool MojingSDK_GetBackerTexID(int* pTexID);
bool MojingSDK_StopBacker();
#endif

// API for get SDK status
bool MojingSDK_GetInitSDK(void);
bool MojingSDK_GetStartTracker(void);
bool MojingSDK_GetInMojingWorld(void);
const char* MojingSDK_GetSDKVersion(void);
const char* MojingSDK_GetGlasses(void);
bool MojingSDK_IsGlassesNeedDistortion(void);
bool MojingSDK_IsGlassesNeedDistortionByName(const char * szGlassesName);
void MojingSDK_getProjectionMatrix(int eye,
	bool bVrMode,
	float fFOV,
	float fNear,
	float fFar, 
	float* pfProjectionMatrix,
	int* pfViewRect);
const char* MojingSDK_GetCpuName();
const char* MojingSDK_GetGpuName();

String MojingSDK_GetUserSettings();
bool   MojingSDK_SetUserSettings(const char * sUserSettings);
/************************************************************************/
/* 辅助函数：帮助界面开发者做数学运算                                   */
/************************************************************************/
// 计算射线 Direction 与世界坐标[v2TopLeft ，v2BottomRight]在Z平面上相对于v2TopLeft的交汇点。如果没有相交返回(-1,-1)
Vector2f MojingSDK_Math_DirectionalRadiaInRect(Matrix4f Direction, Vector2f v2TopLeft, Vector2f v2BottomRight, float fZ);
// 计算射线 Direction 与世界坐标中的iRectCounts个平面中的哪一个相交，返回-1表示没有相交
int MojingSDK_Math_SelectRectByDirectional(Matrix4f Direction, int iRectCounts, float * pv3TopLeft, float * pv3BottomRight);

void MojingSDK_Math_GetOverlayPosition3D(float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre, Vector4f &OverlayRectLeft, Vector4f &OverlayRectRight);

/**
 * Get light sensation for Mojing5.
 * @return Current light sensation value in [UNIT]
 */
bool MojingSDK_IsLowPower();

#ifdef USING_MINIDUMP
void MojingSDK_CheckCrashReport();
#endif

#ifndef MJ_OS_WIN32
bool MojingSDK_ktxLoadTextureN(const char* const filename, unsigned int* pTexture, unsigned int* pTarget, int* pHeight, int* pWidth, int* pDepth, bool* pIsMipmapped, unsigned int* pGlerror, int * ktxError);
#endif

#ifdef _DEBUG
bool MojingSDKPrivate_DistortionSetParamet(int iSegment, float *KR, float *KG, float *KB);
int MojingSDKPrivate_DistortionGetParamet(float *KR, float *KG, float *KB);
bool MojingSDKPrivate_DistortionSaveParamet();
bool MojingSDKPrivate_DistortionResetParamet();
#endif
