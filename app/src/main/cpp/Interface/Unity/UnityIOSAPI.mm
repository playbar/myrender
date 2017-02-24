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
// #include "UnityPlugin.cpp"
/*+++++++++++++++++++++++++++++++++++++++++++++++*/


extern "C" void UnitySendMessage(const char* obj, const char* method, const char* msg);
int g_GamePad_AxisMode = GAMEPAD_AXIS_DIRECTION;

#define LOG_LEVEL_ALL       0
#define LOG_LEVEL_TRACE     LOG_LEVEL_ALL
#define LOG_LEVEL_DEBUG     1000
#define LOG_LEVEL_INFO      2000
#define LOG_LEVEL_WARN      3000
#define LOG_LEVEL_ERROR     4000
#define LOG_LEVEL_FATAL     5000
#define LOG_LEVEL_OFF       6000
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