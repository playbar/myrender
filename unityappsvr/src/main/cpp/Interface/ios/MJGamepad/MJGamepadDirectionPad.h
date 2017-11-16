//
//  MJGamepadDirectionPad.h
//  MJGamepadSDK
//
//  Created by 秦宏 on 15/5/21.
//
//

#import <Foundation/Foundation.h>
#import "MJGamepadElement.h"
#import "MJGamepad.h"

@class MJGamepadButton, MJGamepadDirectionPad;

typedef void (^MJGamepadDirectionPadValueChangedBlock)(MJGamepadDirectionPad *dpad);

@interface MJGamepadDirectionPad : MJGamepadElement
{
    MJGamepadButton *_up;
    MJGamepadButton *_down;
    MJGamepadButton *_left;
    MJGamepadButton *_right;
    MJGamepadButton *_center;
    NSTimeInterval _timestamp;
    MJGamepadDirectionPadValueChangedBlock _valueChangedHandler;
}

@property (nonatomic, readonly, getter=getDirection) mj_direction direction;
@property (nonatomic, readonly) MJGamepadButton *up;
@property (nonatomic, readonly) MJGamepadButton *down;
@property (nonatomic, readonly) MJGamepadButton *left;
@property (nonatomic, readonly) MJGamepadButton *right;
@property (nonatomic, readonly) MJGamepadButton *center;
@property (nonatomic, readonly) NSTimeInterval timestamp;
@property (nonatomic, copy) MJGamepadDirectionPadValueChangedBlock valueChangedHandler;

- (void)setTimestamp:(NSTimeInterval)timestamp;
- (void)setBuffer:(mj_direction)buffer;
- (mj_direction)getDirection;

@end
