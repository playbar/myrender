//=============================================================================
// FILE: svrRenderExt.cpp
//
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#include "svrRenderExt.h"
#include "svrUtil.h"

namespace Svr
{
    PFNEGLCREATESYNCKHRPROC     eglCreateSyncKHR;
    PFNEGLDESTROYSYNCKHRPROC    eglDestroySyncKHR;
    PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR;

    void* InitExtension(const char* extName)
    {
        void* pExt = (void*)eglGetProcAddress(extName);
        if (NULL == pExt)
        {
            LOGE("Failed to locate extension: %s", extName);
        }
        return pExt;
    }

    bool InitializeRenderExtensions()
    {
        eglCreateSyncKHR = (PFNEGLCREATESYNCKHRPROC)InitExtension("eglCreateSyncKHR");
        if (!eglCreateSyncKHR)
        {
            return false;
        }

        eglDestroySyncKHR = (PFNEGLDESTROYSYNCKHRPROC)InitExtension("eglDestroySyncKHR");
        if (!eglDestroySyncKHR)
        {
            return false;
        }

        eglClientWaitSyncKHR = (PFNEGLCLIENTWAITSYNCKHRPROC)InitExtension("eglClientWaitSyncKHR");
        if (!eglClientWaitSyncKHR)
        {
            return false;
        }

        return true;
    }
}//End namespace Svr