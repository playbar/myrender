//=============================================================================
// FILE: svrUtil.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//============================================================================
#pragma once

#include <math.h>
#include <android/log.h>

#include "glm/glm.hpp"

#include "svrApi.h"

#define VERBOSE_LOGGING_ENABLED 0

#define LOGE(...) __android_log_print( ANDROID_LOG_ERROR, "svr", __VA_ARGS__ )
#define LOGI(...) __android_log_print( ANDROID_LOG_INFO, "svr", __VA_ARGS__ )
#define LOGW(...) __android_log_print( ANDROID_LOG_WARN, "svr", __VA_ARGS__ )

#if defined(NDEBUG) || !VERBOSE_LOGGING_ENABLED
#define LOGV(...)
#else
#define LOGV(...) __android_log_print( ANDROID_LOG_VERBOSE, "svr", __VA_ARGS__ )
#endif

namespace Svr
{
    void SvrCheckGlError(const char* file, int line);
    void SvrCheckEglError(const char* file, int line);

    void SvrGetEyeViewMatrices(const svrHeadPoseState& poseState, bool bUseHeadModel,
                                float ipd, float headHeight, float headDepth,
                                glm::mat4& outLeftEyeMatrix, glm::mat4& outRightEyeMatrix);
};

#if defined(NDEBUG)
#define GL( func ) func;
#define EGL( func ) func;
#else
#define GL( func ) func; Svr::SvrCheckGlError(__FILE__,__LINE__);
#define EGL( func ) func; Svr::SvrCheckEglError(__FILE__,__LINE__);
#endif


#ifndef DEG_TO_RAD
#define DEG_TO_RAD (float)M_PI / 180.0f
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG (float)180.0f / M_PI
#endif

#ifndef NANOSECONDS_TO_MILLISECONDS
#define NANOSECONDS_TO_MILLISECONDS 0.000001f
#endif

#define DEFAULT_HEAD_HEIGHT     0.0750f
#define DEFAULT_HEAD_DEPTH      0.0805f
#define DEFAULT_IPD             0.0640f



