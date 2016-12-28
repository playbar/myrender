//
//  MJTunerButton.h
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/7.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#ifndef MojingSDK_MJTunerButton_h
#define MojingSDK_MJTunerButton_h

#import <Foundation/Foundation.h>

@class MJTunerButton;
typedef void (^MJTunerButtonValueChangedBlock)(MJTunerButton *button, BOOL pressed, float value);

@interface MJTunerButton : NSObject
{
    BOOL _analog;
    BOOL _pressed;
    float _value;
    NSTimeInterval _timestamp;
    MJTunerButtonValueChangedBlock _valueChangedHandler;
}

@property (nonatomic, readonly) BOOL pressed; //按键是否按下
@property (nonatomic, readonly) BOOL analog; //是否为压感按键
@property (nonatomic, readonly) float value; //按键压力等级
@property (nonatomic, readonly) NSTimeInterval timestamp; //按键事件发送时间
@property (nonatomic, copy) MJTunerButtonValueChangedBlock valueChangedHandler;

//hide
- (void)setAnalog:(BOOL)analog;
- (void)setPressed:(BOOL)pressed;
- (void)setValue:(float)value;
- (void)setTimestamp:(NSTimeInterval)timestamp;
- (NSDictionary *)toDictionary;

@end

#endif
