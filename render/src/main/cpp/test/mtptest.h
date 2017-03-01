//=============================================================================
// FILE: app.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#pragma once

#include <GLES/gl.h>

#include <math.h>
#include "imgui.h"
#include "svrApplication.h"
#include "svrCpuTimer.h"
#include "svrGeometry.h"
#include "svrGpuTimer.h"
#include "svrKtxLoader.h"
#include "svrRenderTarget.h"
#include "svrShader.h"
#include "svrUtil.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/transform.hpp"

class MtpTestApp : public Svr::SvrApplication

{
public:
    MtpTestApp();
    void Initialize();
    void Update();
    void Render();
    void AllocateEyeBuffers();

private:

    int                 mFrameCount;
    unsigned int        mLastFPSTime;
    float               mCurrentFPS;
    glm::mat4           mViewMtx[2];
    glm::fquat          mBasisQuat;
    glm::mat4           mBasisMtx;
    Svr::SvrBufferedCpuTimer    mFrameTimer;
    Svr::SvrGpuTimer            mGpuTimer;
    glm::fquat          mPrevOrientation;
};