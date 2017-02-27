//=============================================================================
// FILE: svrApiSensor.h
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#ifndef _SVR_API_SENSOR_H_
#define _SVR_API_SENSOR_H_

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "private/svrApiCore.h"

bool svrStartSensors(bool UseDSPSensors);
bool svrStopSensors();

glm::fquat svrGetHeadPoseAsQuat(float predictedTimeMs);

#endif //_SVR_API_SENSOR_H_