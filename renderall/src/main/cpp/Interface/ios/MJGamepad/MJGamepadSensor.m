//
//  MJGamepadSensor.m
//  ble
//
//  Created by 汤申亮 on 17/4/11.
//
//

#import "MJGamepadSensor.h"

@interface MJGamepadSensor ()
{
}

@end

@implementation MJGamepadSensor
@synthesize dicOrientation = _dicOrientation;
@synthesize dicAccel = _dicAccel;
@synthesize dicGyro = _dicGyro;
@synthesize timestamp = _timestamp;
@synthesize valueChangedHandler = _valueChangedHandler;

- (void)dealloc
{
}

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        _dicOrientation = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                           @0, @"x", @0, @"y", @0, @"z", @1.0, @"w", nil];
        _dicAccel = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                           @0, @"x", @0, @"y", @0, @"z", nil];
        _dicGyro = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                           @0, @"x", @0, @"y", @0, @"z", nil];
    }
    return self;
}

- (void)setSensorData:(mj_sensor)sensorData
{
    [_dicOrientation setObject:@(sensorData.fOrientationX) forKey:@"x"];
    [_dicOrientation setObject:@(sensorData.fOrientationY) forKey:@"y"];
    [_dicOrientation setObject:@(sensorData.fOrientationZ) forKey:@"z"];
    [_dicOrientation setObject:@(sensorData.fOrientationW) forKey:@"w"];
    
    [_dicAccel setObject:@(sensorData.fAccelX) forKey:@"x"];
    [_dicAccel setObject:@(sensorData.fAccelY) forKey:@"y"];
    [_dicAccel setObject:@(sensorData.fAccelZ) forKey:@"z"];
    
    [_dicGyro setObject:@(sensorData.fGyroX) forKey:@"x"];
    [_dicGyro setObject:@(sensorData.fGyroY) forKey:@"y"];
    [_dicGyro setObject:@(sensorData.fGyroZ) forKey:@"z"];
    
    if (_valueChangedHandler)
    {
        _valueChangedHandler(self, _dicOrientation, _dicAccel, _dicGyro, _timestamp);
    }
}

- (void)setTimestamp:(NSTimeInterval)timestamp
{
    _timestamp = timestamp;
}

@end
