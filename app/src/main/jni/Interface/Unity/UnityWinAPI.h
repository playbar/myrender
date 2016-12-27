//
//  MojingWinAPI.h
//  MojingSDK
//
//  Created by 姹ょ敵浜� on 16/1/25.
//  Copyright (c) 2016骞� baofeng. All rights reserved.
//

#ifndef __MojingSDK_UNITY_WINAPI__
#define __MojingSDK_UNITY_WINAPI__

#define UNITY_EXPORT __declspec(dllexport)

#define LOG_LEVEL_ALL       0
#define LOG_LEVEL_TRACE     LOG_LEVEL_ALL
#define LOG_LEVEL_DEBUG     1000
#define LOG_LEVEL_INFO      2000
#define LOG_LEVEL_WARN      3000
#define LOG_LEVEL_ERROR     4000
#define LOG_LEVEL_FATAL     5000
#define LOG_LEVEL_OFF       6000

extern "C"
{
	UNITY_EXPORT void Unity_Log(int logLevel, char* info, char* filename, int line);

	UNITY_EXPORT bool Unity_Init(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szChannelID, const char* szProfilePath);
	UNITY_EXPORT bool Unity_OnSurfaceChanged(int newWidth, int newHeight);
	UNITY_EXPORT float Unity_GetGlassesFOV();
	UNITY_EXPORT float Unity_GetGlassesSeparation();
	UNITY_EXPORT int Unity_GetGlassesSeparationInPixel();
	UNITY_EXPORT void Unity_GetScreenSize(float* pSize);
	UNITY_EXPORT bool Unity_StartTracker(int nSampleFrequence);
	UNITY_EXPORT bool Unity_StartTrackerCalibration();
	UNITY_EXPORT float Unity_IsTrackerCalibrated();
	UNITY_EXPORT void Unity_getLastHeadView(float* pfViewMatrix);
	UNITY_EXPORT void Unity_getLastHeadQuarternion(float &w, float &x, float &y, float &z);
	UNITY_EXPORT void Unity_StopTracker(void);
	UNITY_EXPORT void Unity_SetOverlay(void* iLeftOverlayTextureID, void* iRightOverlayTextureID, float fLeft, float fTop, float fWidth, float fHeight);
	UNITY_EXPORT void Unity_SetOverlay3D(int iEyeType, void* iTextureID, float fWidth, float fHeight, float fDistanceInMetre);
	UNITY_EXPORT void Unity_SetTextureID(void* iLeftTextureID, void* iRightTextureID);
	UNITY_EXPORT void Unity_ResetSensorOrientation();
	UNITY_EXPORT void Unity_ResetSensorOrientation2();
	UNITY_EXPORT void Unity_ResetTracker(void);
	UNITY_EXPORT int  Unity_GetTextureSize(void);
	UNITY_EXPORT bool Unity_SetMojingWorld(const char * szGlassesName, bool bTimeWarp , void * pAndroidActivity);
	UNITY_EXPORT void Unity_SetEnableTimeWarp(bool bEnable);
	UNITY_EXPORT bool Unity_IsInMojingWorld(const char * szGlassesName);
	UNITY_EXPORT bool Unity_EnterMojingWorld(const char * szGlassesName, bool bTimewarp, void* pOtherParams);
	UNITY_EXPORT bool Unity_ChangeMojingWorld(const char * szGlassesName);
	UNITY_EXPORT bool Unity_IsGlassesNeedDistortion(void);
	UNITY_EXPORT void Unity_SetCenterLine(int iWdith, int iColR = 255, int iColG = 255, int iColB = 255, int iColA = 255);
	UNITY_EXPORT int  Unity_GetMojingWorldDistortionMesh(const char * szGlassesName, int iScreenWidth, int iScreenHeight, int iWidthCells, int iHeightCells,
		float *pFOV, float *pGlassesSeparationInMeter,
		float * pVerts, float * pUV, int * pIndices);

	UNITY_EXPORT void UnitySetGraphicsDevice (void* device, int deviceType, int eventType);
	UNITY_EXPORT void UnityRenderEvent(int eventID);

	UNITY_EXPORT void Unity_FreeString(char * pReleaseString);
	UNITY_EXPORT const char* Unity_GetDefaultMojingWorld(const char* strLanguageCodeByISO639);
	UNITY_EXPORT const char* Unity_GetLastMojingWorld(const char* strLanguageCodeByISO639);
	UNITY_EXPORT const char* Unity_GetManufacturerList(const char* strLanguageCodeByISO639);
	UNITY_EXPORT const char* Unity_GetProductList(const char* strManufacturerKey, const char* strLanguageCodeByISO639);
	UNITY_EXPORT const char* Unity_GetGlassList(const char* strProductKey, const char* strLanguageCodeByISO639);
	UNITY_EXPORT const char* Unity_GetGlassInfo(const char* strGlassKey, const char* strLanguageCodeByISO639);
	UNITY_EXPORT const char* Unity_GenerationGlassKey(const char* strProductQRCode, const char* strGlassQRCode);
    
	UNITY_EXPORT void Unity_getProjectionMatrix(int eye, bool bVrMode, float fFOV, float fNear, float fFar, float* pfProjectionMatrix, int* pfViewRect);
	UNITY_EXPORT const char* Unity_GetSDKVersion();
	UNITY_EXPORT const char* Unity_GetGlasses();
	UNITY_EXPORT bool Unity_SetUserSettings(const char * sUserSettings);
	UNITY_EXPORT const char* Unity_GetUserSettings();
	UNITY_EXPORT bool Unity_SetEngineVersion(const char* lpszEngine);
    
	UNITY_EXPORT void Unity_AppSetContinueInterval(int interval);
	UNITY_EXPORT void Unity_AppSetReportInterval(int interval);
	UNITY_EXPORT void Unity_AppSetReportImmediate(bool bImmediate);
	UNITY_EXPORT void Unity_AppResume();
	UNITY_EXPORT void Unity_AppPause();
	UNITY_EXPORT void Unity_AppPageStart(const char* szPageName);
	UNITY_EXPORT void Unity_AppPageEnd(const char* szPageName);
	UNITY_EXPORT void Unity_AppSetEvent(const char* szEventName, const char* szEventChannelID, const char* szEventInName, float eInData, const char* szEventOutName, float eOutData);
	UNITY_EXPORT void Unity_AppReportLog(int iLogType, const char* szTypeName, const char* szLogContent);
	UNITY_EXPORT const char* Unity_AppGetRunID();
}
#endif /* defined(__MojingSDK_UNITY_WINAPI__) */
