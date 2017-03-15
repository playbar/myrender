//=============================================================================
// FILE: svrProfile.cpp
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================

#include <stdio.h>

#include "svrCpuTimer.h"
#include "svrProfile.h"
#include "svrUtil.h"


#if defined(SVR_PROFILING_ENABLED) && defined(SVR_PROFILE_TELEMETRY)

static unsigned char*   gTelemetryBuffer;
TelemetryData           gTelemetryData;

const char*     gTelemetryTitle = "svrApi";
unsigned int    gTelemetryArenaSize = 2*1024*1024;
const char*     gTelemetryFile = "/sdcard/Telemetry/svrApi/tmdata";
bool            gEnableMemoryDebugging = false;
bool            gEnableTelemetryCallstacks = false;

TmU64 TimerFunc()
{
    return Svr::GetTimeNano();
}

bool InitializeTelemetry()
{

#if !defined(ANDROID)
    tmLoadTelemetry(CHECKED_BUILD);
#endif

    tmStartupEx(&TimerFunc);

    gTelemetryBuffer = new unsigned char[gTelemetryArenaSize];

    tmInitializeContext(&gTelemetryData.context, (void*)gTelemetryBuffer, gTelemetryArenaSize);

    TmI32 version = 2;
    tmSetParameter(gTelemetryData.context, TMP_EMULATED_SERVER_VERSION, &version);

    TmErrorCode res = tmOpen(gTelemetryData.context, gTelemetryTitle, __DATE__ " " __TIME__, gTelemetryFile, TMCT_FILE, TELEMETRY_DEFAULT_PORT, TMOF_INIT_NETWORKING, -1);
    if (res != TM_OK)
    {
        LOGE("InitializeTelemetry, tmOpen failed");
        switch (res)
        {
        case TMERR_INVALID_PARAM:
            LOGE("TMERR_INAVLID_PARAM");
            break;
        case TMERR_INVALID_CONTEXT:
            LOGE("TMERR_INVALID_CONTEXT");
            break;
        case TMERR_NETWORK_NOT_INITIALIZED:
            LOGE("TMERR_NETWORK_NOT_INITIALIZED");
            break;
        case TMERR_UNKNOWN_NETWORK:
            LOGE("TMERR_UNKNOWN_NETWORK");
            break;
        case TMERR_DISABLED:
            LOGE("TMERR_DISABLED");
            break;
        case TMERR_OUT_OF_RESOURCES:
            LOGE("TMERR_OUT_OF_RESOURCES");
            break;
        case TMERR_UNINITIALIZED:
            LOGE("TMERR_UNINITIALIZED");
            break;
        case TMERR_BAD_HOSTNAME:
            LOGE("TMERR_BAD_HOSTNAME");
            break;
        case TMERR_COULD_NOT_CONNECT:
            LOGE("TMERR_COULD_NOT_CONNECT");
            break;
        case TMERR_ALREADY_SHUTDOWN:
            LOGE("TMERR_ALREADY_SHUTDOWN");
            break;
        case TMERR_ARENA_TOO_SMALL:
            LOGE("TMERR_ARENA_TOO_SMALL");
            break;
        case TMERR_BAD_HANDSHAKE:
            LOGE("TMERR_BAD_HANDSHAKE");
            break;
        case TMERR_UNALIGNED:
            LOGE("TMERR_UNALIGNED");
            break;
        case TMERR_BAD_VERSION:
            LOGE("TMERR_BAD_VERSION");
            break;
        case TMERR_BAD_TIMER:
            LOGE("TMERR_BAD_TIMER");
            break;
        default:
            LOGE("TMERR_UNKNOWN");
            break;      
        }
        return false;
    }

    for (int i = 0, count = sizeof(gTelemetryData.contexts) / sizeof(gTelemetryData.contexts[0]); i<count; ++i) 
    {
        gTelemetryData.contexts[i] = gTelemetryData.context;
    }

    tmEnable(GROUP_GENERIC, TMO_SUPPORT_MEMORY, gEnableMemoryDebugging ? 1 : 0);
    tmEnable(GROUP_GENERIC, TMO_RECORD_CALLSTACKS, gEnableTelemetryCallstacks ? 1 : 0);

    LOGI("Telemetry Initialized");
    return true;
}

void ShutdownTelemetry()
{
    tmClose(gTelemetryData.context);
    tmShutdownContext(gTelemetryData.context);
    tmShutdown();

    delete[] gTelemetryBuffer;

    LOGI("Telemetry Shutdown");
}

#endif //SVR_PROFILING_ENABLED


#if defined(SVR_PROFILING_ENABLED) && defined(SVR_PROFILE_ATRACE)

#include <dlfcn.h>

void *(*ATrace_beginSection) (const char* sectionName);
void *(*ATrace_endSection) (void);
void *(*ATrace_isEnabled) (void);

typedef void *(*fp_ATrace_beginSection) (const char* sectionName);
typedef void *(*fp_ATrace_endSection) (void);
typedef void *(*fp_ATrace_isEnabled) (void);

void InitializeATrace()
{
    void *lib = dlopen("libandroid.so", RTLD_NOW | RTLD_LOCAL);
    if (lib != NULL) 
    {
        ATrace_beginSection =
            reinterpret_cast<fp_ATrace_beginSection >(
                dlsym(lib, "ATrace_beginSection"));
        ATrace_endSection =
            reinterpret_cast<fp_ATrace_endSection >(
                dlsym(lib, "ATrace_endSection"));
        ATrace_isEnabled =
            reinterpret_cast<fp_ATrace_isEnabled >(
                dlsym(lib, "ATrace_isEnabled"));
        LOGI("ATrace Profiling Initialized");
    }
    else
    {
        LOGE("Failed to open libandroid.so");
    }
}

void ShutdownATrace()
{

}

#endif

