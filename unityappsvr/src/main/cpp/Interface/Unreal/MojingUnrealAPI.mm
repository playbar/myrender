#import "MojingIOSBase.h"
#import "MojingGamepad.h"

#include "../../MojingManager.h"
#include "../../Parameters/MojingParameters.h"
#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

#include "MojingUnrealAPI.cpp"

typedef void(*FP_OnMojingKeyEventCB)(int32_t deviceId, int32_t buttonId, bool buttonDown);
typedef void(*FP_OnMojingAxisEventCB)(int32_t deviceId, int32_t axisId, float axisValue);
FP_OnMojingKeyEventCB g_fpOnMojingKeyEventCB = NULL;
FP_OnMojingAxisEventCB g_fpOnMojingAxisEventCB = NULL;

typedef void(*FP_OniCadeKeyEventCB)(int32_t deviceId, int32_t buttonId, bool buttonDown);
FP_OniCadeKeyEventCB g_fpOniCadeKeyEventCB = NULL;

static void Unreal_RegisterAllGamepad()
{
    [MojingGamepad sharedGamepad].buttonValueChangedHandler = ^void(NSString* peripheralName, AXIS_GAMEPAD axisID, KEY_GAMEPAD keyID, BOOL pressed){
        
        if(peripheralName == nil)
        {
            return;
        }
#ifdef _DEBUG
        if(pressed)
        {
            MOJING_TRACE(g_APIlogger, [peripheralName UTF8String] << " key is pressed. KeyID = " << keyID);
        }
#endif
        if([peripheralName isEqualToString:@"iCade"])
        {
            if(g_fpOniCadeKeyEventCB)
            {
                g_fpOniCadeKeyEventCB(0, keyID, pressed);
            }
            else
            {
                MOJING_ERROR(g_APIlogger, "g_fpOniCadeKeyEventCB is null");
            }
        }
        else
        {
            if(g_fpOnMojingKeyEventCB)
            {
                g_fpOnMojingKeyEventCB(0, keyID, pressed);
            }
            else
            {
                MOJING_ERROR(g_APIlogger, "g_fpOnMojingKeyEventCB is null");
            }
        }
    };
    
    [MojingGamepad sharedGamepad].axisValueChangedHandler = ^void(NSString* peripheralName, AXIS_GAMEPAD axisID, float xValue, float yValue)
    {
        if(g_fpOnMojingAxisEventCB)
        {
            g_fpOnMojingAxisEventCB(0, 0, xValue);
            g_fpOnMojingAxisEventCB(0, 1, -yValue);
        }
        else
        {
            MOJING_ERROR(g_APIlogger, "g_fpOnMojingAxisEventCB is null");
        }
    };
    
    [[MojingGamepad sharedGamepad] registerGamepad];
    [[MojingGamepad sharedGamepad] setAutoScan:YES];
    
    MOJING_TRACE(g_APIlogger, "Register all gamepad.");
}

bool Unreal_Init(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szChannelID)
{
    MOJING_FUNC_TRACE(g_APIlogger);
    MOJING_TRACE(g_APIlogger, "Init under iOS...");
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
    bRet = GetMobileScreenSize(nWidth, nHeight, xdpi, ydpi, scale , false);
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
    
    const char * szUserID = GetVerdorUUID();
    MOJING_TRACE(g_APIlogger, "UserID: " << szUserID);
    
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
    
    Unreal_RegisterAllGamepad();
    
    return bRet;
}


