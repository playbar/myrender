//=============================================================================
// FILE: svrApiCore.h
//
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#ifndef _SVR_API_CORE_H_
#define _SVR_API_CORE_H_

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES/glext.h>

#include <android/looper.h>
#include <android/native_window.h>
#include <android/sensor.h>

#include <pthread.h>
#include <jni.h>

// #include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"

#include "svrApi.h"
#include "svrGpuTimer.h"

#ifdef USE_QVR_SERVICE
#include "QVRServiceClient.hpp"
#endif // USE_QVR_SERVICE

#define NUM_SWAP_FRAMES 5

namespace Svr
{
    struct svrFrameParamsInternal
    {
        svrFrameParams      frameParams;
        GLsync              frameSync;
        uint64_t            frameSubmitTimeStamp;
        uint64_t            warpFrameLeftTimeStamp;
        uint64_t            warpFrameRightTimeStamp;
        uint64_t            minVSyncCount;
    };

    struct SvrModeContext
    {
        EGLDisplay      display;
        ANativeWindow*  nativeWindow;

        uint64_t        vsyncCount;
        uint64_t        vsyncTimeNano;
        pthread_mutex_t vsyncMutex;

        //Warp Thread/Context data
        EGLSurface      eyeRenderWarpSurface;
        EGLSurface      eyeRenderOrigSurface;
        EGLint          eyeRenderOrigConfigId;
        EGLContext      eyeRenderContext;

        EGLContext      warpRenderContext;
        EGLSurface      warpRenderSurface;
        int             warpRenderSurfaceWidth;
        int             warpRenderSurfaceHeight;

        pthread_cond_t  warpThreadContextCv;
        pthread_mutex_t warpThreadContextMutex;
        bool            warpContextCreated;

        pthread_t       warpThread;
        bool            warpThreadExit;

        pthread_t       vsyncThread;
        bool            vsyncThreadExit;

        pthread_cond_t  warpBufferConsumedCv;
        pthread_mutex_t warpBufferConsumedMutex;
       
        svrFrameParamsInternal frameParams[NUM_SWAP_FRAMES];
        unsigned int           submitFrameCount;
        unsigned int           warpFrameCount;
        uint64_t               prevSubmitVsyncCount;
        
        // Recenter transforms
        glm::fquat          recenterRot;
        glm::vec3           recenterPos;

        //Sensor data
        ASensorManager*     sensorManager;
        ASensorEventQueue*  sensorEventQueue;
        ASensor const*      sensorRotVector;
        ASensor const*      accVectorSensor;
        ASensor const*      magVectorSensor;
        ASensor const*      gyroVectorSensor;

        pthread_t       sensorThread;
        bool            sensorThreadExit;
        ALooper*        sensorThreadLooper;
        pthread_cond_t  sensorThreadReadyCv;
        pthread_mutex_t sensorThreadReadyMutex;
        bool            sensorThreadReady;
    };

    struct SvrAppContext
    {
        ALooper*            looper;

        JavaVM*	            javaVm;
        JNIEnv*	            javaEnv;
        jobject		        javaActivityObject;

        jclass              javaSvrApiClass;
        jmethodID           javaSvrApiStartVsyncMethodId;
        jmethodID           javaSvrApiStopVsyncMethodId;

#ifdef USE_QVR_SERVICE
        QVRServiceClient*   qvrService;
#endif // USE_QVR_SERVICE

        SvrModeContext*     modeContext;

        svrDeviceInfo       deviceInfo;
        unsigned int        currentTrackingMode;

        bool                m_TimewarpEnabled;
    };

	struct VertexTable
	{
		int m_iVertexSize; // elements count in Vertex
		float*  m_pVertexLeft;
		float*  m_pVertexRight;
		int m_iIndexSize; // elements count in Index
		unsigned int*    m_pIndex;
	};
    extern SvrAppContext* gAppContext;
    extern bool g_bMeshDirty;
	extern VertexTable * gVertexTable;
}

#endif //_SVR_API_CORE_H_