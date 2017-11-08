//
//  MJGamepadThumbStick.h
//  MJGamepadSDK
//
//  Created by 秦宏 on 15/5/21.
//
//

#import <Foundation/Foundation.h>
#import "MJGamepadElement.h"

@class MJGamepadThumbStick;

typedef void (^MJGamepadThumbStickValueChangedBlock)(MJGamepadThumbStick *stick, float xAxis, float yAxis);

@interface MJGamepadThumbStick : MJGamepadElement
{
    float _xAxis;
    float _yAxis;
    float _radian;
    float _offsetToCenter;
    NSTimeInterval _timestamp;
    MJGamepadThumbStickValueChangedBlock _valueChangedHandler;
}

@property (nonatomic, readonly) float xAxis;
@property (nonatomic, readonly) float yAxis;
@property (nonatomic, readonly) float radian;
@property (nonatomic, readonly) float offsetToCenter;
@property (nonatomic, readonly) NSTimeInterval timestamp;
@property (nonatomic, copy) MJGamepadThumbStickValueChangedBlock valueChangedHandler;

- (void)setPoint:(CGPoint)point;
- (void)setTouchPoint:(CGPoint)point;
- (void)setTimestamp:(NSTimeInterval)timestamp;

@end
