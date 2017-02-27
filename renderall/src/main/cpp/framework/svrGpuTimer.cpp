//=============================================================================
// FILE: svrGpuTimer.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#include "svrGpuTimer.h"

#include "svrUtil.h"    // For log functions

#include <EGL/egl.h>    // For eglGetProcAddress

#define GL_TIME_ELAPSED_EXT                              0x88BF
#define GL_TIMESTAMP_EXT                                 0x8E28
#define GL_QUERY_RESULT                                  0x8866
#define GL_QUERY_RESULT_AVAILABLE                        0x8867
#define GL_GPU_DISJOINT_EXT                              0x8FBB 

namespace Svr
{
    typedef void (*glGenQueriesFunc)(GLuint n, GLuint *ids);
    typedef void (*glDeleteQueriesFunc)(GLuint n, const GLuint *ids);
    typedef void (*glBeginQueryFunc)(GLuint target, GLuint id);
    typedef void (*glEndQueryFunc)(GLuint target);
    typedef void (*glGetQueryObjectuivFunc)(GLuint id, GLuint pname, GLuint *params);
    typedef void (*glQueryCounterEXTFunc)(GLuint target, GLuint id);
    typedef void (*glGetQueryObjectui64vEXTFunc)(GLuint id, GLuint pname,  unsigned long long int *params);

    static glGenQueriesFunc             glGenQueries = 0;
    static glDeleteQueriesFunc          glDeleteQueries = 0;
    static glBeginQueryFunc             glBeginQuery = 0;
    static glEndQueryFunc               glEndQuery = 0;
    static glGetQueryObjectuivFunc      glGetQueryObjectuiv = 0;
    static glQueryCounterEXTFunc        glQueryCounterEXT = 0;
    static glGetQueryObjectui64vEXTFunc glGetQueryObjectui64vEXT = 0;

    bool SvrGpuTimer::smGLExtensionsLoaded = false;
    bool SvrGpuTimer::LoadGLExtensions()
    {
        // Load all the timer function pointers...
        glGenQueries = (glGenQueriesFunc)eglGetProcAddress("glGenQueries");
        glDeleteQueries = (glDeleteQueriesFunc)eglGetProcAddress("glDeleteQueries");
        glBeginQuery = (glBeginQueryFunc)eglGetProcAddress("glBeginQuery");
        glEndQuery = (glEndQueryFunc)eglGetProcAddress("glEndQuery");
        glGetQueryObjectuiv = (glGetQueryObjectuivFunc)eglGetProcAddress("glGetQueryObjectuiv");
        glQueryCounterEXT = (glQueryCounterEXTFunc)eglGetProcAddress("glQueryCounterEXT");
        glGetQueryObjectui64vEXT = (glGetQueryObjectui64vEXTFunc)eglGetProcAddress("glGetQueryObjectui64vEXT");

        // ... and check the results
        if (!glGenQueries) 
        {
            LOGI("glGenQueries: FAILED");
            return false;
        }
        if (!glDeleteQueries) 
        {
            LOGI("glDeleteQueries: FAILED");
            return false;
        }
        if (!glBeginQuery) 
        {
            LOGI("glBeginQuery: FAILED");
            return false;
        }
        if (!glEndQuery) 
        {
            LOGI("glEndQuery: FAILED");
            return false;
        }
        if (!glGetQueryObjectuiv) 
        {
            LOGI("glGetQueryObjectuiv: FAILED");
            return false;
        }

        if (!glQueryCounterEXT) 
        {
            LOGI("glQueryCounterEXT: FAILED");
            return false;
        }

        if (!glGetQueryObjectui64vEXT) 
        {
            LOGI("glGetQueryObjectui64vEXT: FAILED");
            return false;
        }

        smGLExtensionsLoaded = true;
        return true;
    }

    bool SvrGpuTimer::Initialize(int numSamples)
    {
        mInitialized = false;

        if(mQueries[0])
        {
            delete [] mQueries[0];
            delete [] mQueries[1];
        }

        if(mTimeElapsed)
        {
            delete [] mTimeElapsed;
        }

        if(!smGLExtensionsLoaded)
        {
            if(!LoadGLExtensions()) return false;
        }

        mNumSamples = numSamples;
        mDisjointOccurred = 0;
        mAvailable = 0;

        mQueries[0] = new unsigned int[mNumSamples];
        mQueries[1] = new unsigned int[mNumSamples];
        mTimeElapsed = new uint64_t[mNumSamples];

        mCurrentIndex = 0;
        glGenQueries(mNumSamples, mQueries[0]);
        glGenQueries(mNumSamples, mQueries[1]);
        mCurrentQueries = 0;

        for(int i=0; i<mNumSamples; i++)
        {
            mTimeElapsed[i] = 0;
        }

        mInitialized = true;

        return true;
    }

    void SvrGpuTimer::StartFrame()
    {
        mCurrentIndex = 0;
        if(mInitialized)
        {
            glGetIntegerv(GL_GPU_DISJOINT_EXT, &mDisjointOccurred);
        }
        TakeSample();
    }

    void SvrGpuTimer::EndFrame()
    {
        TakeSample();                                           // Add a sample for the timer internal EndFrame sample.
        if(!mInitialized || mCurrentIndex != mNumSamples)   // First sample is timer internal StartFrame sample.
        {
            // LOGI("Number of samples: %d\nNumber of expected samples: %d\nInitializing Timer!\n", mCurrentIndex, mNumSamples);

            if(!Initialize(mCurrentIndex))
            {
                return;
            }
        }
        else
        {
            mAvailable = false;
            mQueriesReady[mCurrentQueries] = true;
            mCurrentQueries = mCurrentQueries ? 0 : 1;
            if(mQueriesReady[mCurrentQueries])
            {
                while (!mAvailable) 
                {
                    glGetQueryObjectuiv(mQueries[mCurrentQueries][mNumSamples-1], GL_QUERY_RESULT_AVAILABLE, &mAvailable);
                }

                glGetIntegerv(GL_GPU_DISJOINT_EXT, &mDisjointOccurred);

                if (!mDisjointOccurred) {
                    for (int i = 0; i < mNumSamples; i++) 
                    {
                        /* See how much time the rendering of object i took in nanoseconds. */
                        glGetQueryObjectui64vEXT(mQueries[mCurrentQueries][i], GL_QUERY_RESULT, &mTimeElapsed[i]);
                    }
                }
                mQueriesReady[mCurrentQueries] = false;
            }
        }
    }

    unsigned int SvrGpuTimer::TakeSample()
    {
        if(mInitialized && mCurrentIndex < mNumSamples)
        {
            glQueryCounterEXT(mQueries[mCurrentQueries][mCurrentIndex], GL_TIMESTAMP_EXT);
            // glQueryCounter(mQueries[mCurrentQueries][mCurrentIndex], GL_TIMESTAMP);
        }
        return mCurrentIndex++;
    }

}   // namespace svr
