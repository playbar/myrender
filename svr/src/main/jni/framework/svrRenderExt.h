//=============================================================================
// FILE: svrRenderExt.h
//
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>


namespace Svr
{
    bool InitializeRenderExtensions();

    extern PFNEGLCREATESYNCKHRPROC     eglCreateSyncKHR;
    extern PFNEGLDESTROYSYNCKHRPROC    eglDestroySyncKHR;
    extern PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR;

} //End namespace Svr
