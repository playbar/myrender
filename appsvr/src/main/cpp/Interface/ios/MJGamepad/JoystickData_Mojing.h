//
// Created by 汤申亮 on 17/4/11.
//

#ifndef CONTROLLER_SERVICE_JOYSTICKDATA_MOJING_1_H
#define CONTROLLER_SERVICE_JOYSTICKDATA_MOJING_1_H

#include "JoystickDataBase.h"

struct STJoystickData
{
    float fOrientationX; // 归一化四元数(X, Y, Z, W)
    float fOrientationY;
    float fOrientationZ;
    float fOrientationW;
    float fAccelX;
    float fAccelY;
    float fAccelZ;
    float fGyroX;
    float fGyroY;
    float fGyroZ;
    float fTouchpadX;  // X: 0~1
    float fTouchpadY; 	// Y: 0~1
    bool  bTouched;
    uint8_t keyState; 	// 0x1:Click, 0x2:Home, 0x4:App, 0x8:VolumeDown, 0x10:VolumeUp
    double dTimestamp; // ns
};

class JoystickData_Mojing : public JoystickDataBase {

public:
    JoystickData_Mojing();
    virtual ~JoystickData_Mojing();

    bool BytesToData(uint8_t *pOriginData, STJoystickData* pJoystickData);

private:
    float GetOrientation(int32_t paramInt);
    float GetGravity(int32_t paramInt);
    float GetGyro(int32_t paramInt);

private:
    int32_t lastTimestamp;
    int64_t F;
};

#endif //CONTROLLER_SERVICE_JOYSTICKDATA_MOJING_1_H
