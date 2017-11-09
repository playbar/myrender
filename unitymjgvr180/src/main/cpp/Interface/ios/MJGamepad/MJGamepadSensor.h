//
//  MJGamepadSensor.h
//  MJGamepadSDK
//
//  Created by 汤申亮 on 17/4/11.
//
//

#import <Foundation/Foundation.h>
#import "MJGamepadElement.h"
#import "MJGamepad_Structures.h"

@class MJGamepadSensor;

typedef void (^MJGamepadSensorValueChangedBlock)(MJGamepadSensor *sensor, NSMutableDictionary *dicOrientation, NSMutableDictionary *dicAccel, NSMutableDictionary *dicGyro, double timestamp);

@interface MJGamepadSensor : MJGamepadElement
{
    NSMutableDictionary *_dicOrientation;
    NSMutableDictionary *_dicAccel;
    NSMutableDictionary *_dicGyro;
    NSTimeInterval _timestamp;
    MJGamepadSensorValueChangedBlock _valueChangedHandler;
}

@property (nonatomic, readonly) NSMutableDictionary *dicOrientation;
@property (nonatomic, readonly) NSMutableDictionary *dicAccel;
@property (nonatomic, readonly) NSMutableDictionary *dicGyro;
@property (nonatomic, readonly) NSTimeInterval timestamp;
@property (nonatomic, copy) MJGamepadSensorValueChangedBlock valueChangedHandler;

- (void)setSensorData:(mj_sensor)sensorData;
- (void)setTimestamp:(NSTimeInterval)timestamp;
- (void)setHomeKeyStatus:(NSTimeInterval)timestamp keyStaus:(bool)bPressed;

@end
