//
//  MojingKeyCode.h
//  MojingSDK
//
//  Created by 汤申亮 on 15/10/10.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#ifndef MojingSDK_MojingKeyCode_h
#define MojingSDK_MojingKeyCode_h

typedef enum _KEY_GAMEPAD
{
    KEY_HOME = 3,      //KEYCODE_HOME
    KEY_BACK = 4,      //KEYCODE_BACK
    KEY_UP = 19,       //KEYCODE_PAD_UP
    KEY_DOWN = 20,     //KEYCODE_PAD_RIGHT
    KEY_LEFT = 21,     //KEYCODE_PAD_LEFT
    KEY_RIGHT = 22,    //KEYCODE_PAD_RIGHT
    KEY_CENTER = 23,   //KEYCODE_PAD_CENTER
    KEY_VOLUMEUP = 24,  //KEYCODE_VOLUME_UP
    KEY_VOLUMEDOWN = 25, //KEYCODE_VOLUME_DOWN
    KEY_OK = 66,       //KEYCODE_ENTER
    KEY_MENU = 82,     //KEYCODE_MENU
    BUTTON_A = 96,
    BUTTON_B = 97,
    BUTTON_X = 99,
    BUTTON_Y = 100,
    BUTTON_L1 = 102,
    BUTTON_R1 = 103,
    BUTTON_L2 = 104,
    BUTTON_R2 = 105,
    BUTTON_THUMBL = 106,
    BUTTON_THUMBR = 107,   
    BUTTON_START = 108,
    BUTTON_SELECT = 109,
    BUTTON_MODE = 110,
    BUTTON_PAUSE = 111,
    
    KEY_CONNECT = 0xFFF0,   //标识手柄连接状态，pressed为YES表示连接，NO表示断开连接
    KEY_BLUETOOTH = 0xFFF1  //标识系统设置蓝牙是否打开可用，pressed为YES表示可用，NO表示禁用
} KEY_GAMEPAD;

typedef enum _AXIS_GAMEPAD
{
    AXIS_NONE  = 0,
    AXIS_DPAD  = 100,
    AXIS_LEFT  = 101,
    AXIS_RIGHT = 102,
    
    MAX_AXIS_VALUE = 16384  //用于数值转换，将轴坐标的float转换为int
} AXIS_GAMEPAD;
#endif
