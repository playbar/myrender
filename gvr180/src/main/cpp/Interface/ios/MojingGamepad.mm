//
//  MojingGamePad.m
//  MojingSDK
//
//  Created by 汤申亮 on 15/10/13.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//


#import "MojingGamepad.h"
#import "CWLSynthesizeSingleton.h"
#import "MJGamepad/MJGamepad.h"
#import "iCade/MJiCadeTransfor.h"
//#import "iCade/MJiCadePad.h"
#import "MJGameController/MJGameController.h"
#import "MJAudioSession.h"

#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

extern MojingLogger g_APIlogger;

@interface MojingGamepad()
{
    MJGamepad* mjPad;
//    MJiCadeGamePad* iCadePad;
    MJiCadeTransfor* iCadePad;
    MJGameController* mfiPad;
    MJAudioSession *_audioSession;
}
@end

@implementation MojingGamepad
@synthesize buttonValueChangedHandler = _buttonValueChangedHandler;
@synthesize axisValueChangedHandler = _axisValueChangedHandler;
@synthesize sensorDataChangedHandler = _sensorDataChangedHandler;

CWL_SYNTHESIZE_SINGLETON_FOR_CLASS(MojingGamepad)

#pragma mark - init

+ (instancetype)sharedGamepad{
    static MojingGamepad *share=nil;
    
    if (!share) {
        share=[[MojingGamepad alloc] init];
    }
    return share;
}

/*
- (void)dealloc
{
    [_buttonValueChangedHandler release];
    [_axisValueChangedHandler release];
    [mjPad release];
    [iCadePad release];
    [mfiPad release];
    
    [super dealloc];
}
*/

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        mjPad = [MJGamepad sharedMJGamepad];
//        iCadePad = [[MJiCadeGamePad alloc] init];
        iCadePad = [MJiCadeTransfor shareIcadeTransfor];
        mfiPad = [MJGameController sharedMFIGamepad];
//        _audioSession = [MJAudioSession sharedManager];
    }
    return self;
}

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

- (void)registerMJGamepad
{
    if(mjPad)
    {
        Log_OC(LOG_LEVEL_TRACE, @"registerMJGamepad Enter", [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        
        //System Bluetooth is enabled or not
        
        __block MojingGamepad *blockMjPad = [MojingGamepad sharedGamepad];
        
        mjPad.btnCBCMEnabled.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
            if(blockMjPad.buttonValueChangedHandler)
            {
                blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_NONE, KEY_BLUETOOTH, pressed);
            }
            
            NSLog(@"BluetoothAdapter is%@", pressed? @"Enabed":@"Disabled");
            NSString *str = [NSString stringWithFormat:@"BluetoothAdapter is%@", pressed? @"Enabed":@"Disabled"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        //GamePad Connect state
        mjPad.btnPadConnect.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
            if(blockMjPad.buttonValueChangedHandler)
            {
                blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_NONE, KEY_CONNECT, pressed);
            }
            
            NSLog(@"Game Pad %@", pressed? @"connected":@"disconnected");
            NSString *str = [NSString stringWithFormat:@"Game Pad %@", pressed? @"connected":@"disconnected"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        mjPad.buttonStart.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
            if(blockMjPad.buttonValueChangedHandler)
            {
                blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_NONE, KEY_OK, pressed);
            }
            
            NSLog(@"开始键%@", pressed? @"按下":@"松开");
            NSString *str = [NSString stringWithFormat:@"开始键%@", pressed? @"按下":@"松开"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        mjPad.buttonBack.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
            if(blockMjPad.buttonValueChangedHandler)
            {
                blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_NONE, KEY_BACK, pressed);
            }
            
            NSLog(@"返回键%@", pressed? @"按下":@"松开");
            NSString *str = [NSString stringWithFormat:@"返回键%@", pressed? @"按下":@"松开"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        //菜单
        mjPad.buttonMenu.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
            if(blockMjPad.buttonValueChangedHandler)
            {
                blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_NONE, KEY_MENU, pressed);
            }
            
            NSLog(@"菜单键%@", pressed? @"按下":@"松开");
            NSString *str = [NSString stringWithFormat:@"菜单键%@", pressed? @"按下":@"松开"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        //Home
        mjPad.buttonHome.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
            if(blockMjPad.buttonValueChangedHandler)
            {
                blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_NONE, KEY_HOME, pressed);
            }
            
            NSLog(@"HOME键%@", pressed? @"按下":@"松开");
            NSString *str = [NSString stringWithFormat:@"HOME键%@", pressed? @"按下":@"松开"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
       
        //Click
        mjPad.buttonClick.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
            if(blockMjPad.buttonValueChangedHandler)
            {
                blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_NONE, BUTTON_A, pressed);
            }
            
            NSLog(@"Click键%@", pressed? @"按下":@"松开");
            NSString *str = [NSString stringWithFormat:@"Click键%@", pressed? @"按下":@"松开"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
       
        //音量加
        mjPad.btnVolumeUp.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
            if(blockMjPad.buttonValueChangedHandler)
            {
                blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_NONE, KEY_VOLUMEUP, pressed);
            }
            
            NSLog(@"音量加键%@", pressed? @"按下":@"松开");
            NSString *str = [NSString stringWithFormat:@"音量加键%@", pressed? @"按下":@"松开"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        //音量减
        mjPad.btnVolumeDown.valueChangedHandler = ^void(MJGamepadButton *button, BOOL pressed, float value){
            if(blockMjPad.buttonValueChangedHandler)
            {
                blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_NONE, KEY_VOLUMEDOWN, pressed);
            }
            
            NSLog(@"音量减键%@", pressed? @"按下":@"松开");
            NSString *str = [NSString stringWithFormat:@"音量减键%@", pressed? @"按下":@"松开"];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        //Sensor
        mjPad.sensorStick.valueChangedHandler = ^void(MJGamepadSensor *sensor, NSMutableDictionary *dicOrientation, NSMutableDictionary *dicAccel, NSMutableDictionary *dicGyro, double timestamp){
            if(blockMjPad.sensorDataChangedHandler)
            {
                blockMjPad.sensorDataChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_NONE, dicOrientation, dicAccel, dicGyro, timestamp);
            }
            
            //NSLog(@"Orientation: %@", dicOrientation);
            //NSLog(@"dicAccel: %@", dicAccel);
            //NSLog(@"dicGyro: %@", dicGyro);
            //NSLog(@"timestamp: %f", timestamp);
            //NSString *str = [NSString stringWithFormat:@"音量减键%@", pressed? @"按下":@"松开"];
            //Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        //摇杆
        mjPad.thumbStick.valueChangedHandler = ^void(MJGamepadThumbStick *stick, float x, float y){
            if(blockMjPad.axisValueChangedHandler)
            {
                blockMjPad.axisValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, x, y);
            }
            
            NSLog(@"摇杆坐标: (%f, %f), 长度:%f", x, y, stick.offsetToCenter);
            //NSString *str = [NSString stringWithFormat:@"摇杆坐标: (%f, %f), 长度:%f", x, y, stick.offsetToCenter];
            //Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        //十字键
        mjPad.vdpad.valueChangedHandler = ^void(MJGamepadDirectionPad *dpad){
            static int keyLasted = 0;
            mj_direction d = dpad.direction;

            if(_buttonValueChangedHandler == nil)
                return;
            
            if(d.all_flags == 0)
            {
                if(keyLasted > 0)
                {
                    blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, (KEY_GAMEPAD)keyLasted, NO);
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
                   			 blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, (KEY_GAMEPAD)keyLasted, NO);
                            break;
                        default:
                            break;
                    }
                    keyLasted = KEY_UP;
                    blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, KEY_UP, YES);
                }
                if(d.down)
                {
                    switch (keyLasted) {
                        case KEY_UP:
                        case KEY_LEFT:
                        case KEY_RIGHT:
                        case KEY_CENTER:
                            blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, (KEY_GAMEPAD)keyLasted, NO);
                            break;
                        default:
                            break;
                    }
                    keyLasted = KEY_DOWN;
                    blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, KEY_DOWN, YES);
                }
                if(d.left)
                {
                    switch (keyLasted) {
                        case KEY_UP:
                        case KEY_DOWN:
                        case KEY_RIGHT:
                        case KEY_CENTER:
                    		blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, (KEY_GAMEPAD)keyLasted, NO);
                            break;
                        default:
                            break;
                    }
                    keyLasted = KEY_LEFT;
                    blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, KEY_LEFT, YES);
                }
                if(d.right)
                {
                    switch (keyLasted) {
                        case KEY_UP:
                        case KEY_DOWN:
                        case KEY_LEFT:
                        case KEY_CENTER:
                   			 blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, (KEY_GAMEPAD)keyLasted, NO);
                            break;
                        default:
                            break;
                    }
                    keyLasted = KEY_RIGHT;
                    blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, KEY_RIGHT, YES);
                    
                }
                if(d.center)
                {
                    switch (keyLasted) {
                        case KEY_UP:
                        case KEY_DOWN:
                        case KEY_LEFT:
                        case KEY_RIGHT:
                    		blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, (KEY_GAMEPAD)keyLasted, NO);
                            break;
                        default:
                            break;
                    }
                    keyLasted = KEY_CENTER;
                    blockMjPad.buttonValueChangedHandler(blockMjPad->mjPad.peripheral.name, AXIS_DPAD, KEY_CENTER, YES);
                }
            }
            
            NSLog(@"十字键: (上:%d 下:%d 左:%d 右:%d 中:%d)", d.up, d.down, d.left, d.right, d.center);
            NSString *str = [NSString stringWithFormat:@"十字键: (上:%d 下:%d 左:%d 右:%d 中:%d)", d.up, d.down, d.left, d.right, d.center];
            Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        };
        
        [mjPad scan];
    }
}

- (void)registeriCadeGamepad:(UIViewController* )viewCon
{
    Log_OC(LOG_LEVEL_TRACE, @"registeriCadePad Enter", [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    
    if(iCadePad == nil) return;
    
    [iCadePad registerKey:viewCon];
    
    __block MojingGamepad *blockMjPad = [MojingGamepad sharedGamepad];
    iCadePad.valueChangedHandler = ^void(KEY_GAMEPAD keyID, BOOL pressed){
        if(blockMjPad.buttonValueChangedHandler)
        {
            switch (keyID) {
                case KEY_UP:
                case KEY_DOWN:
                case KEY_LEFT:
                case KEY_RIGHT:
                 	blockMjPad.buttonValueChangedHandler(@"iCade", AXIS_NONE, KEY_CENTER, !pressed);  //模拟CENTER键
                default:
                	blockMjPad.buttonValueChangedHandler(@"iCade", AXIS_NONE, keyID, pressed);
                    break;
            }
        }
        
        NSLog(@"按键[ID=%d]%@", keyID, pressed? @"按下":@"松开");
        NSString *str = [NSString stringWithFormat:@"按键[ID=%d]%@", keyID, pressed? @"按下":@"松开"];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
}

- (void)registerMFIGamepad
{
    if(mfiPad == nil) return;
    
    Log_OC(LOG_LEVEL_TRACE, @"registerMFIPad Enter", [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    
    __block MojingGamepad *blockMjPad = [MojingGamepad sharedGamepad];
    mfiPad.buttonValueChangedHandler = ^void(AXIS_GAMEPAD axisID, KEY_GAMEPAD keyID, BOOL pressed){
        if(blockMjPad.buttonValueChangedHandler)
        {
            switch (keyID) {
                case KEY_UP:
                case KEY_DOWN:
                case KEY_LEFT:
                case KEY_RIGHT:
                    blockMjPad.buttonValueChangedHandler(@"MFI", axisID, KEY_CENTER, !pressed);  //模拟CENTER键
                default:
                    blockMjPad.buttonValueChangedHandler(@"MFI", axisID, keyID, pressed);
                    break;
            }
        }
        
        NSLog(@"按键[ID=%d/%d]%@", axisID, keyID, pressed? @"按下":@"松开");
        NSString *str = [NSString stringWithFormat:@"按键[ID=%d/%d]%@", axisID, keyID, pressed? @"按下+++++":@"松开"];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    mfiPad.axisValueChangedHandler = ^void(AXIS_GAMEPAD axisID, float xValue, float yValue)
    {
        if(blockMjPad.axisValueChangedHandler)
            blockMjPad.axisValueChangedHandler(@"MFI", axisID, xValue, yValue);
        
        NSLog(@"摇杆[Pad ID=%d]坐标: (%f, %f)", axisID, xValue, yValue);
        //NSString *str = [NSString stringWithFormat:@"摇杆[Pad ID=%d]坐标: (%f, %f)", axisID, xValue, yValue];
        //Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
    };
    
    [mfiPad registerKey];
}

- (void)registMGAudioSession {
    
    __block MojingGamepad *blockMjPad = [MojingGamepad sharedGamepad];
    _audioSession.audioSessionHandler = ^void(KEY_GAMEPAD keyEvent, BOOL pressed) {
        if (blockMjPad.buttonValueChangedHandler)
            blockMjPad.buttonValueChangedHandler(@"MG", AXIS_NONE, keyEvent, pressed);
    };
}

- (void)registerGamepad :(UIViewController* )viewCon;
{
	//Mojing4 Gamepad
    [self registerMJGamepad];
    
    //iCade Gamepad
    [self registeriCadeGamepad:viewCon];
    
    //MFI GamePad
    [self registerMFIGamepad];
    
//    [self registMGAudioSession];
}

- (void)registerGamepad;
{
    //Mojing4 Gamepad
    [self registerMJGamepad];

    //iCade Gamepad
    dispatch_sync(dispatch_get_main_queue(), ^{

        UIViewController* viewCon = [UIApplication sharedApplication].keyWindow.rootViewController;
        NSLog(@"keyWindow addr = %p,  rootViewController addr = %p", [UIApplication sharedApplication].keyWindow, viewCon);
        NSString *str = [NSString stringWithFormat:@"keyWindow addr = %p,  rootViewController addr = %p", [UIApplication sharedApplication].keyWindow, viewCon];
        Log_OC(LOG_LEVEL_TRACE, str, [[NSString stringWithUTF8String:__FILE__] lastPathComponent],  __LINE__);
        [self registeriCadeGamepad:viewCon];
    });
    
    //MFI GamePad
    [self registerMFIGamepad];
    
//    [self registMGAudioSession];
    
}

- (void)scan
{
    if(mjPad)
    {
        [mjPad scan];
    }
}

- (void)stopScan
{
    if(mjPad)
    {
        [mjPad stopScan];
    }
}

- (void)disconnect
{
    if(mjPad)
    {
        [mjPad disconnect];
    }
}

- (void)setAutoScan:(BOOL)bAuto
{
    if(mjPad)
    {
        mjPad.isAutoScan = bAuto;
    }
}

- (CBCentralManager *)getMotionGamepadCentralManager
{
    if(mjPad)
    {
        return mjPad.centralManager;
    }
    return nil;
}
- (CBPeripheral *)getMotionGamepadPeripheral
{
    if(mjPad)
    {
        return mjPad.peripheral;
    }
    return nil;
}
- (NSDictionary *)getMotionGamepadDeviceInfoDic
{
    if(mjPad)
    {
        return [mjPad getDeviceInfoCharValueDictionary];
    }
    return nil;
}
@end
