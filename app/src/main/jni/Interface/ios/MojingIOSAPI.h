//
//  MojingSDK.h
//  MojingSDK
//
//  Created by 汤申亮 on 15/7/14.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#import <Foundation/Foundation.h>

/*
 Metal
 */
#include <TargetConditionals.h>
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
#import <simd/simd.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#endif


#define LOG_LEVEL_ALL       0
#define LOG_LEVEL_TRACE     LOG_LEVEL_ALL
#define LOG_LEVEL_DEBUG     1000
#define LOG_LEVEL_INFO      2000
#define LOG_LEVEL_WARN      3000
#define LOG_LEVEL_ERROR     4000
#define LOG_LEVEL_FATAL     5000
#define LOG_LEVEL_OFF       6000

@interface MojingSDK : NSObject

void MojingSDK_API_Log(int logLevel, NSString* sInfo, NSString* sFileName, int line);
bool MojingSDK_API_Init(NSString* strMerchantID, NSString* strAppID, NSString* strAppKey, NSString* strChannelID);
bool MojingSDK_API_StartTracker(int nSampleFrequence);
int MojingSDK_API_CheckSensors();
bool MojingSDK_API_StartTrackerCalibration();
float MojingSDK_API_IsTrackerCalibrated();
void MojingSDK_API_ResetSensorOrientation();
void MojingSDK_API_ResetTracker();
void MojingSDK_API_getLastHeadView(float* pMatrixArray);
void MojingSDK_API_getLastHeadEulerAngles(float* pEulerAngles);
void MojingSDK_API_getLastHeadQuarternion(float* pUnitQuarternion, int length);
void MojingSDK_API_StopTracker();
void MojingSDK_API_SetEnableTimeWarp(bool bEnable);
bool MojingSDK_API_EnterMojingWorld(NSString* GlassesName, bool bEnableTimeWarp = false);
bool MojingSDK_API_ChangeMojingWorld(NSString* GlassesName);
bool MojingSDK_API_LeaveMojingWorld();
bool MojingSDK_API_SetDefaultMojingWorld(NSString* GlassesName);
NSString* MojingSDK_API_GetDefaultMojingWorld(NSString* strLanguageCodeByISO639);
NSString* MojingSDK_API_GetLastMojingWorld(NSString* strLanguageCodeByISO639);
float MojingSDK_API_GetMojingWorldFOV();
bool MojingSDK_API_OnSurfaceChanged(int newWidth, int newHeight);
int MojingSDK_API_GetEyeTexture(int eyeTextureType, int* poutParams);
float MojingSDK_API_GetGlassesSeparationInPix();
bool MojingSDK_API_DrawTexture(int LeftTexID, int RightTexID, int iLeftOverlayTextureID = 0, int iRightOverlayTextureID = 0);
void MojingSDK_API_SetOverlayPosition(float fLeft , float fTop, float fWidth, float fHeight );
void MojingSDK_API_SetOverlayPosition3D( int eyeTextureType, /*float fLeft, float fTop, */float fWidth, float fHeight, float fDistanceInMetre);
void MojingSDK_API_SetOverlayPosition3D_V2(unsigned int eyeTextureType, float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre);
void MojingSDK_API_SetImageYOffset(float fYOffset);
void MojingSDK_API_SetCenterLine(int iWidth, int colR, int colG, int colB, int colA);
bool MojingSDK_API_DistortionSetLensSeparation(float fNewValue);
float MojingSDK_API_DistortionGetLensSeparation();

bool MojingSDK_API_GetInitSDK();
bool MojingSDK_API_GetStartTracker();
bool MojingSDK_API_GetInMojingWorld();
NSString* MojingSDK_API_GetSDKVersion();
NSString* MojingSDK_API_GetGlasses();

NSString* MojingSDK_API_GetManufacturerList(NSString* strLanguageCodeByISO639);
NSString* MojingSDK_API_GetProductList(NSString* strManufacturerKey, NSString* strLanguageCodeByISO639);
NSString* MojingSDK_API_GetGlassList(NSString* strProductKey, NSString* strLanguageCodeByISO639);
NSString* MojingSDK_API_GetGlassInfo(NSString* strGlassKey, NSString* strLanguageCodeByISO639);
NSString* MojingSDK_API_GenerationGlassKey(NSString* strProductQRCode, NSString* strGlassQRCode);

void MojingSDK_API_AppSetContinueInterval(int interval);
void MojingSDK_API_AppSetReportInterval(int interval);
void MojingSDK_API_AppSetReportImmediate(bool bImmediate);
void MojingSDK_API_AppResume();
void MojingSDK_API_AppPause();
void MojingSDK_API_AppPageStart(NSString* strPageName);
void MojingSDK_API_AppPageEnd(NSString* strPageName);
void MojingSDK_API_AppSetEvent(NSString* strEventName, NSString* strEventChannelID, NSString* strEventInName, float eInData, NSString* strEventOutName, float eOutData);
void MojingSDK_API_ReportLog(int iLogType, NSString* strTypeName, NSString* strContent);
const char* MojingSDK_API_AppGetUserID();
const char* MojingSDK_API_AppGetRunID();

// Joystick
NSString* MojingSDK_API_GetJoystickList();
bool MojingSDK_API_SetJoystick(int iID);

/*
 Metal
 */
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
bool MojingSDK_API_EnterMojingWorld_Metal(const char * GlassesName,
                                          bool bEnableMultiThread,
                                          bool bEnableTimeWarp,
                                          id<MTLDevice> device,
                                          id<MTLCommandQueue> cmdQueue,
                                          id<MTLCommandBuffer> cmdBuffer);

bool MojingSDK_API_DrawTexture_Metal(id<CAMetalDrawable> drawable,
                                     id<MTLTexture> eyeTextureLeft,
                                     id<MTLTexture> eyeTextureRight,
                                     id<MTLTexture> overlayTextureLeft = nullptr,
                                     id<MTLTexture> overlayTextureRight = nullptr);
#endif

/************************************************************************/
/* 以下是SDK私有接口，禁止发布于MojingSDK                                    */
/************************************************************************/
void MojingSDKPrivate_API_FuncTest();

@end
