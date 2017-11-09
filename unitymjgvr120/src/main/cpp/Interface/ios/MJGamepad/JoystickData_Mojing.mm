//
// Created by 汤申亮 on 17/4/11.
//

#include "JoystickData_Mojing.h"

#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include <math.h>


JoystickData_Mojing::JoystickData_Mojing()
        : lastTimestamp(0)
        , F(0)
{

}

JoystickData_Mojing::~JoystickData_Mojing()
{

}

bool JoystickData_Mojing::BytesToData(uint8_t *pOriginData, STJoystickData* pJoystickData)
{
    if(pOriginData == NULL || pJoystickData == NULL)
    {
        return false;
    }
    char* bytes = (char *)pOriginData;
  //       NSLog(@"mjbytes: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7], bytes[8], bytes[9], bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15], bytes[16], bytes[17], bytes[18], bytes[19]);
    
    ResetData(bytes);
    
    int32_t timestamp = ReadInt(9, false);
    int32_t idx = ReadInt(2, false);
    int32_t orientation1 = ReadInt(14, true);
    int32_t orientation2 = ReadInt(14, true);
    int32_t orientation3 = ReadInt(14, true);
    int32_t accel1 = ReadInt(13, true);
    int32_t accel2 = ReadInt(13, true);
    int32_t accel3 = ReadInt(13, true);
    int32_t gyro1 = ReadInt(13, true);
    int32_t gyro2 = ReadInt(13, true);
    int32_t gyro3 = ReadInt(13, true);
    int32_t touchpadX = ReadInt(8, false);
    int32_t touchpadY = ReadInt(8, false);
    int32_t keyState = ReadInt(5, false);
    int32_t keyStateEx = ReadInt(1, false);
    keyState |= (keyStateEx << 5);
    //        NSLog(@"timestamp:%d idx:%d o1:%d o2:%d o3:%d a1:%d a2:%d a3:%d g1:%d g2:%d g3:%d touchY:%d touchY:%d key:%x",
    //                            timestamp, idx, orientation1, orientation2, orientation3, accel1, accel2, accel3, gyro1, gyro2, gyro3, touchpadX, touchpadY, keyState);
    
    float f1 = GetOrientation(orientation1);
    float f2 = GetOrientation(orientation2);
    float f3 = GetOrientation(orientation3);
    float f4 = sqrtf(1.0F - f1 * f1 - f2 * f2 - f3 * f3);
    switch (idx)
    {
        case 0:
            pJoystickData->fOrientationX = f4;
            pJoystickData->fOrientationY = f1;
            pJoystickData->fOrientationZ = f2;
            pJoystickData->fOrientationW = f3;
            break;
        case 1:
            pJoystickData->fOrientationX = f1;
            pJoystickData->fOrientationY = f4;
            pJoystickData->fOrientationZ = f2;
            pJoystickData->fOrientationW = f3;
            break;
        case 2:
            pJoystickData->fOrientationX = f1;
            pJoystickData->fOrientationY = f2;
            pJoystickData->fOrientationZ = f4;
            pJoystickData->fOrientationW = f3;
            break;
        case 3:
            pJoystickData->fOrientationX = f1;
            pJoystickData->fOrientationY = f2;
            pJoystickData->fOrientationZ = f3;
            pJoystickData->fOrientationW = f4;
            break;
    }
    
    pJoystickData->fAccelX = GetGravity(accel1);
    pJoystickData->fAccelY = GetGravity(accel2);
    pJoystickData->fAccelZ = GetGravity(accel3);
    pJoystickData->fGyroX = GetGyro(gyro1);
    pJoystickData->fGyroY = GetGyro(gyro2);
    pJoystickData->fGyroZ = GetGyro(gyro3);
    
    if (touchpadX != 0 || touchpadY != 0) {
        pJoystickData->bTouched = true;
    } else {
        pJoystickData->bTouched = false;
    }
    
    pJoystickData->fTouchpadX = touchpadX / 255.0F;
    pJoystickData->fTouchpadY = touchpadY / 255.0F;
    pJoystickData->keyState = keyState;
    
    if (timestamp <= lastTimestamp)
        F = (512L + F);
    lastTimestamp = timestamp;
    pJoystickData->dTimestamp = 1000000.0 * (F + timestamp); // minisecond to nano

//  NSLog(@"timestamp: %d %x %"PRIu64"", timestamp, timestamp, 1000000L * (F + timestamp));

    return true;
}

float JoystickData_Mojing::GetOrientation(int paramInt) {
    return (float) (paramInt / sqrtf(2.0F) / 8191.0F);
}

float JoystickData_Mojing::GetGravity(int paramInt) {
    return (9.8F * 8.0F * paramInt / 4095.0F);
}

float JoystickData_Mojing::GetGyro(int paramInt) {
    return (float) (2048.0 * 3.141592653589793 * paramInt / 4095.0F / 180.0);
}
