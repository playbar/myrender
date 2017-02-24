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
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <android/log.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include "../../Base/MojingTypes.h"
#include "IntelVSync.h"

#define WANT_VERBOSE 1
#define WANT_SPIN_SLEEP 1
#define WAIT_ON_VBLANK_CLOCK 0
#define VSYNC_DELAY 0
#define REFRESH_RATE 60

// #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "VSYNC", __VA_ARGS__))
// #define MOJING_ERROR( g_APIlogger , ...) ((void)__android_log_print(ANDROID_LOG_ERROR, "VSYNC", __VA_ARGS__))
// 
// #if WANT_VERBOSE
// #define LOGV(...)  ((void)__android_log_print(ANDROID_LOG_VERBOSE, "VSYNC", __VA_ARGS__))
// #else
// #define LOGV(...)
// #endif

#ifdef LOG4CPLUS_IMPORT
#include "../log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif // ENABLE_LOGGER

#define MIN( a, b ) ((a)<(b)?(a):(b))
#define MAX( a, b ) ((a)>(b)?(a):(b))
#define USEC_ARGS "%llus %03llums %03lluus"
//#define USEC_ARGS "%lus %03lums %03luus"
#define USEC_PARAMS( T ) (T)/1000000,((T)%1000000)/1000, ((T)%1000000)%1000


// Drm
//#include <xf86drm.h>

// libdrm vsync
typedef enum {
    DRM_VBLANK_ABSOLUTE = 0x0,	/**< Wait for specific vblank sequence number */
    DRM_VBLANK_RELATIVE = 0x1,	/**< Wait for given number of vblanks */
    /* bits 1-6 are reserved for high crtcs */
    DRM_VBLANK_HIGH_CRTC_MASK = 0x0000003e,
    DRM_VBLANK_EVENT = 0x4000000,	/**< Send event instead of blocking */
    DRM_VBLANK_FLIP = 0x8000000,	/**< Scheduled buffer swap should flip */
    DRM_VBLANK_NEXTONMISS = 0x10000000,	/**< If missed, wait for next vblank */
    DRM_VBLANK_SECONDARY = 0x20000000,	/**< Secondary display controller */
    DRM_VBLANK_SIGNAL = 0x40000000	/* Send signal instead of blocking */
} drmVBlankSeqType;
#define DRM_VBLANK_HIGH_CRTC_SHIFT 1

typedef struct _drmVBlankReq {
    drmVBlankSeqType type;
    unsigned int sequence;
    unsigned long signal;
} drmVBlankReq, *drmVBlankReqPtr;

typedef struct _drmVBlankReply {
    drmVBlankSeqType type;
    unsigned int sequence;
    long tval_sec;
    long tval_usec;
} drmVBlankReply, *drmVBlankReplyPtr;

typedef union _drmVBlank {
    drmVBlankReq request;
    drmVBlankReply reply;
} drmVBlank, *drmVBlankPtr;

typedef int(*pfn_drmOpen)(const char *name, const char *busid);
typedef int(*pfn_drmClose)(int fd);
typedef int(*pfn_drmWaitVBlank)(int fd, drmVBlankPtr vbl);

pfn_drmOpen         drmOpen = 0;
pfn_drmClose        drmClose = 0;
pfn_drmWaitVBlank   drmWaitVBlank = 0;

class TimeStat
{
public:
    TimeStat() : mTotalTime(0), mMaxTime(0), mCount(0) {}

    void Update(int64_t time)
    {
        if (time < 0)
            time = 0 - time;

        mTotalTime += time;
        if (time > mMaxTime)
            mMaxTime = time;

        mCount++;
    }

    int32_t GetAverageTime() { return (int32_t)(mTotalTime / (float)mCount); }
    int32_t GetMaxTime() { return mMaxTime; }
    void Reset() { mTotalTime = 0;  mMaxTime = 0;  mCount = 0; }


    int64_t mTotalTime;
    int32_t mMaxTime;
    int32_t mCount;
};

class IntelVSyncStats
{
public:
    IntelVSyncStats() : mStartTime(0), mVSyncMiss(0), mHalfVSyncMiss(0), mVSyncEventMiss(0), mHalfVSyncEventMiss(0) {}

    void Frame(bool bVSyncMiss, int32_t waitTime, int32_t driftTime)
    {
        if (mStartTime == 0)
            mStartTime = IntelVSync::GetCurrentTimeUSec();
        uint64_t now = IntelVSync::GetCurrentTimeUSec();

        if (bVSyncMiss)
            mVSyncEventMiss++;

        mVSyncWaitTime.Update(waitTime);
        mVSyncDriftTime.Update(driftTime);
        mPosLatency.Update(now - mStartLatencyTime);

        uint64_t delta = now - mStartTime;
        const int32_t seconds = 10;
        if (delta > (seconds * 1000000))
        {
            char str[400] = "";
            const int columnSize = 15;
            static int count = 0;

            if ((count++ % 10) == 0)
            {
                sprintf(str, "Stats per %3ds |%*s|%*s|%*s|%*s|%*s|%*s|%*s|%*s|%*s|%*s",
                    seconds,
                    columnSize, "VS Miss", columnSize, "1/2 VS Miss",
                    columnSize, "VS Ev Miss", columnSize, "1/2 VS Ev Miss",
                    columnSize, "VS Max Wait", columnSize, "VS Av Wait",
                    columnSize, "1/2 VS Max Wait", columnSize, "1/2 VS Av Wait",
                    columnSize, "Pos Max Lat", columnSize, "Pos Av Lat");
                    MOJING_TRACE(g_APIlogger ,str);
            }

            sprintf(str, "%*s|%*d|%*d|%*d|%*d|%*d|%*d|%*d|%*d|%*d|%*d",
                columnSize, " ",
                columnSize, mVSyncMiss, columnSize, mHalfVSyncMiss,
                columnSize, mVSyncEventMiss, columnSize, mHalfVSyncEventMiss,
                columnSize, mVSyncWaitTime.GetMaxTime(), columnSize, mVSyncWaitTime.GetAverageTime(),
                columnSize, mHalfVSyncWaitTime.GetMaxTime(), columnSize, mHalfVSyncWaitTime.GetAverageTime(),
                columnSize, mPosLatency.GetMaxTime(), columnSize, mPosLatency.GetAverageTime()
                );
			MOJING_TRACE(g_APIlogger, str);

            mStartTime = now;
            mVSyncMiss = 0;
            mHalfVSyncMiss = 0;
            mVSyncEventMiss = 0;
            mHalfVSyncEventMiss = 0;

            mVSyncWaitTime.Reset();
            mVSyncDriftTime.Reset();
            mHalfVSyncWaitTime.Reset();
            mPosLatency.Reset();
        }
    }

    void MissVSync(bool bMiss)
    {
        if (bMiss)
            mVSyncMiss++;
    }

    bool MissHalfVSync(bool bMiss)
    {
        if (bMiss)
            mHalfVSyncMiss++;
		return bMiss;
    }

    void HalfFrame(bool bMiss, uint64_t waitTime)
    {
        if (bMiss)
            mHalfVSyncEventMiss++;
        mHalfVSyncWaitTime.Update(waitTime);
    }

    void StartLatencyTiming(uint64_t time)
    {
        mStartLatencyTime = time;
    }

    uint64_t mStartTime;
    uint32_t mVSyncEventMiss;
    uint32_t mHalfVSyncEventMiss;
    uint32_t mVSyncMiss;
    uint32_t mHalfVSyncMiss;
    uint64_t mStartLatencyTime;

    TimeStat mVSyncWaitTime;
    TimeStat mVSyncDriftTime;
    TimeStat mHalfVSyncWaitTime;
    TimeStat mPosLatency;

};

static IntelVSyncStats s_Stats;

bool IntelVSync::OpenDRM()
{
    mpLibDRM = dlopen("libdrm.so", RTLD_LAZY);
    if (!mpLibDRM)
    {
		MOJING_ERROR( g_APIlogger , "Failed to open libDRM");
        return false;
    }

    drmOpen = (pfn_drmOpen)dlsym(mpLibDRM, "drmOpen");
    drmClose = (pfn_drmClose)dlsym(mpLibDRM, "drmClose");
    drmWaitVBlank = (pfn_drmWaitVBlank)dlsym(mpLibDRM, "drmWaitVBlank");

    if (!drmOpen || !drmClose || !drmWaitVBlank)
    {
        MOJING_ERROR( g_APIlogger , "Failed to load function pointers");
        return false;
    }

    mDrmFd = drmOpen("i915", NULL);
    if (mDrmFd == -1)
    {
		MOJING_ERROR(g_APIlogger, "Failed to open libDRM [" << errno << " / " << strerror(errno) << "]");
        return false;
    }

    pthread_t self = pthread_self();
    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    int err = pthread_setschedparam(self, SCHED_FIFO, &params);
    if (err)
    {
		MOJING_ERROR(g_APIlogger, "Failed set sched param [ " << err << " / " << errno << " / " << strerror(errno) << "]");
//        return false;
    }

    int policy = 0;
    err = pthread_getschedparam(self, &policy, &params);
    if (err)
    {
		MOJING_ERROR(g_APIlogger, "Failed get sched param [ " << err << " / " << errno << " / " << strerror(errno) << "]");
        //return false;
    }
    //LOGI("Policy %u%s Priority %u\n",policy, (policy == SCHED_FIFO) ? " SCHED_FIFO" : "", params.sched_priority);

    int priority = getpriority(PRIO_PROCESS, 0);
    //LOGI("Found process priority %d\n", priority);

    return true;
}

bool IntelVSync::CloseDRM()
{
    if (mDrmFd != -1)
    {
        if (drmClose)
        {
            drmClose(mDrmFd);
        }
    }

    if (mpLibDRM)
    {
        dlclose(mpLibDRM);
    }
    return 1;
}

uint64_t IntelVSync::GetCurrentTimeUSec()
{
    struct timespec now;
    int err = clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000000 + (now.tv_nsec / 1000);
}

void IntelVSync::Sleep(uint64_t usec)
{
#if WANT_SPIN_SLEEP
    uint64_t start = GetCurrentTimeUSec();

    while (GetCurrentTimeUSec() < (start + usec))
    {
        // spin
    }
#else
    struct timespec wait;
    struct timespec rem;
    wait.tv_sec = 0;
    wait.tv_nsec = usec * 1000;
    nanosleep(&wait, &rem);
#endif
}

void IntelVSync::StartLatencyTiming()
{
    uint64_t sysTime = GetCurrentTimeUSec();
    s_Stats.StartLatencyTiming(sysTime);
}

bool IntelVSync::CheckBeforeVSync()
{
    int64_t wait = GetTimeToNextVSync();
    bool bMiss = wait < 0;
    //LOGV("CheckBeforeVSync : %lldus wait [%d]", wait, bMiss);
    s_Stats.MissVSync(bMiss);
    return bMiss;
}

bool IntelVSync::CheckBeforeHalfVSync()
{
    int64_t wait = GetTimeToNextHalfVSync();
    bool bMiss = wait < 0;
    //LOGV("CheckBeforeHalfVSync : %lldus wait [%d]", wait, bMiss);
    s_Stats.MissHalfVSync(bMiss);
    return bMiss;
}

int64_t IntelVSync::GetTimeToNextVSync()
{
	if (mLastVBlankSys == 0)
	{
		MOJING_TRACE(g_APIlogger , "mLastVBlankSys == 0");
		return 0;
	}

    const float frameTime = 1000000.0f / REFRESH_RATE;
    uint64_t sysTime = GetCurrentTimeUSec();

#if WAIT_ON_VBLANK_CLOCK
    uint64_t nextVBlank = mLastVBlankHW + (uint32_t)frameTime;
#else
    uint64_t nextVBlank = mLastVBlankSys + (uint32_t)frameTime;
#endif
    int64_t waitTime = nextVBlank - sysTime;
    //LOGV("Time to next vsync = %lldus", waitTime);
	if (waitTime < 0)
	{
		waitTime = ((-waitTime - 1) / frameTime + 1) * frameTime;
	}
    return waitTime;
}

int64_t IntelVSync::GetTimeToNextHalfVSync()
{
    if (mLastVBlankSys == 0)
        return 0;
	const float frameTime = 1000000.0f / REFRESH_RATE;
    const float halfFrameTime = 1000000.0f / (REFRESH_RATE * 2.0f);
    uint64_t sysTime = GetCurrentTimeUSec();

#if WAIT_ON_VBLANK_CLOCK
    uint64_t halfVBlank = mLastVBlankHW + (uint32_t)halfFrameTime;
#else
    uint64_t halfVBlank = mLastVBlankSys + (uint32_t)halfFrameTime;
#endif
    int64_t waitTime = halfVBlank - sysTime;
    //LOGV("Time to next 1/2 vsync = %lldus", waitTime);
	if (waitTime < 0)
	{
		waitTime = ((-waitTime - 1) / frameTime + 1) * frameTime;
	}
    return waitTime;
}

bool IntelVSync::WaitForHalfVSync()
{
    int64_t waitTime = GetTimeToNextHalfVSync();

    if (waitTime < 0)
    {
        s_Stats.HalfFrame(true, 0);
        waitTime = 0 - waitTime;
        //LOGV("Missed the 1/2 frame vsync by %5lldus", waitTime);
        return false;
    }

    this->Sleep(waitTime);
    s_Stats.HalfFrame(false, waitTime);


    uint64_t sysNow = GetCurrentTimeUSec();
    //LOGV("1/2 VSync : Drift = %5dus, Wait = %5lldus, Sys = " USEC_ARGS ", 1/2 Vblank : " USEC_ARGS, mTimeDrift, waitTime, USEC_PARAMS(sysNow), USEC_PARAMS(waitTime + mLastVBlankSys));
    return true;
}


bool IntelVSync::WaitForVSync()
{
    static int count = 0;

    if (!drmWaitVBlank || !mDrmFd)
    {
        MOJING_ERROR( g_APIlogger , "Failed to wait for VSync");
        return false;
    }
    mDroppedFrame = false;

    // re-enable to test out the fake vsync tracking
    // It will re-calibrate once per second
    if (1)//(count++ % REFRESH_RATE) == 0)
    {
        const uint64_t sysStart = GetCurrentTimeUSec();

        // Which pipe to use? Look at pipe output from HWC log
        // adb shell hwclogviewer –v
        // 707s 252ms 848480ns TID:2675 drmIoctl(DRM_IOCTL_I915_GEM_WAIT[boHandle 5, timeout 3000000000])
        // 707s 253ms 221805ns TID : 2675 DrmPageFlip Drm DrmDisplay 0 DrmConnector 29[Crtc 13] issuing drm updates for frame : 498 707s 221ms[timeline:499]
        // 707s 253ms 255368ns TID : 2675 DrmSetDisplayPageFlip Crtc : 13 Pipe : 1 ZOrder : 0, PASASBCA
        //                                                                ^^^^^^^^
		uint32_t pipe = 0;//3; 
        uint32_t flags = DRM_VBLANK_ABSOLUTE;
        if (pipe == 1)
            flags |= DRM_VBLANK_SECONDARY;
        else if (pipe > 1)
            flags |= ((pipe << DRM_VBLANK_HIGH_CRTC_SHIFT) & DRM_VBLANK_HIGH_CRTC_MASK);

        drmVBlank vbl;
        vbl.request.sequence = mSequence;
        vbl.request.signal = 0;
		vbl.request.type =  (drmVBlankSeqType)(flags | (mSequence == 0 ? DRM_VBLANK_NEXTONMISS : 0));

        int err = drmWaitVBlank(mDrmFd, &vbl);

        if (err)
        {
			MOJING_ERROR(g_APIlogger, "failed vblank [" << mDrmFd << " " << err << " / " << errno << " / " << strerror(errno) << "]");
            mLastVBlankSys = sysStart;
            mLastVBlankHW = sysStart;
            mDroppedFrame = true;
            s_Stats.Frame(true, 0, 0);
            return false;
        }
        else
        {
            // Establish system time and vblank time in usecs.
            const uint64_t sysNow = GetCurrentTimeUSec();
            const uint64_t vblNow = vbl.reply.tval_sec * 1000000 + vbl.reply.tval_usec;

            const int64_t sysWait = (int32_t)(sysNow - sysStart);

            if (mSequence > 0)
            {
                // Calulate elapsed and difference.
                int32_t sysEla = (int32_t)(sysNow - mLastVBlankSys);
                int32_t vblEla = (int32_t)(vblNow - mLastVBlankHW);

                mTimeDrift = (int32_t)(sysNow - vblNow);

                //LOGV("VSync : Wait = %5lldus, Sys = " USEC_ARGS ", Vblank : " USEC_ARGS ", SysElapsed : %5uus, VblankElapsed : %5uus, Seq : %d",sysWait, USEC_PARAMS(sysNow), USEC_PARAMS(vblNow), sysEla, vblEla, mSequence);

                if (vbl.reply.sequence != mSequence)
                {
                    mDroppedFrame = true;
                    //LOGV("Expected seq %u, got %u [wait %5lldus]\n", mSequence, vbl.reply.sequence, sysWait);
                }

                s_Stats.Frame(mDroppedFrame, sysWait, mTimeDrift);
            }


            mLastVBlankSys = sysNow + VSYNC_DELAY;
            mLastVBlankHW = vblNow + VSYNC_DELAY;
            mSequence = vbl.reply.sequence + 1;
        }
    }
    else // end of if 1
    {
        const uint64_t sysStart = GetCurrentTimeUSec();
        
        const float frameTime = 1000000.0f / REFRESH_RATE;
#if WAIT_ON_VBLANK_CLOCK
        uint64_t nextVBlank = mLastVBlankHW + (uint64_t)frameTime;
#else
        uint64_t nextVBlank = mLastVBlankSys + (uint64_t)frameTime;
#endif
        int64_t waitTime = nextVBlank - sysStart;
        if (waitTime < 0)
        {
            mDroppedFrame = true;
        }
        else
        {
            this->Sleep(waitTime);
        }
        const uint64_t sysNow = GetCurrentTimeUSec();
        const int64_t sysWait = (int64_t)(sysNow - sysStart);
        int64_t sysEla = (int64_t)(sysNow - mLastVBlankSys);

        mLastVBlankSys = mLastVBlankSys + frameTime;
        mLastVBlankHW = mLastVBlankSys;
        mSequence++;
        
        //LOGV("Fake VSync : Sleep = %5lldus, Wait = %5lldus, Sys = " USEC_ARGS " SysElapsed : %5lldus", waitTime, sysWait, USEC_PARAMS(sysNow), sysEla);
    }

    return mDroppedFrame;
}
