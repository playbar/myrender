//
//  MojingSDK.m
//  MojingSDK
//
//  Created by 汤申亮 on 15/7/14.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#import "MojingIOSAPI.h"
#import "MojingIOSBase.h"
#import "MJTuner/MJTuner.h"

#include "AES.h"
#include "MojingManager.h"
#include "MojingRenderBase.h"
#include "MojingRenderMetal.h"
#include "MojingParameters.h"
#include "MojingDistortion.h"
#include "MojingSDKStatus.h"
#include "MojingPlatformBase.h"
#include "MojingAPI.h"
#include "MJiCadeTransfor.h"


#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

using namespace Baofeng::Mojing;

@implementation MojingSDK
void  MojingSDK_API_RegisterMJTuner(int minScale, int maxScale, float minLensSeparation, float maxLensSeparation)
{
    NSString *str = [NSString stringWithFormat:@"RegisterMJTuner Enter, Scale(%d ~ %d), LensSeparation(%f ~ %f)", minScale, maxScale, minLensSeparation, maxLensSeparation];
    MojingSDK_API_Log(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    
    //Tuner Connect state
    [MJTuner sharedMJTuner].btnConnect.valueChangedHandler = ^void(MJTunerButton *button, BOOL pressed, float value){
        NSLog(@"Mojing Tuner %@", pressed? @"connected":@"disconnected");
        NSString *str = [NSString stringWithFormat:@"Mojing Tuner %@", pressed? @"connected":@"disconnected"];
        MojingSDK_API_Log(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    //Menu
    [MJTuner sharedMJTuner].buttonMenu.valueChangedHandler = ^void(MJTunerButton *button, BOOL pressed, float value){
        NSLog(@"菜单键%@", pressed? @"按下":@"松开");
        NSString *str = [NSString stringWithFormat:@"菜单键%@", pressed? @"按下":@"松开"];
        MojingSDK_API_Log(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    //Thumb turn
    [MJTuner sharedMJTuner].thumbTurn.valueChangedHandler = ^void(MJTunerThumb *thumbTurn, uint8_t value){
        float fLensSeparation = minLensSeparation+(maxLensSeparation-minLensSeparation)*(value-minScale)/(maxScale-minScale);
        MojingSDK_DistortionSetLensSeparation(fLensSeparation);
        
        NSLog(@"Tuner thumb value: (%d), LensSeparation = %f", value, fLensSeparation);
        NSString *str = [NSString stringWithFormat:@"Tuner thumb value: (%d), LensSeparation = %f", value, fLensSeparation];
        MojingSDK_API_Log(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
}

void MojingSDK_API_Log(int logLevel, NSString* sInfo, NSString* sFileName, int line)
{
#ifdef ENABLE_LOGGER
    log4cplus::Logger const & log = log4cplus::detail::macros_get_logger(g_APIlogger.m_logger);
    if (log.isEnabledFor(logLevel))
    {
        const char* jInfo = [sInfo UTF8String];
    	const char* jFileName = [sFileName UTF8String];
        //NSData *data=[sInfo dataUsingEncoding:NSUTF8StringEncoding];
        //char* jInfo = (char *)[data bytes];
        //data = [sFileName dataUsingEncoding:NSUTF8StringEncoding];
        //char* jFileName = (char *)[data bytes];
        log4cplus::detail::macro_forced_log(log, logLevel, jInfo, jFileName, line, LOG4CPLUS_MACRO_FUNCTION());
    }
#endif
}

bool MojingSDK_API_Init(NSString* strMerchantID, NSString* strAppID, NSString* strAppKey, NSString* strChannelID)
{
#ifdef _DEBUG
    sleep(2);
#endif
    MOJING_FUNC_TRACE(g_APIlogger);
#ifdef MJ_OS_MAC
    MOJING_TRACE(g_APIlogger, "Init under MAC OS");
#endif

#ifdef MJ_OS_ANDROID
    MOJING_TRACE(g_APIlogger, "Init under ANDROID");
#endif
    
#ifdef MJ_OS_IOS
    MOJING_TRACE(g_APIlogger, "Init under iOS");
#endif
#ifdef MJ_OS_IPHONE
    MOJING_TRACE(g_APIlogger, "Init under IPHONE");
#endif
    
#ifdef MJ_OS_WIN32
    MOJING_TRACE(g_APIlogger, "Init under Windows");
#endif
    
#ifdef MJ_OS_LINUX
    MOJING_TRACE(g_APIlogger, "Init under Linux");
#endif
    
    
#ifdef MJ_OS_OTHER
    MOJING_TRACE(g_APIlogger, "Init under Unknown OS");
#endif
    
    bool bRet = false;
    
    char* Brand = (char *)GetMobileBrand();
    MOJING_TRACE(g_APIlogger, "Brand: " << Brand);
    char* Model = (char *)GetMobileInnerModelName();
    MOJING_TRACE(g_APIlogger, "Model: " << Model);
    char* Serial = (char *)GetMobileSerialNumber();
    MOJING_TRACE(g_APIlogger, "Serial: " << Serial);
    
    int nWidth = 0, nHeight = 0;
    float xdpi = 0, ydpi = 0;
    float scale = 1;
    bRet = GetMobileScreenSize(nWidth, nHeight, xdpi, ydpi, scale);
    if(!bRet)
    {
        return  false;
    }
    MOJING_TRACE(g_APIlogger, "Screen Size: " << nWidth << " X " << nHeight);
    MOJING_TRACE(g_APIlogger, "Screen DPI:  " << xdpi << " X " << ydpi);
    
    const char * szAppName = GetCurAppVersion();
    MOJING_TRACE(g_APIlogger, "AppName: " << szAppName);
    const char* szPackageName = GetCurPackageName();
    MOJING_TRACE(g_APIlogger, "PackageName: " << szPackageName);
    
    const char * szUserID = GetUserID();//GetVerdorUUID();
   
    MOJING_TRACE(g_APIlogger, "UserID: " << szUserID);
   
    const char* szMerchantID = [strMerchantID UTF8String];
    const char* szAppID = [strAppID UTF8String];
    const char* szAppKey = [strAppKey UTF8String];
    const char* szChannelID  = [strChannelID UTF8String];
#ifdef _DEBUG
	MOJING_TRACE(g_APIlogger, "MerchantID: " << szMerchantID << ", AppID: " << szAppID << ", AppKey: " << szAppKey << ", ChannelID: " << szChannelID);
#endif
    
    const char* szProfilePath = GetSDKAssetsDirPath();//GetSDKAssetsFilePath("GlassesConfig.json");
    if(szProfilePath != NULL)
        MOJING_TRACE(g_APIlogger, "ProfilePath: " << szProfilePath);
    
    bRet = MojingSDK_Init(nWidth, nHeight, xdpi, ydpi, Brand, Model, Serial, szMerchantID, szAppID, szAppKey, szAppName, szPackageName, szUserID, szChannelID, szProfilePath);
    Manager* pManager = Manager::GetMojingManager();
    if (pManager)
    {
        MojingDisplayParameters *pDisplay = pManager->GetParameters()->GetDisplayParameters();
        if (pDisplay)
            pDisplay->SetScale(scale);
    }
    
    //Register Mojing Tuner
    //MojingSDK_API_RegisterMJTuner(22, 55, 0.06f, 0.068f);
    
    return bRet;
}


bool MojingSDK_API_StartTracker(int nSampleFrequence)
{
    return MojingSDK_StartTracker(nSampleFrequence);
}

int MojingSDK_API_CheckSensors()
{
    return MojingSDK_CheckSensors();
}

bool MojingSDK_API_StartTrackerCalibration()
{
    return MojingSDK_StartTrackerCalibration() >= 0;
}

float MojingSDK_API_IsTrackerCalibrated()
{
    return MojingSDK_IsTrackerCalibrated();
}

void MojingSDK_API_ResetSensorOrientation()
{
    MojingSDK_ResetSensorOrientation();
}

void MojingSDK_API_ResetTracker()
{
    MojingSDK_ResetTracker();
}

void MojingSDK_API_getLastHeadView(float* pMatrixArray)
{
	Matrix4f Ret;
    MojingSDK_getLastHeadView((float*)&Ret.M[0][0]);
    
    // 我们预期要让摄像头旋转而不是让模型旋转，所以这里要根据手机的朝向做特殊的处理
    	// Right模式，HOME键在屏幕右侧。
    	// 这种情况下返回的坐标要沿着Z轴转180度
        // R.RotationZ(180);
        
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
    if  (pStatus->GetEngineStatus() != ENGINE_UNITY)
    {
        Ret = Matrix4f::RotationZ(PI) * Ret;
    }
    else
    {
        Ret = Matrix4f::RotationX(PI) * Ret;
        Ret = Matrix4f::RotationY(PI) * Ret;
    }
    memcpy(pMatrixArray , (float*)&Ret.M[0][0], sizeof(float) * 16);
}

void MojingSDK_API_getLastHeadEulerAngles(float* pEulerAngles)
{
    MojingSDK_getLastHeadEulerAngles(pEulerAngles);
}

void MojingSDK_API_getLastHeadQuarternion(float* pUnitQuarternion, int length)
{
    if (length < 4)
    {
        MOJING_ERROR(g_APIlogger, "The array for quarternion is too small. Need 4 while only %d" << length);
        for (int i = 0; i < length; i++)
        {
            pUnitQuarternion[i] = -1.0f;
        }
        return;
    }
    float x, y, z, w;
    MojingSDK_getLastHeadQuarternion(&w, &x, &y, &z);
    pUnitQuarternion[0] = w;
    pUnitQuarternion[1] = x;
    pUnitQuarternion[2] = y;
    pUnitQuarternion[3] = z;

}

void MojingSDK_API_StopTracker()
{
    MojingSDK_StopTracker();
}

void MojingSDK_API_SetEnableTimeWarp(bool bEnable)
{
	MojingSDK_SetEnableTimeWarp(bEnable);
}

bool MojingSDK_API_EnterMojingWorld(NSString* GlassesName, bool bEnableTimeWarp)
{
    const char * szGlassesName = [GlassesName UTF8String];
    bool ret = MojingSDK_EnterMojingWorld(szGlassesName, false, bEnableTimeWarp);
    
    return ret;
}

/*
 Metal
 */
#if (TARGET_CPU_ARM || TARGET_CPU_ARM64)
bool MojingSDK_API_EnterMojingWorld_Metal(const char * GlassesName,
                                          bool bEnableMultiThread,
                                          bool bEnableTimeWarp,
                                          id<MTLDevice> device,
                                          id<MTLCommandQueue> cmdQueue,
                                          id<MTLCommandBuffer> cmdBuffer)
{
    ENTER_MINIDUMP_FUNCTION;
    MOJING_FUNC_TRACE(g_APIlogger);
    MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
    if (!pStatus->IsMojingSDKEnbaled())
    {
        MOJING_ERROR(g_APIlogger, "EnterMojingWorld without Init SDK!");
        return false;
    }
    
    
    if (MojingSDK_ChangeMojingWorld(GlassesName))
    {
        if (pStatus->GetEngineStatus() != ENGINE_UNREAL)
        {
            MojingRenderMetal::CreateCurrentRenderMetal(false, false, nullptr, device, cmdQueue, cmdBuffer);
        }
    }
    
    return true;
}

bool MojingSDK_API_DrawTexture_Metal(id<CAMetalDrawable> drawable,
                                     id<MTLTexture> eyeTextureLeft,
                                     id<MTLTexture> eyeTextureRight,
                                     id<MTLTexture> overlayTextureLeft,
                                     id<MTLTexture> overlayTextureRight)
{
    ENTER_MINIDUMP_FUNCTION;
    // #ifdef _DEBUG
    // 	MOJING_FUNC_TRACE(g_APIlogger);
    // #endif
    MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
    if (!pStatus->IsMojingSDKEnbaled())
    {
        MOJING_ERROR(g_APIlogger, "Call DrawTexture befor Init! InitStatus = " << pStatus->GetInitStatus());
        return false;
    }
    
    Manager* pManager = Manager::GetMojingManager();
    if (pManager)
    {
        MojingRenderMetal *pRender = MojingRenderMetal::GetInstance();
        if (pRender == NULL)
        {
            MOJING_ERROR(g_APIlogger, "Render without Mojing Word!!");
            return false;
        }
        
        if (pStatus->GetVerfiyStatus() != VERIFY_OK)
        {
            // TODO, 清空资源？
            //            pRender->SetEyeTextureMetal(nullptr ,nullptr);
        }
        else
        {
            //            pRender->SetEyeTextureMetal(eyeTextureLeft, eyeTextureRight);
            //            pRender->SetOverlayTexture(overlayTextureLeft, overlayTextureRight);
        }
        
        return pRender->WarpToScreen(drawable, eyeTextureLeft, eyeTextureRight, overlayTextureLeft, overlayTextureRight);
    }
    return false;
}
#endif // Metal

float MojingSDK_API_GetMojingWorldFOV()
{
    return MojingSDK_GetFOV();
}

bool MojingSDK_API_ChangeMojingWorld(NSString* GlassesName)
{
    const char * szGlassesName = [GlassesName UTF8String];
    bool ret = MojingSDK_ChangeMojingWorld(szGlassesName);
    return ret;
}

void MojingSDK_API_SetCenterLine(int iWidth, int colR, int colG, int colB, int colA)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	MojingSDK_SetCenterLine(iWidth,  colR, colG, colB, colA);
}
	
bool MojingSDK_API_LeaveMojingWorld()
{
    return MojingSDK_LeaveMojingWorld();
}

bool MojingSDK_API_OnSurfaceChanged(int newWidth, int newHeight)
{
    MOJING_FUNC_TRACE(g_APIlogger);
    
    Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->SetScreenWidth(fmax(newWidth, newHeight));
    Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->SetScreenHeight(fmin(newWidth, newHeight));
    
    MojingRenderBase::SetModify();
    return true	;
}

int MojingSDK_API_GetEyeTexture(int eyeTextureType, int* poutParams)
{
    unsigned uiWidth, uiHeight, uiFormat;
    int Ret = MojingSDK_GetEyeTexture(eyeTextureType, uiWidth, uiHeight, uiFormat);
    
    poutParams[0] = uiWidth;
    poutParams[1] = uiHeight;
    poutParams[2] = uiFormat;

    return Ret;
}

float MojingSDK_API_GetGlassesSeparationInPix()
{
    MOJING_FUNC_TRACE(g_APIlogger);
    return MojingSDK_GetGlassesSeparationInPix();
}

bool MojingSDK_API_DrawTexture(int LeftTexID, int RightTexID, int iLeftOverlayTextureID, int iRightOverlayTextureID)
{
    return MojingSDK_DrawTexture(LeftTexID , RightTexID, iLeftOverlayTextureID , iRightOverlayTextureID);
}

void MojingSDK_API_SetOverlayPosition(float fLeft, float fTop , float fWidth , float fHeight )
{
	MojingSDK_SetOverlayPosition(fLeft, fTop, fWidth, fHeight);
}

void MojingSDK_API_SetOverlayPosition3D( int eyeTextureType, /*float fLeft, float fTop, */float fWidth, float fHeight, float fDistanceInMetre)
{
	MojingSDK_SetOverlayPosition3D(eyeTextureType, fWidth, fHeight , fDistanceInMetre);
}

void MojingSDK_API_SetOverlayPosition3D_V2(unsigned int eyeTextureType, float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre)
{
    MojingSDK_SetOverlayPosition3D_V2(eyeTextureType, fLeft, fTop, fWidth, fHeight, fDistanceInMetre);
}

void MojingSDK_API_SetImageYOffset(float fYOffset)
{
    MOJING_FUNC_TRACE(g_APIlogger);
    MOJING_TRACE(g_APIlogger, "Set YOffset = " << fYOffset);
    Manager::GetMojingManager()->GetDistortion()->SetYOffset(fYOffset);
    MojingRenderBase::SetModify();
}

bool MojingSDK_API_GetInitSDK()
{
    return MojingSDK_GetInitSDK();
}

bool MojingSDK_API_GetStartTracker()
{
    return MojingSDK_GetStartTracker();
}

bool MojingSDK_API_GetInMojingWorld()
{
    return MojingSDK_GetInMojingWorld();
}

bool MojingSDK_API_SetDefaultMojingWorld(NSString* GlassesName)
{
   return  MojingSDK_SetDefaultMojingWorld(GlassesName.UTF8String);
}

NSString*  MojingSDK_API_GetDefaultMojingWorld(NSString* strLanguageCodeByISO639)
{
    String strRet = MojingSDK_GetDefaultMojingWorld(strLanguageCodeByISO639.UTF8String);
    return [NSString stringWithUTF8String:strRet.ToCStr()];
}

NSString* MojingSDK_API_GetLastMojingWorld(NSString* strLanguageCodeByISO639)
{
    String strRet = MojingSDK_GetLastMojingWorld(strLanguageCodeByISO639.UTF8String);
    return [NSString stringWithUTF8String:strRet.ToCStr()];
}

NSString* MojingSDK_API_GetSDKVersion()
{
    const char* pVersion = MojingSDK_GetSDKVersion();
    
    return [NSString stringWithUTF8String:pVersion];
}

NSString* MojingSDK_API_GetGlasses()
{
    const char* pGlasses = MojingSDK_GetGlasses();
    
    return [NSString stringWithUTF8String:pGlasses];
}

NSString* MojingSDK_API_GetManufacturerList(NSString* strLanguageCodeByISO639)
{
    String strRet = MojingSDK_GetManufacturerList(strLanguageCodeByISO639.UTF8String);
    return [NSString stringWithUTF8String:strRet.ToCStr()];
}

NSString* MojingSDK_API_GetProductList(NSString* strManufacturerKey, NSString* strLanguageCodeByISO639)
{
    String strRet = MojingSDK_GetProductList(strManufacturerKey.UTF8String ,  strLanguageCodeByISO639.UTF8String);
    return [NSString stringWithUTF8String:strRet.ToCStr()];

}
NSString* MojingSDK_API_GetGlassList(NSString* strProductKey, NSString* strLanguageCodeByISO639)
{
    String strRet = MojingSDK_GetGlassList(strProductKey.UTF8String ,  strLanguageCodeByISO639.UTF8String);
    return [NSString stringWithUTF8String:strRet.ToCStr()];
}
NSString* MojingSDK_API_GetGlassInfo(NSString* strGlassKey, NSString* strLanguageCodeByISO639)
{
    String strRet = MojingSDK_GetGlassInfo(strGlassKey.UTF8String ,  strLanguageCodeByISO639.UTF8String);
    return [NSString stringWithUTF8String:strRet.ToCStr()];
}

NSString* MojingSDK_API_GenerationGlassKey(NSString* strProductQRCode, NSString* strGlassQRCode)
{
    String strRet = MojingSDK_GenerationGlassKey(strProductQRCode.UTF8String ,  strGlassQRCode.UTF8String);
    return [NSString stringWithUTF8String:strRet.ToCStr()];
}

// 设置和获取瞳距，以米为单位
bool MojingSDK_API_DistortionSetLensSeparation(float fNewValue)
{
    return MojingSDK_DistortionSetLensSeparation(fNewValue);
}

float MojingSDK_API_DistortionGetLensSeparation()
{
    return MojingSDK_DistortionGetLensSeparation();
}

//for reporter
void MojingSDK_API_AppSetContinueInterval(int interval)
{
    MojingSDK_AppSetContinueInterval(interval);
}

void MojingSDK_API_AppSetReportInterval(int interval)
{
    MojingSDK_AppSetReportInterval(interval);
}

void MojingSDK_API_AppSetReportImmediate(bool bImmediate)
{
    MojingSDK_AppSetReportImmediate(bImmediate);
}


void MojingSDK_API_AppResume()
{
    MojingSDK_AppResume(GetUUID());
}

void MojingSDK_API_AppPause()
{
    MojingSDK_AppPause();
}

void MojingSDK_API_AppPageStart(NSString* strPageName)
{
    MojingSDK_AppPageStart([strPageName UTF8String]);
}

void MojingSDK_API_AppPageEnd(NSString* strPageName)
{
    MojingSDK_AppPageEnd([strPageName UTF8String]);
}

void MojingSDK_API_AppSetEvent(NSString* strEventName, NSString* strEventChannelID, NSString* strEventInName, float eInData, NSString* strEventOutName, float eOutData)
{
    const char* szEventName = [strEventName UTF8String];
    const char* szEventChannelID = [strEventChannelID UTF8String];
    const char* szEventInName = [strEventInName UTF8String];
    const char* szEventOutName = [strEventOutName UTF8String];
    
    MojingSDK_AppSetEvent(szEventName, szEventChannelID, szEventInName, eInData, szEventOutName, eOutData);
}

void MojingSDK_API_ReportLog(int iLogType, NSString* strTypeName, NSString* strContent)
{
    const char* szTypeName = [strTypeName UTF8String];
    const char* szContent = [strContent UTF8String];
    
    MojingSDK_ReportLog(iLogType, szTypeName, szContent, false);
}

const char* MojingSDK_API_AppGetUserID()
{
    return MojingPlatformBase::GetPlatform()->GetUserID();
}

const char* MojingSDK_API_AppGetRunID()
{
    return MojingPlatformBase::GetPlatform()->GetRunID();
}


NSString* MojingSDK_API_GetJoystickList()
{
    JoystickProfile *pProfile = Manager::GetMojingManager()->GetParameters()->GetJoystickProfile();
    String szRetJson = pProfile->GetJsonProfile();
    return [NSString stringWithUTF8String:szRetJson.ToCStr()];
}
bool MojingSDK_API_SetJoystick(int iID)
{
    JoystickProfile *pProfile = Manager::GetMojingManager()->GetParameters()->GetJoystickProfile();
    String szKeys = pProfile->GetJoystickKeys( iID);
    if (szKeys.GetLength() > 0 && (szKeys.GetLength() %2) == 0)
    {// to do
        NSString *hanelName = [NSString stringWithUTF8String:szKeys.ToCStr()];
        bool ret = [[MJiCadeTransfor shareIcadeTransfor] dicFromString:hanelName];
        return ret;
    }
    return false;
}



#ifdef _DEBUG
/************************************************************************/
/* 以下为私有代码，给SDK用的代码放到前面去			                    */
/************************************************************************/
// 设置和获取K值，Get函数的返回值为畸变引导点的数量
bool MojingSDKPrivate_API_DistortionSetParamet(int iSegment, float* pKR, float* pKG, float* pKB)
{
    bool bRet = MojingSDKPrivate_DistortionSetParamet(iSegment, pKR, pKG, pKB);
    return bRet ;
}

int MojingSDKPrivate_API_DistortionGetParamet(float* pKR, float* pKG, float* pKB)
{
    int iRet = MojingSDKPrivate_DistortionGetParamet(pKR, pKG, pKB);
    
    return iRet;
}

bool MojingSDKPrivate_API_DistortionSaveParamet()
{
    return MojingSDKPrivate_DistortionSaveParamet();
}

bool MojingSDKPrivate_API_DistortionResetParamet()
{
    return MojingSDKPrivate_DistortionResetParamet();
}

void MojingSDKPrivate_API_FuncTest()
{
    MOJING_FUNC_TRACE(g_APIlogger);
    
    Manager* pManager = Manager::GetMojingManager();
    if (pManager)
    {
        AES A((unsigned char*)"1234567980abcdef");
        char S1[] = "1234567980abcdef";
        char S2[] = "1234567980abcdef";
        A.Cipher(S1 , 16);
        A.Cipher(S2, 16);
        int iV = memcmp(S1 ,S2 , 16);
        
        MOJING_TRACE(g_APIlogger, "MojingSDKPrivate FuncTest, value = " << iV );
    }
}

/************************************************************************/
/* 以上为私有代码，给SDK用的代码放到前面去			                    */
/************************************************************************/
#endif
@end
