//
//  MJGamepadSensor.m
//  ble
//
//  Created by 汤申亮 on 17/4/11.
//
//

#import "MJGamepadSensor.h"

extern void MojingSDK_SendControllerDataV2(float* pArray, double dTimestamp, bool bRecenter);

@interface MJGamepadSensor ()
{
    float fSensorArray[14];
    double dPressHomeTimeStamp;
    bool bHomePressed;
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
        //Fixed Orientation
        fSensorArray[10] = 0;
        fSensorArray[11] = 0;
        fSensorArray[12] = 0;
        fSensorArray[13] = 1;
        dPressHomeTimeStamp = 0;
        bHomePressed = false;
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
    
    fSensorArray[0] = sensorData.fOrientationX;
    fSensorArray[1] = sensorData.fOrientationY;
    fSensorArray[2] = sensorData.fOrientationZ;
    fSensorArray[3] = sensorData.fOrientationW;
    fSensorArray[4] = sensorData.fAccelX;
    fSensorArray[5] = sensorData.fAccelY;
    fSensorArray[6] = sensorData.fAccelZ;
    fSensorArray[7] = sensorData.fGyroX;
    fSensorArray[8] = sensorData.fGyroY;
    fSensorArray[9] = sensorData.fGyroZ;
    bool bRecenter = false;
    if(bHomePressed && ((_timestamp - dPressHomeTimeStamp) >= 800000000))
    {
        bRecenter = true;
        fSensorArray[10] = sensorData.fOrientationX;
        fSensorArray[11] = sensorData.fOrientationY;
        fSensorArray[12] = sensorData.fOrientationZ;
        fSensorArray[13] = sensorData.fOrientationW;
    }
    //NSLog(@"mojing-motion sensor data: CurrentOrientation(%f/%f/%f/%f), FixedOrientation(%f/%f/%f/%f),
    //        timestamp(%f), bReCenter(%d)",
    //      fSensorArray[0], fSensorArray[1], fSensorArray[2], fSensorArray[3],
    //      fSensorArray[10], fSensorArray[11], fSensorArray[12], fSensorArray[13],
    //      _timestamp, bRecenter);

    MojingSDK_SendControllerDataV2(fSensorArray, _timestamp/1000000000.0, bRecenter);
}

- (void)setTimestamp:(NSTimeInterval)timestamp
{
    _timestamp = timestamp;
}

- (void)setHomeKeyStatus:(NSTimeInterval)timestamp keyStaus:(bool)bPressed
{
    if(bHomePressed != bPressed)
    {
        bHomePressed = bPressed;
        dPressHomeTimeStamp = timestamp;
    }
}

@end
