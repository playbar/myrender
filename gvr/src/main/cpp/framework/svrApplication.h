//=============================================================================
// FILE: svrApplication.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#pragma once

#include <android/sensor.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <imgui.h>
#include <pthread.h>

#include "svrApi.h"

#include "svrInput.h"
#include "svrRenderTarget.h"

#define SVR_NUM_EYE_BUFFERS     3
#define SVR_NUM_EYES			2

namespace Svr
{
    enum SvrEyeId
    {
        kLeft = 0,
        kRight = 1
    };

    struct SvrEyeBuffer
    {
        SvrRenderTarget eyeTarget[SVR_NUM_EYES];
		SvrRenderTarget singleSampledBufferForTimeWarp[SVR_NUM_EYES];
    };

    struct SvrApplicationContext
    {
        int             physicalWidth;
        int             physicalHeight;
        int             targetEyeWidth;
        int             targetEyeHeight;
        float           targetEyeFovXDeg;
        float           targetEyeFovYDeg;

        SvrEyeBuffer    eyeBuffers[SVR_NUM_EYE_BUFFERS];
        int             eyeBufferIndex;
    
        EGLDisplay      display;
        EGLSurface      eyeRenderSurface;
        EGLContext      eyeRenderContext;
       
        ANativeActivity*    activity;
        AAssetManager*  assetManager;
        ANativeWindow*  nativeWindow;

        svrPerfLevel    cpuPerfLevel;
        svrPerfLevel    gpuPerfLevel;
        unsigned int    trackingMode;

    	char*          internalPath;
        char*          externalPath;

        int            frameCount;
    };

    class SvrApplication
    {
    public:
        SvrApplication();
        virtual ~SvrApplication();

        virtual void Initialize();
        virtual void Shutdown();

        virtual void Update();
        virtual void Render() = 0;

        virtual void AllocateEyeBuffers();

        SvrInput& GetInput();
        SvrApplicationContext& GetApplicationContext();
      
    protected:
        //ImGui Members
        static bool mImGuiInitialized;
        static int  mImGuiShaderHandle;
        static int  mImGuiVertHandle;
        static int  mImGuiFragHandle;
        static int  mImGuiAttribLocTex;
        static int  mImGuiAttribLocProjMtx;
        static int  mImGuiAttribLocPos;
        static int  mImGuiAttribLocUv;
        static int  mImGuiAttribLocColor;
        static unsigned int mImGuiFontTextureHandle;
        static unsigned int mImGuiVboHandle;
        static unsigned int mImGuiVaoHandle;
        static unsigned int mImGuiElementsHandle;
        static void CreateImGuiDeviceObjects();
        static void RenderImGuiDrawLists(ImDrawData* draw_data);

    protected:
        SvrApplicationContext   mAppContext;
        SvrInput                mInput;
    };

    extern SvrApplication* CreateApplication();
}
