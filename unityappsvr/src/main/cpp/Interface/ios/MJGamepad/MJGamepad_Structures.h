//
//  MJGamepad_Structures.h
//  MJGamepadSDK
//
//  Created by 秦宏 on 15/5/27.
//
//

#ifndef ble_MJGamepad_Structures_h
#define ble_MJGamepad_Structures_h

#import <CoreGraphics/CoreGraphics.h>

//按键类型
typedef NS_ENUM(uint32_t, MJGamepadButtonType)
{
    MJGamepadButtonUnknown       = 0,
    MJGamepadButtonA             = 1 << 0,
    MJGamepadButtonB             = 1 << 1,
    MJGamepadButtonX             = 1 << 2,
    MJGamepadButtonY             = 1 << 3,
    MJGamepadButtonLeftShoulder  = 1 << 4,
    MJGamepadButtonRightShoulder = 1 << 5,
    MJGamepadButtonLeftTrigger   = 1 << 6,
    MJGamepadButtonRightTrigger  = 1 << 7,
    
    MJGamepadButtonUp            = 1 << 8,
    MJGamepadButtonDown          = 1 << 9,
    MJGamepadButtonLeft          = 1 << 10,
    MJGamepadButtonRight         = 1 << 11,
    MJGamepadButtonCenter        = 1 << 12,
    MJGamepadButtonBack          = 1 << 13,
    MJGamepadButtonStart         = 1 << 14,
    MJGamepadButtonLeftThumb     = 1 << 15,
    MJGamepadButtonRightThumb    = 1 << 16,
    MJGamepadButtonMenu          = 1 << 17,
};

typedef struct {
    union
    {
        struct
        {
            uint8_t   up:1,
            down:1,
            left:1,
            right:1,
            center:1;
        };
        uint8_t all_flags;
    };
} mj_direction;

typedef struct
{
    uint8_t raw_char_size; //原始数据字节大小
    NSTimeInterval timestamp; //时间戳
    
    //按键标志位位域
    union
    {
        struct
        {
            uint32_t a:1, // A键
            b:1, // B键
            x:1, // X键
            y:1, // Y键
            left_shoulder:1, // 左Shoulder
            right_shoulder:1, // 右Shoulder
            left_trigger:1, // 左Triger
            right_trigger:1, // 右Triger
            up:1, // 十字键上
            down:1, // 十字键下
            left:1, // 十字键左
            right:1, // 十字键右
            center:1, // 十字键Center
            back:1, //返回
            start:1, //开始
            left_thumb:1, //左摇杆(主摇杆)
            right_thumb:1, //右摇杆
            menu:1; //菜单键
        };
        uint32_t all_flags; //所有标志位
    };
    
    //模拟量
    CGPoint left_thumb_point; //左摇杆坐标
    CGPoint right_thumb_point; //右摇杆坐标
} mj_buffer;

typedef struct
{
    int analog;
    int pressed;
    float value;
} mj_button_buffer;

#endif
