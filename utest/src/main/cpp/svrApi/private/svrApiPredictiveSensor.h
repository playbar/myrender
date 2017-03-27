//=============================================================================
// FILE: svrApiPredictiveSensor.h
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#ifndef _SVR_API_PREDICTIVE_SENSOR_H_
#define _SVR_API_PREDICTIVE_SENSOR_H_

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "private/svrApiCore.h"

#ifdef USE_QVR_SERVICE
int svrGetPredictiveHeadPoseAsQuat(float predictedTimeMs, uint64_t *pSampleTimeStamp, glm::fquat& orientation, glm::vec3& position);
#endif // USE_QVR_SERVICE

#endif //_SVR_API_PREDICTIVE_SENSOR_H_
