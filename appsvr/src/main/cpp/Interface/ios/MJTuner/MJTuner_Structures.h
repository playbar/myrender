//
//  MJTuner_Structures.h
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/7.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#ifndef MojingSDK_MJTuner_Structures_h
#define MojingSDK_MJTuner_Structures_h

//#import <CoreGraphics/CoreGraphics.h>

//按键类型
typedef NS_ENUM(uint32_t, MJTunerButtonType)
{
    MJTunerButtonUnknown       = 0,
    MJTunerButtonMenu          = 1 << 0,
};

typedef struct
{
    uint8_t raw_char_size; //原始数据字节大小
    NSTimeInterval timestamp; //时间戳
    
    //按键标志位位域
    union
    {
        struct
        {
            uint32_t menu:1; //菜单键
        };
        uint32_t all_flags; //所有标志位
    };
    
    //模拟量
    uint8_t value_tuner; //滑动变阻器值
} mj_tuner_buffer;

typedef struct
{
    int analog;
    int pressed;
    float value;
} mj_tuner_button_buffer;

#endif
