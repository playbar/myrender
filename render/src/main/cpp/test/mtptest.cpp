//=============================================================================

// FILE: app.cpp

//

//                  Copyright (c) 2015 QUALCOMM Technologies Inc.

//                              All Rights Reserved.

//

//=============================================================================

#include <unistd.h>

#include "svrApi.h"
#include "svrProfile.h"
#include "mtptest.h"

#define DEG_TO_RAD (float)M_PI / 180.0f
#define DEFAULT_IPD 0.0640f
#define PROFILE_EYE_RENDERING   0

using namespace Svr;

const char* gLogTag = "MtpTestApp";

//-----------------------------------------------------------------------------
unsigned int GetTimeMS()
//-----------------------------------------------------------------------------
{
    struct timeval t;

    t.tv_sec = t.tv_usec = 0;
    if (gettimeofday(&t, NULL) == -1)
    {
        return 0;
    }
    return (unsigned int)(t.tv_sec * 1000LL + t.tv_usec / 1000LL);
}

MtpTestApp::MtpTestApp()
    : mFrameTimer(64)
{
}

void MtpTestApp::Initialize()
{
    SvrApplication::Initialize();

    mFrameCount;
    mLastFPSTime = GetTimeMS();
    mCurrentFPS = 0.0f;
}

void MtpTestApp::AllocateEyeBuffers()
{
    LOGI("MtpTestApp - Allocating Separate Single Eye Buffers");
    for (int i = 0; i < SVR_NUM_EYE_BUFFERS; i++)
    {
        mAppContext.eyeBuffers[i].eyeTarget[kLeft].Initialize(mAppContext.targetEyeWidth, mAppContext.targetEyeHeight, 1, GL_RGBA8, true);
        mAppContext.eyeBuffers[i].eyeTarget[kRight].Initialize(mAppContext.targetEyeWidth, mAppContext.targetEyeHeight, 1, GL_RGBA8, true);
    }
}

void MtpTestApp::Update()
{
    PROFILE_ENTER(GROUP_WORLDRENDER, 0, "Update");

    float max = M_PI_2 - 0.01;
    float min = -max;

    mFrameTimer.Start();

    SvrApplication::Update();

    // Update the time
    unsigned int TimeNow = GetTimeMS();
    unsigned int ElapsedTime = TimeNow - mLastFPSTime;
    unsigned int ReportTimeFPS = 1000;

    mFrameCount++;

    if (TimeNow - mLastFPSTime > ReportTimeFPS)
    {
        float ElapsedSeconds = (float)(TimeNow - mLastFPSTime) / 1000.0f;
        mCurrentFPS = (float)mFrameCount / ElapsedSeconds;
        mFrameCount = 0;
        mLastFPSTime = GetTimeMS();;
    }

    PROFILE_EXIT(GROUP_WORLDRENDER);
}

void MtpTestApp::Render()

{
    PROFILE_ENTER(GROUP_WORLDRENDER, 0, "Render");

    svrHeadPoseState poseState = svrGetPredictedHeadPose(0.0f);
    poseState.pose.position.x = 0.0f;
    poseState.pose.position.y = 0.0f;
    poseState.pose.position.z = 0.0f;
    SvrGetEyeViewMatrices(poseState, true,
                        DEFAULT_IPD, DEFAULT_HEAD_HEIGHT, DEFAULT_HEAD_DEPTH, mViewMtx[kLeft], mViewMtx[kRight]);

#if 0
    glm::vec4 prevOrientation = glm::vec4(mPrevOrientation.x, mPrevOrientation.y, mPrevOrientation.z, mPrevOrientation.w);
    glm::vec4 currentOrientation = glm::vec4(poseState.pose.rotation.x, poseState.pose.rotation.y, poseState.pose.rotation.z, poseState.pose.rotation.w);
    mPrevOrientation = glm::fquat(currentOrientation.w, currentOrientation.x, currentOrientation.y, currentOrientation.z);

    glm::vec4 diff = currentOrientation - prevOrientation;

    float W = glm::length(diff);

    float factor = 75.0f * W;
    if (factor > 1.0f)
        factor = 1.0f;

    glClearColor(factor, factor, factor, 1.0f);
#else
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
#endif

    for (int i = 0; i < 2; i++)
    {
        // These get changed when rendering the GUI so put them back now
        glEnable(GL_SCISSOR_TEST);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        //Bind the eye buffer
        mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[i].Bind();
        glViewport(0, 0, mAppContext.targetEyeWidth, mAppContext.targetEyeHeight);
        glScissor(0, 0, mAppContext.targetEyeWidth, mAppContext.targetEyeHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Unbind the eye buffer (call so attachments are invalidated correctly)
        mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[i].Unbind();
    }

    svrFrameParams frameParams;
    memset(&frameParams, 0, sizeof(frameParams));
    frameParams.frameIndex = mAppContext.frameCount;
    frameParams.eyeBufferArray[0] = mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[Svr::kLeft].GetColorAttachment();
    frameParams.eyeBufferArray[1] = mAppContext.eyeBuffers[mAppContext.eyeBufferIndex].eyeTarget[Svr::kRight].GetColorAttachment();
    frameParams.eyeBufferType = kEyeBufferStereoSeparate;

    frameParams.frameOptions |= kEnableMotionToPhoton;
    frameParams.headPoseState = poseState;
    frameParams.minVsyncs = 1;

    PROFILE_EXIT(GROUP_WORLDRENDER);

    svrSubmitFrame(&frameParams);
    mAppContext.eyeBufferIndex = (mAppContext.eyeBufferIndex + 1) % SVR_NUM_EYE_BUFFERS;
    mFrameTimer.Stop();
}

namespace Svr
{
    Svr::SvrApplication* CreateApplication()
    {
        return new MtpTestApp();
    }
}