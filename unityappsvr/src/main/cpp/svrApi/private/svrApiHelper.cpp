//=============================================================================
// FILE: svrApiHelper.cpp
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================

#include <unistd.h>
#include <sys/syscall.h>

#include "svrApiCore.h"
#include "svrApiHelper.h"
#include "svrCpuTimer.h"
#include "svrUtil.h"    // Added for log functions

using namespace Svr;

//-----------------------------------------------------------------------------
bool svrSetThreadAffinity(int coreId)
//-----------------------------------------------------------------------------
{
    long returnVal = 0;
    long result = 0;

    pid_t threadId = gettid();

    int currentMask = 0;
    result = syscall(__NR_sched_getaffinity, threadId, sizeof(currentMask), &currentMask);
    LOGI("    Current Affinity (Thread 0x%x): 0x%x", (int)threadId, currentMask);

    int mask = 1 << coreId;
    returnVal = syscall(__NR_sched_setaffinity, threadId, sizeof(mask), &mask);

    currentMask = 0;
    result = syscall(__NR_sched_getaffinity, threadId, sizeof(currentMask), &currentMask);
    LOGI("    New Affinity (Thread 0x%x): 0x%x", (int)threadId, currentMask);

    return (returnVal == 0 ? true : false);
}

//-----------------------------------------------------------------------------
bool svrClearThreadAffinity()
//-----------------------------------------------------------------------------
{
    long returnVal = 0;
    long result = 0;

    pid_t threadId = gettid();

    int currentMask = 0;
    result = syscall(__NR_sched_getaffinity, threadId, sizeof(currentMask), &currentMask);
    LOGI("    Current Affinity (Thread 0x%x): 0x%x", (int)threadId, currentMask);

    int mask = 0xf;
    returnVal = syscall(__NR_sched_setaffinity, threadId, sizeof(mask), &mask);

    currentMask = 0;
    result = syscall(__NR_sched_getaffinity, threadId, sizeof(currentMask), &currentMask);
    LOGI("    New Affinity (Thread 0x%x): 0x%x", (int)threadId, currentMask);

    return (returnVal == 0 ? true : false);
}


//-----------------------------------------------------------------------------
double svrGetCurrentPointInFramePct()
//-----------------------------------------------------------------------------
{
    double framePeriodNano = 1e9 / gAppContext->deviceInfo.displayRefreshRateHz;
    uint64_t timestamp = Svr::GetTimeNano();
    double framePct = (double)Svr::gAppContext->modeContext->vsyncCount + ((double)(timestamp - Svr::gAppContext->modeContext->vsyncTimeNano) / framePeriodNano);
    double fractFrame = framePct - ((long)framePct);
	return fractFrame;
}