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
	const char* Brand,
	const char* Model,
	const char* Serial,
	const char* szMerchantID,
	const char* szAppID,
	const char* szAppKey,
	const char* szAppName,
	const char* szPackageName,
	const char* szUserID,
	const char* szChannelID,
	const char* ProfilePath);

void MojingSDK_SetGlassesSerialNumber(const char* lpszSN);
bool MojingSDK_SetEngineVersion(const char* lpszEngine);
void MojingSDK_Validate(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szChannelID);
void MojingSDK_CheckPackage(const char* szAppName, const char* szCaseCode);

//for report
bool MojingSDK_ReportReInit();
void MojingSDK_AppExit();
bool MojingSDK_AppResume(const char* szUniqueID);
void MojingSDK_AppPause();
void MojingSDK_AppPageStart(const char* szPageName);
void MojingSDK_AppPageEnd(const char* szPageName);
void MojingSDK_AppSetEvent(const char* szEventName, const char* szEventChannelID, const char* szEventInName, float eInData, const char* szEventOutName, float eOutData);
void MojingSDK_ReportLog(int iLogType, const char* szTypeName, const char* szLogContent, bool pd);
#ifdef MJ_OS_ANDROID
void MojingSDK_ReportUserAction(const char* szActionType, const char* szItemID, const char* strJsonValue);
#endif
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

#define MOJING_WORLDKEY_DEFAULT "DefaultMojingWorld" 
#define MOJING_WORLDKEY_LAST "LastMojingWorld" 
// MojingSDK_GetMojingWorldKey函数仅供内部使用，不得公开！
String MojingSDK_GetMojingWorldKey(const char* szKeyType);
// MojingSDK_SetMojingWorldKey函数仅供内部使用，不得公开！
bool MojingSDK_SetMojingWorldKey(const char* szKeyType, const char* szGlassesName);

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
bool MojingSDK_IsInMachine();
bool MojingSDK_IsUseUnityForSVR();
bool MojingSDK_IsUseForDayDream();
int MojingSDK_GetSocketPort();// Socket向服务报告端口的
int MojingSDK_StartTrackerChecker(int nSampleFrequence);
int MojingSDK_GetTrackerCheckerResult(__tagSampleCheckeResult *pOutCheckeResult);
/*以下接口用于控制DD的陀螺仪*/
// 开启或关闭DD陀螺仪
void MojingSDK_DD_SetEnableTracker(bool bEnable);

// 获取DD陀螺仪状态
bool MojingSDK_DD_GetEnableTracker();

// 当DD陀螺仪关闭时，存放真实的陀螺仪数据
bool MojingSDK_DD_GetLastHeadView(float* pfViewMatrix);
/*以上接口用于控制DD的陀螺仪*/
#endif
#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
int MojingSDK_Device_GetKeymask(int iID, int *pKeyMask);
float MojingSDK_Device_GetCurrentPoaseInfo(int iID, float *pQuart, float *pAngularAccel, float *pLinearAccel, float *pPosition, unsigned int *pKeystatus);
float MojingSDK_Device_GetFixPoaseInfo(int iID, float *pQuart, float *pAngularAccel, float *pLinearAccel, float *pPosition);
float MojingSDK_Device_GetControlFixCurrentInfo(int iID, float *pQuart, float *pAngularAccel, float *pLinearAccel, float *pPosition, unsigned int *pKeystatus);
void MojingSDK_Device_GetFixScore(int* pStatus, int* pScore);
#endif

void MojingSDK_ResetSensorOrientation(void);
void MojingSDK_ResetSensorOrientation2(void);
void MojingSDK_ResetTracker(void);
int MojingSDK_StartTrackerCalibration();
float MojingSDK_IsTrackerCalibrated();
double MojingSDK_getLastSensorState(float* fArray);
uint64_t  MojingSDK_getLastHeadView(float* pfViewMatrix);
int MojingSDK_getPredictionHeadView(float* pfViewMatrix, double time);
void MojingSDK_getLastHeadEulerAngles(float* pfEulerAngles);
void MojingSDK_getLastHeadQuarternion(float *w, float *x, float *y, float *z);
void MojingSDK_Math_DirectionalInScreen(float* fArray);

void MojingSDK_StopTracker(void);
void MojingSDK_SendSensorData(float* pArray, double dLastSampleTime);
void MojingSDK_SendControllerData(const Baofeng::UByte* pArray, int dataLen);
void MojingSDK_SendControllerDataV2(float* pArray, double dTimestamp, bool bRecenter);
	
/************************************************************************/
/* 厂商-产品-镜片-APP管理                                               */
/************************************************************************/
String MojingSDK_GetManufacturerList(const char* strLanguageCodeByISO639);
String MojingSDK_GetProductList(const char* strManufacturerKey, const char* strLanguageCodeByISO639);
String MojingSDK_GetGlassList(const char* strProductKey, const char* strLanguageCodeByISO639);
String MojingSDK_GetGlassInfo(const char* strGlassKey, const char* strLanguageCodeByISO639);
String MojingSDK_GenerationGlassKey(const char* strProductQRCode, const char* strGlassQRCode);

#if 0
/************************************************************************/
/* 智能设备连接管理														*/
/************************************************************************/
/*
	{
		"ClassName" : "DeviceList",
		"DeviceList":[
			{
				"ID":1,
				"DeviceName" : "MojingJoystick",
				"Connect":0
			},
			{
			"ID":2,
			"DeviceName" : "BE-TOP",
			"Connect":1
			}
		]
	}	


*/
// 返回一个Json串，表示当前可以被连接/已经的连接的全部设备列表
String MojingSDK_Device_GetDeviceList();

// 连接指定的设备，传入的ID应该是MojingSDK_Device_GetDeviceList返回的JSON串中Device节点的ID值
bool MojingSDK_Device_ConnectDevice(int iID);
// 断开指定设备的连接。传入ID=0表示断开所有的设备。
bool MojingSDK_Device_DisconnectDevice(int iID);
// 检查设备的连接状态
bool MojingSDK_Device_IsDeviceConnected(int iID);
// 获取已经连接的设备上的按键列表的掩码，返回长度为32的int数组。分别表示按键状态码的第0位到第31位表示的键值。
int MojingSDK_Device_GetKeymask(int iID,int *pKeyMask);

// 获取设备的姿态信息和其他传感器信息,返回值为采样的时间。
float MojingSDK_Device_IsDeviceConnected(int iID, 
	float *pQuart, /*四元数表示的旋转，依次为XYZW*/
	float *pAngularAccel, /*角加速度，依次为XYZ*/
	float *pLinearAccel，/*线加速度，依次为XYZ*/
	float *pPosition, /*设备的空间位置，以米为单位，默认是0,0,0。*/
	unsigned int *pKeystatus/*设备上的按键状态，默认是0表示没有按键被按下*/ );
#endif



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
void MojingSDK_GetProjectionMatrix(int eye,
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
int    MojingSDK_GetSensorOrigin();
bool   MojingSDK_SetSensorOrigin(int SensorOrigin);
/************************************************************************/
/* 辅助函数：帮助界面开发者做数学运算                                   */
/************************************************************************/
// 计算射线 Direction 与世界坐标[v2TopLeft ，v2BottomRight]在Z平面上相对于v2TopLeft的交汇点。如果没有相交返回(-1,-1)
Vector2f MojingSDK_Math_DirectionalRadiaInRect(Matrix4f Direction, Vector2f v2TopLeft, Vector2f v2BottomRight, float fZ);
// 计算射线 Direction 与世界坐标中的iRectCounts个平面中的哪一个相交，返回-1表示没有相交
int MojingSDK_Math_SelectRectByDirectional(Matrix4f Direction, int iRectCounts, float * pv3TopLeft, float * pv3BottomRight);

void MojingSDK_Math_GetOverlayPosition3D(float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre, Vector4f &OverlayRectLeft, Vector4f &OverlayRectRight);

// 计算矩形中心在考虑景深的情况下在近平面上的位置投影
Rectf MojingSDK_Math_OverlayPosition3D(unsigned int eyeTextureType, Rectf rcRect, float fDistanceInMetre);
/**
 * Get light sensation for Mojing5.
 * @return Current light sensation value in [UNIT]
 */
bool MojingSDK_IsLowPower();
void MojingSDK_SetHDMWorking(bool bHDMWorking);
bool MojingSDK_IsHDMWorking();

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
