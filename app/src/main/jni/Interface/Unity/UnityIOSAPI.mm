//
//  UnityIOSAPI.cpp
//  MojingSDK
//
//  Created by 汤申亮 on 15/8/13.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MojingIOSBase.h"
#import "MJGamepad.h"
#import "MJTuner.h"
#import "UnityIOSAPI.h"
//#import "MJiCadePad.h"
#import "MJiCadeTransfor.h"
#import "MJGameController.h"
#import "MojingGamepad.h"

#include "UnityIOSAPI.h"
#include "../iOS/MojingIOSAPI.h"
/*+++++ most interface define in UnityPlugin.cpp +++++*/
#include "UnityPlugin.cpp"
/*+++++++++++++++++++++++++++++++++++++++++++++++*/



extern "C"
{
#define GAMEPAD_AXIS_DIRECTION   0
#define GAMEPAD_AXIS_POSITION    1
#define GAMEPAD_AXIS_ALL         2
    
int g_GamePad_AxisMode = GAMEPAD_AXIS_DIRECTION;
    
//void  Unity_RegisterMJGamePad();
//void  Unity_RegisterMFIGamePad();
//void  Unity_RegisterMJTuner(int minScale, int maxScale, float minLensSeparation, float maxLensSeparation);
    
UNITY_EXPORT bool Unity_Init(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szChannelID)
{
#ifdef _DEBUG
    sleep(2);
#endif
    
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
    bRet = GetMobileScreenSize(nWidth, nHeight, xdpi, ydpi, scale , true);
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
    
    g_GamePad_AxisMode = GAMEPAD_AXIS_DIRECTION;
    
    /* 改由开发者自己调用ResigterGamepad()
    //Register Mojing GamePad("Mojing4")
    Unity_RegisterMJGamePad();
    // Register MFI GameController
    Unity_RegisterMFIGamePad();
    */
    
    // Register Mojing Tuner
    //Unity_RegisterMJTuner(22, 55, 0.06f, 0.068f);
    
    return bRet;
}
 
#if 0
UNITY_EXPORT bool Unity_OnSurfaceChanged(int newWidth, int newHeight)
{
    MOJING_FUNC_TRACE(g_APIlogger);
    
    Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->SetScreenWidth(fmax(newWidth, newHeight));
    Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->SetScreenHeight(fmin(newWidth, newHeight));
    
    MojingRenderBase::SetModify();
    return true	;
}
#endif
    
    
UNITY_EXPORT void UnitySetGraphicsDevice (void* device, int deviceType, int eventType)
{
    MOJING_FUNC_TRACE(g_APIlogger);    
}
    
UNITY_EXPORT void Unity_AppResume()
{
    MojingSDK_AppResume(GetUUID());
}
    
UNITY_EXPORT void Unity_AppPause()
{
	MojingSDK_AppPause();
}

UNITY_EXPORT    void Unity_AppExit(int iExitCode)
{
    exit(iExitCode);
}
    
UNITY_EXPORT void Unity_SetGamePadAxisMode(int mode)
{
    MOJING_FUNC_TRACE(g_APIlogger);
    g_GamePad_AxisMode = mode;
}
    
extern void UnitySendMessage(const char* obj, const char* method, const char* msg);
    
static void  Log_OC(int logLevel, NSString* info, NSString* filename, int line)
{
#ifdef _DEBUG
#ifdef ENABLE_LOGGER
    log4cplus::Logger const & log = log4cplus::detail::macros_get_logger(g_APIlogger.m_logger);
    if (log.isEnabledFor(logLevel))
    {
        const char* jInfo = [info UTF8String];
        const char* jFileName = [filename UTF8String];
        log4cplus::detail::macro_forced_log(log, logLevel, jInfo, jFileName, line, LOG4CPLUS_MACRO_FUNCTION());
    }
#endif
#endif
}
 
    UNITY_EXPORT void Unity_getLastHeadView(float* pfViewMatrix)
    {
        MojingSDK_API_getLastHeadView(pfViewMatrix);
	}

/*
void  Unity_RegisterMJGamePad()
{
    Log_OC(LOG_LEVEL_TRACE, @"RegisterMJGamePad Enter", [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    
    //System Bluetooth is enabled or not
    [MJGamepad sharedMJGamepad].btnCBCMEnabled.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
        if(pressed)
        {
            UnitySendMessage("MojingInputManager", "onBluetoothAdapterStateChanged", "12");
        }
        else
        {
            UnitySendMessage("MojingInputManager", "onBluetoothAdapterStateChanged", "10");
        }
        
        NSLog(@"BluetoothAdapter is%@", pressed? @"Enabed":@"Disabled");
        NSString *str = [NSString stringWithFormat:@"BluetoothAdapter is%@", pressed? @"Enabed":@"Disabled"];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    //GamePad Connect state
    [MJGamepad sharedMJGamepad].btnPadConnect.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
        NSString *name = [MJGamepad sharedMJGamepad].peripheral.name;
        if(pressed)
        {
            UnitySendMessage("MojingInputManager", "onDeviceAttached", [name UTF8String]);
        }
        else
        {
            UnitySendMessage("MojingInputManager", "onDeviceDetached", [name UTF8String]);
        }
        
        NSLog(@"Game Pad %@", pressed? @"connected":@"disconnected");
        NSString *str = [NSString stringWithFormat:@"Game Pad %@", pressed? @"connected":@"disconnected"];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    [MJGamepad sharedMJGamepad].buttonStart.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
        char msg[128];
        NSString *name = [MJGamepad sharedMJGamepad].peripheral.name;
        sprintf(msg, "%s/%d", [name UTF8String], KEY_OK);
        if(pressed)
        {
            UnitySendMessage("MojingInputManager", "OnButtonDown", msg);
        }
        else
        {
            UnitySendMessage("MojingInputManager", "OnButtonUp", msg);
        }
        
        NSLog(@"开始键%@", pressed? @"按下":@"松开");
        NSString *str = [NSString stringWithFormat:@"开始键%@", pressed? @"按下":@"松开"];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    [MJGamepad sharedMJGamepad].buttonBack.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
        char msg[128];
        NSString *name = [MJGamepad sharedMJGamepad].peripheral.name;
        sprintf(msg, "%s/%d", [name UTF8String], KEY_BACK);
        if(pressed)
        {
            UnitySendMessage("MojingInputManager", "OnButtonDown", msg);
        }
        else
        {
            UnitySendMessage("MojingInputManager", "OnButtonUp", msg);
        }
        
        NSLog(@"返回键%@", pressed? @"按下":@"松开");
        NSString *str = [NSString stringWithFormat:@"返回键%@", pressed? @"按下":@"松开"];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    //菜单
    [MJGamepad sharedMJGamepad].buttonMenu.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
        char msg[128];
        NSString *name = [MJGamepad sharedMJGamepad].peripheral.name;
        sprintf(msg, "%s/%d", [name UTF8String], KEY_MENU);
        if(pressed)
        {
            UnitySendMessage("MojingInputManager", "OnButtonDown", msg);
        }
        else
        {
            UnitySendMessage("MojingInputManager", "OnButtonUp", msg);
        }
        
        NSLog(@"菜单键%@", pressed? @"按下":@"松开");
        NSString *str = [NSString stringWithFormat:@"菜单键%@", pressed? @"按下":@"松开"];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    

    //摇杆
    [MJGamepad sharedMJGamepad].thumbStick.valueChangedHandler = ^void(MJGamepadThumbStick *stick, float x, float y){
        if(g_GamePad_AxisMode != GAMEPAD_AXIS_POSITION && g_GamePad_AxisMode != GAMEPAD_AXIS_ALL)
        {
            return;
        }
        
        char msg[128];
        NSString *name = [MJGamepad sharedMJGamepad].peripheral.name;
        sprintf(msg, "%s/%d/%d,%d,0", [name UTF8String], AXIS_DPAD, int(x*MAX_AXIS_VALUE), int(y*MAX_AXIS_VALUE));
        UnitySendMessage("MojingInputManager", "onMove", msg);
        
        NSLog(@"摇杆坐标: (%f, %f), 长度:%f", x, y, stick.offsetToCenter);
        //NSString *str = [NSString stringWithFormat:@"摇杆坐标: (%f, %f), 长度:%f", x, y, stick.offsetToCenter];
        //Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    //十字键
    [MJGamepad sharedMJGamepad].vdpad.valueChangedHandler = ^void(MJGamepadDirectionPad *dpad){
        if(g_GamePad_AxisMode != GAMEPAD_AXIS_DIRECTION && g_GamePad_AxisMode != GAMEPAD_AXIS_ALL)
        {
            return;
        }
        static int keyLasted = 0;
        mj_direction d = dpad.direction;
        char msg[128];
        NSString *name = [MJGamepad sharedMJGamepad].peripheral.name;
        const char *padName = [name UTF8String];
        
        if(d.all_flags == 0)
        {
            if(keyLasted > 0)
            {
                sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD,keyLasted);
                UnitySendMessage("MojingInputManager", "OnButtonUp", msg);
                keyLasted = 0;
            }
        }
        else
        {
            if(d.up)
            {
                switch (keyLasted) {
                    case KEY_DOWN:
                    case KEY_LEFT:
                    case KEY_RIGHT:
                    case KEY_CENTER:
                        sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD, keyLasted);
                        UnitySendMessage("MojingInputManager", "OnButtonUp", msg);
                        break;
                    default:
                        break;
                }
                keyLasted = KEY_UP;   
                sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD, keyLasted);
                UnitySendMessage("MojingInputManager", "OnButtonDown", msg);
            }
            if(d.down)
            {
                switch (keyLasted) {
                    case KEY_UP:
                    case KEY_LEFT:
                    case KEY_RIGHT:
                    case KEY_CENTER:
                        sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD, keyLasted);
                        UnitySendMessage("MojingInputManager", "OnButtonUp", msg);
                        break;
                    default:
                        break;
                }
                keyLasted = KEY_DOWN;
                sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD, keyLasted);
                UnitySendMessage("MojingInputManager", "OnButtonDown", msg);
            }
            if(d.left)
            {
                switch (keyLasted) {
                    case KEY_UP:
                    case KEY_DOWN:
                    case KEY_RIGHT:
                    case KEY_CENTER:
                        sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD, keyLasted);
                        UnitySendMessage("MojingInputManager", "OnButtonUp", msg);
                        break;
                    default:
                        break;
                }
                keyLasted = KEY_LEFT;
                sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD, keyLasted);
                UnitySendMessage("MojingInputManager", "OnButtonDown", msg);
            }
            if(d.right)
            {
                switch (keyLasted) {
                    case KEY_UP:
                    case KEY_DOWN:
                    case KEY_LEFT:
                    case KEY_CENTER:
                        sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD, keyLasted);
                        UnitySendMessage("MojingInputManager", "OnButtonUp", msg);
                        break;
                    default:
                        break;
                }
                keyLasted = KEY_RIGHT;
                sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD, keyLasted);
                UnitySendMessage("MojingInputManager", "OnButtonDown", msg);

            }
            if(d.center)
            {
                switch (keyLasted) {
                    case KEY_UP:
                    case KEY_DOWN:
                    case KEY_LEFT:
                    case KEY_RIGHT:
                        sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD, keyLasted);
                        UnitySendMessage("MojingInputManager", "OnButtonUp", msg);
                        break;
                    default:
                        break;
                }
                keyLasted = KEY_CENTER;
                sprintf(msg, "%s/%d/%d", padName, AXIS_DPAD, keyLasted);
                UnitySendMessage("MojingInputManager", "OnButtonDown", msg);
            }
        }

        NSLog(@"十字键: (上:%d 下:%d 左:%d 右:%d 中:%d)", d.up, d.down, d.left, d.right, d.center);
        NSString *str = [NSString stringWithFormat:@"十字键: (上:%d 下:%d 左:%d 右:%d 中:%d)", d.up, d.down, d.left, d.right, d.center];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    [[MJGamepad sharedMJGamepad] scan];
}
    
    void  Unity_RegisterMJTuner(int minScale, int maxScale, float minLensSeparation, float maxLensSeparation)
    {
        NSString *str = [NSString stringWithFormat:@"RegisterMJTuner Enter, Scale(%d ~ %d), LensSeparation(%f ~ %f)", minScale, maxScale, minLensSeparation, maxLensSeparation];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        
        //Tuner Connect state
        [MJTuner sharedMJTuner].btnConnect.valueChangedHandler = ^void(MJTunerButton *button, BOOL pressed, float value){

            NSLog(@"Mojing Tuner %@", pressed? @"connected":@"disconnected");
            NSString *str = [NSString stringWithFormat:@"Mojing Tuner %@", pressed? @"connected":@"disconnected"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        //Menu
        [MJTuner sharedMJTuner].buttonMenu.valueChangedHandler = ^void(MJTunerButton *button, BOOL pressed, float value){
            NSLog(@"菜单键%@", pressed? @"按下":@"松开");
            NSString *str = [NSString stringWithFormat:@"菜单键%@", pressed? @"按下":@"松开"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        //Thumb turn
        [MJTuner sharedMJTuner].thumbTurn.valueChangedHandler = ^void(MJTunerThumb *thumbTurn, uint8_t value){
            float fLensSeparation = minLensSeparation+(maxLensSeparation-minLensSeparation)*(value-minScale)/(maxScale-minScale);
            MojingSDK_DistortionSetLensSeparation(fLensSeparation);
            
            NSLog(@"Tuner thumb value: (%d), LensSeparation = %f", value, fLensSeparation);
            NSString *str = [NSString stringWithFormat:@"Tuner thumb value: (%d), LensSeparation = %f", value, fLensSeparation];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
    }
    
    //MFI Game Controller
    void  Unity_RegisterMFIGamePad()
    {
        Log_OC(LOG_LEVEL_TRACE, @"Unity_RegisterMFIGamePad Enter", [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        
        [MJGameController sharedMFIGamepad].buttonValueChangedHandler = ^void(AXIS_GAMEPAD axisID, KEY_GAMEPAD keyID, BOOL pressed){
            char msg[128];
            if(axisID == AXIS_NONE)
            {
                if(keyID == KEY_CONNECT)
                {
                    if(pressed)
                    {
                        UnitySendMessage("MojingInputManager", "onDeviceAttached", "MFI");
                    }
                    else
                    {
                        UnitySendMessage("MojingInputManager", "onDeviceDetached", "MFI");
                    }
                    return;
                }
                else
                {
                	sprintf(msg, "%s/%d", "MFI", keyID);
                }
            }
            else
            {
                if(g_GamePad_AxisMode != GAMEPAD_AXIS_DIRECTION && g_GamePad_AxisMode != GAMEPAD_AXIS_ALL)
                {
                    return;
                }
              	sprintf(msg, "%s/%d/%d", "MFI", axisID, keyID);
            }
            UnitySendMessage("MojingInputManager", pressed?"OnButtonDown":"OnButtonUp", msg);
            
            NSLog(@"按键[ID=%d/%d]%@", axisID, keyID, pressed? @"按下":@"松开");
            NSString *str = [NSString stringWithFormat:@"按键[ID=%d/%d]%@", axisID, keyID, pressed? @"按下":@"松开"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        [MJGameController sharedMFIGamepad].axisValueChangedHandler = ^void(AXIS_GAMEPAD axisID, float xValue, float yValue)
        {
            if(g_GamePad_AxisMode != GAMEPAD_AXIS_POSITION && g_GamePad_AxisMode != GAMEPAD_AXIS_ALL)
            {
                return;
            }
            char msg[128];
            sprintf(msg, "%s/%d/%d,%d,0", "MFI", axisID, int(xValue*MAX_AXIS_VALUE), int(yValue*MAX_AXIS_VALUE));
            UnitySendMessage("MojingInputManager", "onMove", msg);
            
            NSLog(@"摇杆[Pad ID=%d]坐标: (%f, %f)", axisID, xValue, yValue);
            NSString *str = [NSString stringWithFormat:@"摇杆[Pad ID=%d]坐标: (%f, %f)", axisID, xValue, yValue];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        [[MJGameController sharedMFIGamepad] registerKey];
    }
*/
/*
    void Unity_RegisterAllGamepad()
    {
        Log_OC(LOG_LEVEL_TRACE, @"Unity_ResigterAllGamePad Enter", [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        
        [MojingGamepad sharedGamepad].buttonValueChangedHandler = ^void(NSString* peripheralName, AXIS_GAMEPAD axisID, KEY_GAMEPAD keyID, BOOL pressed){
            char msg[128];
            if(axisID == AXIS_NONE)
            {
                if(keyID == KEY_CONNECT)
                {
                    if(pressed)
                    {
                        UnitySendMessage("MojingInputManager", "onDeviceAttached", [peripheralName UTF8String]);
                    }
                    else
                    {
                        UnitySendMessage("MojingInputManager", "onDeviceDetached", [peripheralName UTF8String]);
                    }
                    return;
                }
                else if(keyID == KEY_BLUETOOTH)
                {
                    if(pressed)
                    {
                        UnitySendMessage("MojingInputManager", "onBluetoothAdapterStateChanged", "12");
                    }
                    else
                    {
                        UnitySendMessage("MojingInputManager", "onBluetoothAdapterStateChanged", "10");
                    }
                    return;
                }
                else
                {
                    sprintf(msg, "%s/%d", [peripheralName UTF8String], keyID);
                }
            }
            else
            {
                if(g_GamePad_AxisMode != GAMEPAD_AXIS_DIRECTION && g_GamePad_AxisMode != GAMEPAD_AXIS_ALL)
                {
                    return;
                }
                sprintf(msg, "%s/%d/%d", [peripheralName UTF8String], axisID, keyID);
            }
            UnitySendMessage("MojingInputManager", pressed?"OnButtonDown":"OnButtonUp", msg);
            
            //NSLog(@"按键[ID=%d/%d]%@", axisID, keyID, pressed? @"按下":@"松开");
            //NSString *str = [NSString stringWithFormat:@"按键[ID=%d/%d]%@", axisID, keyID, pressed? @"按下":@"松开"];
            //Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        [MojingGamepad sharedGamepad].axisValueChangedHandler = ^void(NSString* peripheralName, AXIS_GAMEPAD axisID, float xValue, float yValue)
        {
            if(g_GamePad_AxisMode != GAMEPAD_AXIS_POSITION && g_GamePad_AxisMode != GAMEPAD_AXIS_ALL)
            {
                return;
            }
            char msg[128];
            sprintf(msg, "%s/%d/%d,%d,0", [peripheralName UTF8String], axisID, int(xValue*MAX_AXIS_VALUE), int(yValue*MAX_AXIS_VALUE));
            UnitySendMessage("MojingInputManager", "onMove", msg);
            
            //NSLog(@"摇杆[Pad ID=%d]坐标: (%f, %f)", axisID, xValue, yValue);
            //NSString *str = [NSString stringWithFormat:@"摇杆[Pad ID=%d]坐标: (%f, %f)", axisID, xValue, yValue];
            //Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        [[MojingGamepad sharedGamepad] registerGamepad];
        [[MojingGamepad sharedGamepad] setAutoScan:YES];
    }
 */
}

@implementation Unity_MojingSDK
void Unity_RegisteriCadeGamepad(UIViewController* viewCon)
{
    Log_OC(LOG_LEVEL_TRACE, @"Unity_ResigteriCadeGamePad Enter", [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    
    [[MJiCadeTransfor shareIcadeTransfor] registerKey:viewCon];
    
    [MJiCadeTransfor shareIcadeTransfor].valueChangedHandler = ^void(KEY_GAMEPAD keyID, BOOL pressed){
        char msg[128];
        sprintf(msg, "%s/%d", "iCade", keyID);
        UnitySendMessage("MojingInputManager", pressed?"OnButtonDown":"OnButtonUp", msg);
        
        NSLog(@"按键[ID=%d]%@", keyID, pressed? @"按下":@"松开");
        NSString *str = [NSString stringWithFormat:@"按键[ID=%d]%@", keyID, pressed? @"按下":@"松开"];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
}

void Unity_RegisterAllGamepad(UIViewController* viewCon)
{
    Log_OC(LOG_LEVEL_TRACE, @"Unity_ResigterAllGamePad Enter", [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    
    [MojingGamepad sharedGamepad].buttonValueChangedHandler = ^void(NSString* peripheralName, AXIS_GAMEPAD axisID, KEY_GAMEPAD keyID, BOOL pressed){
        char msg[128];
        if(axisID == AXIS_NONE)
        {
            if(keyID == KEY_CONNECT)
            {
                if(pressed)
                {
                    UnitySendMessage("MojingInputManager", "onDeviceAttached", [peripheralName UTF8String]);
                }
                else
                {
                    UnitySendMessage("MojingInputManager", "onDeviceDetached", [peripheralName UTF8String]);
                }
                return;
            }
            else if(keyID == KEY_BLUETOOTH)
            {
                if(pressed)
                {
                    UnitySendMessage("MojingInputManager", "onBluetoothAdapterStateChanged", "12");
                }
                else
                {
                    UnitySendMessage("MojingInputManager", "onBluetoothAdapterStateChanged", "10");
                }
                return;
            }
            else
            {
                sprintf(msg, "%s/%d", [peripheralName UTF8String], keyID);
            }
        }
        else
        {
            if(g_GamePad_AxisMode != GAMEPAD_AXIS_DIRECTION && g_GamePad_AxisMode != GAMEPAD_AXIS_ALL)
            {
                return;
            }
            sprintf(msg, "%s/%d/%d", [peripheralName UTF8String], axisID, keyID);
        }
        UnitySendMessage("MojingInputManager", pressed?"OnButtonDown":"OnButtonUp", msg);
        
        //NSLog(@"按键[ID=%d/%d]%@", axisID, keyID, pressed? @"按下":@"松开");
        //NSString *str = [NSString stringWithFormat:@"按键[ID=%d/%d]%@", axisID, keyID, pressed? @"按下":@"松开"];
        //Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    [MojingGamepad sharedGamepad].axisValueChangedHandler = ^void(NSString* peripheralName, AXIS_GAMEPAD axisID, float xValue, float yValue)
    {
        if(g_GamePad_AxisMode != GAMEPAD_AXIS_POSITION && g_GamePad_AxisMode != GAMEPAD_AXIS_ALL)
        {
            return;
        }
        char msg[128];
        sprintf(msg, "%s/%d/%d,%d,0", [peripheralName UTF8String], axisID, int(xValue*MAX_AXIS_VALUE), int(yValue*MAX_AXIS_VALUE));
        UnitySendMessage("MojingInputManager", "onMove", msg);
        
        //NSLog(@"摇杆[Pad ID=%d]坐标: (%f, %f)", axisID, xValue, yValue);
        //NSString *str = [NSString stringWithFormat:@"摇杆[Pad ID=%d]坐标: (%f, %f)", axisID, xValue, yValue];
        //Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    [[MojingGamepad sharedGamepad] registerGamepad:viewCon];
    [[MojingGamepad sharedGamepad] setAutoScan:YES];
}
@end