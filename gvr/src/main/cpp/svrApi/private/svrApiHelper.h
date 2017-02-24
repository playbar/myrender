//=============================================================================
// FILE: svrApiHelper.h
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#ifndef _SVR_API_HELPER_H_
#define _SVR_API_HELPER_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/quaternion.hpp"

#include "private/svrApiCore.h"

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (float)M_PI / 180.0f
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG (float)180.0f / M_PI
#endif

bool svrSetThreadAffinity(int coreId);
bool svrClearThreadAffinity();
double svrGetCurrentPointInFramePct();

static inline svrQuaternion svrQuatFromGlmQuat(const glm::fquat& fq)
{
    svrQuaternion retQuat;
    memcpy(&retQuat.x, glm::value_ptr(fq), 4 * sizeof(float));
	return retQuat;
}

static inline svrQuaternion svrQuarFromGlmVec4(const glm::vec4& fv)
{
    svrQuaternion retQuat;
    memcpy(&retQuat.x, glm::value_ptr(fv), 4 * sizeof(float));
	return retQuat;
}

static inline glm::fquat  glmQuatFromSvrQuat(const svrQuaternion& sq)
{
    glm::fquat retQuat(sq.w, sq.x, sq.y, sq.z);
    return retQuat;
}

#endif //_SVR_API_HELPER_H_