//--------------------------------------------------------------------------------------
// Copyright 2016 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//--------------------------------------------------------------------------------------

#ifndef IntelVSYNC_H
#define IntelVSYNC_H

class IntelVSync
{
public:
    IntelVSync() : mpLibDRM(NULL), mDrmFd(-1), mLastLatencyStartTime(0), mLastVBlankSys(0), mLastVBlankHW(0), mSequence(0), mTimeDrift(0), mDroppedFrame(true)
    {}

    bool OpenDRM();
    bool CloseDRM();

    void StartLatencyTiming();
    bool WaitForVSync();
    bool WaitForHalfVSync();

    bool CheckBeforeVSync();
    bool CheckBeforeHalfVSync();

    int64_t GetTimeToNextVSync();
    int64_t GetTimeToNextHalfVSync();

    void Sleep(uint64_t usec);

    static uint64_t GetCurrentTimeUSec();

    bool HasDroppedFrame() { return mDroppedFrame; }

private:
    void * mpLibDRM;
    int    mDrmFd;

    uint64_t mLastLatencyStartTime;
    uint64_t mLastVBlankSys;
    uint64_t mLastVBlankHW;
    uint32_t mSequence;
    int32_t mTimeDrift;
    bool mDroppedFrame;
};

#endif //IntelVSYNC_H