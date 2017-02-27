//=============================================================================
// FILE: svrGpuTimer.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <GLES3/gl3.h>

namespace Svr
{
    class SvrGpuTimer
    {
    public:
        SvrGpuTimer() 
            : mInitialized(false)
			, mDisjointOccurred(0)
			, mAvailable(0)
			, mNumSamples(0)
            , mCurrentQueries(0)
			, mTimeElapsed(0)
			, mCurrentIndex(0)
        { 
            mQueries[0] = 0; 
            mQueries[1] = 0; 
            mQueriesReady[0] = false; 
            mQueriesReady[1] = false; 
        }

        void            StartFrame();
        unsigned int    TakeSample();
        void            EndFrame();
        uint64_t        GetTimeElapsed(int index) { return mInitialized ? mTimeElapsed[index+1] : 0; }
        uint64_t        GetStartTimeElapsed() { return mInitialized ? mTimeElapsed[0] : 0; }
        uint64_t        GetEndTimeElapsed() { return mInitialized ? mTimeElapsed[mNumSamples-1] : 0; }

        // GetTimeTilNextSample, passing in numSamples - 2 (for start and end)  will return the time of current sample until EndFrame.
        uint64_t        GetTimeTilNextSample(int index) { return mInitialized ? mTimeElapsed[index + 1] - mTimeElapsed[index] : 0; } 
         // GetTimeSincePreviousSample, passing in 0 will return the time since StartFrame was called.
        uint64_t        GetTimeSincePreviousSample(int index) { return mInitialized ? mTimeElapsed[index] - mTimeElapsed[index - 1] : 0; }
    protected:
        bool    Initialize(int numSamples);
        bool    LoadGLExtensions();

    protected:
        bool            mInitialized;
        static bool     smGLExtensionsLoaded;
        int             mDisjointOccurred;
        unsigned int    mAvailable;
        int             mNumSamples;
        int             mCurrentQueries;
        bool            mQueriesReady[2];
        unsigned int*   mQueries[2];
        uint64_t*       mTimeElapsed;
        int             mCurrentIndex;
    };
}   // namespace Svr
