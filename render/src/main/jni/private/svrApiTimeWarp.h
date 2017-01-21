//=============================================================================
// FILE: svrApiTimeWarp.h
//
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#ifndef _SVR_API_TIMEWARP_H_
#define _SVR_API_TIMEWARP_H_

#include "svrGeometry.h"
#include "svrShader.h"
#include "svrRenderTarget.h"

#include "private/svrApiCore.h"

bool svrBeginTimeWarp();
bool svrEndTimeWarp();

enum svrSensorPosition
{
    kSensorLandscapeLeft = 0,
    kSensorLandscapeRight
};

enum svrWarpMeshOrder
{
    kMeshOrderLeftToRight = 0,
    kMeshOrderRightToLeft,
    kMeshOrderTopToBottom,
    kMeshOrderBottomToTop
};

enum svrWarpMeshArea
{
    kMeshLeft = 0,  // Columns Left to Right [-1, 0]
    kMeshRight,     // Columns Left to Right [0, 1]
    kMeshUL,        // Rows Top to Bottom [Upper Left]
    kMeshUR,        // Rows Top to Bottom [Upper Right]
    kMeshLL,        // Rows Top to Bottom [Lower Left]
    kMeshLR,        // Rows Top to Bottom [Lower Right]
    kMeshU,         // Merged mesh Upper
    kMeshL,         // Merged mesh Lower

    // Low resolution undistorted versions
    kMeshLeft_Low,
    kMeshRight_Low,
    kMeshUL_Low,
    kMeshUR_Low,
    kMeshLL_Low,
    kMeshLR_Low,

    // Inverted Versions
    kMeshLeft_Inv,
    kMeshRight_Inv,
    kMeshUL_Inv,
    kMeshUR_Inv,
    kMeshLL_Inv,
    kMeshLR_Inv,

    // Inverted low resolution undistorted versions
    kMeshLeft_Low_Inv,
    kMeshRight_Low_Inv,
    kMeshUL_Low_Inv,
    kMeshUR_Low_Inv,
    kMeshLL_Low_Inv,
    kMeshLR_Low_Inv,

    kNumWarpMeshAreas
};

enum svrWarpShader
{
    // Standard Warp Shaders
    kShaderSeparate = 0,    // Separate eye buffer for left and right (Unity)
    kShaderSingle,          // Single eye buffer with left and right on either half (Unreal)
    kShaderArray,           // Eye buffers are in an array texture.  0 = Left and 1 = Right
    kShaderSeparate_Mojing, // for Merged mesh
    kShaderSeparate_Mojing_NoOverlay, // Merged mesh without overlay
    kShaderSeparate_Mojing_NoCAC, // Merged mesh without CAC

    // Standard Warp Shaders with Texture Overlay
    kShaderSeparate_Overlay,
    kShaderSingle_Overlay,
    kShaderArray_Overlay,

    // Standard Warp Shaders with Image Overlay
    kShaderSeparate_ImageOverlay,
    kShaderSingle_ImageOverlay,
    kShaderArray_ImageOverlay,

    // Standard Warp Shaders. No chromatic abberation
    kShaderSeparate_NoChroma,
    kShaderSingle_NoChroma,
    kShaderArray_NoChroma,

    // Standard Warp Shaders with Texture Overlay. No chromatic abberation
    kShaderSeparate_Overlay_NoChroma,
    kShaderSingle_Overlay_NoChroma,
    kShaderArray_Overlay_NoChroma,

    // Standard Warp Shaders with Image Overlay. No chromatic abberation
    kShaderSeparate_ImageOverlay_NoChroma,
    kShaderSingle_ImageOverlay_NoChroma,
    kShaderArray_ImageOverlay_NoChroma,

    kNumWarpShaders
};

struct SvrAsyncWarpResources
{
    Svr::SvrGeometry        warpMeshes[kNumWarpMeshAreas];

    Svr::SvrShader          warpShaders[kNumWarpShaders];

    Svr::SvrGeometry        blitMeshGeom;
    Svr::SvrShader          blitShader;
    Svr::SvrShader          blitImageShader;

    Svr::SvrGeometry        stencilMeshGeomLeft;
    Svr::SvrGeometry        stencilMeshGeomRight;
    Svr::SvrShader          stencilShader;

    Svr::SvrRenderTarget    overlayTarget;
};

#endif //_SVR_API_TIMEWARP_H_