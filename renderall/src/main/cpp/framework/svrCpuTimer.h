//=============================================================================
// FILE: svrCpuTimer.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

namespace Svr
{
    uint64_t GetTimeNano();

    class SvrCpuTimer
    {
    public:
        SvrCpuTimer(void)
        {
            Reset();
        }

        void Start(void)
        {
            mStartTime = GetTimeNano();
        }

        // Return value is in milliseconds
        double Stop(void)
        {
            mStopTime = GetTimeNano();
            mValue = (double)(mStopTime - mStartTime) / (double)1.0e6;
            return mValue;
        }

        void Restart(void)
        {
            Stop();
            Start();
        }

        double GetResult() 
        { 
            return mValue; 
        }

        void Reset() 
        { 
            mStartTime = 0;
            mStopTime = 0;
            mValue = 0.0f; 
        }

    public:
        long long       mStartTime;
        long long       mStopTime;
        double          mValue;
    };

    class SvrBufferedCpuTimer
    {
    public:
        SvrBufferedCpuTimer(int bufferSize);
        ~SvrBufferedCpuTimer();

        void    Start();
        float   Stop();
        void    Reset();

        float   GetAverageTime();

    private:
        SvrCpuTimer     mTimer;
        int             mBufferSize;
        int             mInsertIndex;
        float*          mpTimeBuffer;
    };
}

