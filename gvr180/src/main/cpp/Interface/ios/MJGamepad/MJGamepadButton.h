//
//  MJGamepadButton.h
//  MJGamepadSDK
//
//  Created by 秦宏 on 15/5/21.
//
//

#import <Foundation/Foundation.h>
#import "MJGamepadElement.h"

@class MJGamepadButton;

typedef void (^MJGamepadButtonValueChangedBlock)(MJGamepadButton *button, BOOL pressed, float value);

@interface MJGamepadButton : MJGamepadElement
{
    BOOL _analog;
    BOOL _pressed;
    float _value;
    NSTimeInterval _timestamp;
    MJGamepadButtonValueChangedBlock _valueChangedHandler;
}

@property (nonatomic, readonly) BOOL pressed; //按键是否按下
@property (nonatomic, readonly) BOOL analog; //是否为压感按键
@property (nonatomic, readonly) float value; //按键压力等级
@property (nonatomic, readonly) NSTimeInterval timestamp; //按键事件发送时间
@property (nonatomic, copy) MJGamepadButtonValueChangedBlock valueChangedHandler;

//hide
- (void)setAnalog:(BOOL)analog;
- (void)setPressed:(BOOL)pressed;
- (void)setValue:(float)value;
- (void)setTimestamp:(NSTimeInterval)timestamp;
- (NSDictionary *)toDictionary;

@end
