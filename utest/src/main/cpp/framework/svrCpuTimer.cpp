//=============================================================================
// FILE: svrCpuTimer.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#include "svrCpuTimer.h"

namespace Svr
{
    uint64_t GetTimeNano()
    {
        struct timespec t;
        clock_gettime(CLOCK_MONOTONIC, &t);
        uint64_t result = t.tv_sec * 1000000000LL + t.tv_nsec;
        return result;
    }

    SvrBufferedCpuTimer::SvrBufferedCpuTimer(int bufferSize)
        : mInsertIndex(0)
    {
        mpTimeBuffer = new float[bufferSize];
        mBufferSize = bufferSize;
        Reset();
    }

    SvrBufferedCpuTimer::~SvrBufferedCpuTimer()
    {
        delete[] mpTimeBuffer;
    }

    void SvrBufferedCpuTimer::Start()
    {
        mTimer.Start();
    }

    float SvrBufferedCpuTimer::Stop()
    {
        float val = (float)mTimer.Stop();
        mpTimeBuffer[mInsertIndex++] = val;
        if (mInsertIndex >= mBufferSize)
        {
            mInsertIndex = 0;
        }
        return val;
    }

    void SvrBufferedCpuTimer::Reset()
    {
        mTimer.Reset();
        memset(&mpTimeBuffer[0], 0, mBufferSize * sizeof(double));
        mInsertIndex = 0;
    }

    float SvrBufferedCpuTimer::GetAverageTime()
    {
        float avgVal = 0.0f;
        for (int i = 0; i < mBufferSize; i++)
        {
            avgVal += mpTimeBuffer[i];
        }
        avgVal /= mBufferSize;
        return avgVal;
    }
}