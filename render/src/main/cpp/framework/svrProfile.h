//=============================================================================
// FILE: svrProfile.h
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#ifndef _SVR_API_PROFILE_H_
#define _SVR_API_PROFILE_H_

#if defined(SVR_PROFILING_ENABLED)

#if defined(SVR_PROFILE_TELEMETRY) && defined(SVR_PROFILE_ATRACE)
#error "Cannot enable both ATrace and Telemetry profiling simultaneously"
#endif

#if defined(SVR_PROFILE_TELEMETRY)

#include "telemetry.h"

#define ERROR_BUILD_DISABLED    REQUIRED_REPLACE(TMERR_DISABLED, (TmErrorCode)0x0001)

#define LOG_EXCLAMATION         REQUIRED_REPLACE(TMMF_ICON_EXCLAMATION, 0)
#define LOG_NOTE                REQUIRED_REPLACE(TMMF_ICON_NOTE, 0)
#define LOG_WTF                 REQUIRED_REPLACE(TMMF_ICON_WTF, 0)

#define LOG_ZONE_SUBLABEL       REQUIRED_REPLACE(TMMF_ZONE_SUBLABEL, 0)
#define LOG_ZONE_LABEL          REQUIRED_REPLACE(TMMF_ZONE_LABEL, 0)

#define REQUIRED(required_code) required_code
#define REQUIRED_REPLACE(required_code, replacement_code) required_code

bool InitializeTelemetry();
void ShutdownTelemetry();

bool InitializeTmxGpu(void* device, unsigned int flags, int query_count, void* query_resources);

struct TelemetryData
{
    HTELEMETRY contexts[32];
    HTELEMETRY context;
};

extern TelemetryData gTelemetryData;

inline void EnableGroup(HTELEMETRY* context, bool enable)
{
    *context = enable ? gTelemetryData.context : 0;
}

#define GROUP_GENERIC               REQUIRED_REPLACE(gTelemetryData.context, 0)
#define GROUP_SENSORS               REQUIRED_REPLACE(gTelemetryData.contexts[0],  0)
#define GROUP_TIMEWARP              REQUIRED_REPLACE(gTelemetryData.contexts[1],  0)
#define GROUP_WORLDRENDER           REQUIRED_REPLACE(gTelemetryData.contexts[2],  0)

#define PROFILE_ENABLE_GROUP(group, enable) REQUIRED(EnableGroup(&group, enable)) 

#define PROFILE_FASTTIME() REQUIRED_REPLACE(tmFastTime(), 0);

#define PROFILE_INITIALIZE() REQUIRED_REPLACE(InitializeTelemetry(), ERROR_BUILD_DISABLED)

#define PROFILE_SHUTDOWN() REQUIRED(ShutdownTelemetry())

#define PROFILE_SCOPE_FILTERED(context, threshold, flags, format, ...) REQUIRED(tmZoneFiltered(context, threshold, flags, format, ##__VA_ARGS__))

#define PROFILE_SCOPE(context, flags, format, ...) REQUIRED(tmZone(context, flags, format, ##__VA_ARGS__))

#define PROFILE_SCOPE_DEFAULT(context) PROFILE_SCOPE(context, TMZF_NONE, __FUNCTION__)
#define PROFILE_SCOPE_IDLE(context) PROFILE_SCOPE(context, TMZF_IDLE, __FUNCTION__)
#define PROFILE_SCOPE_STALL(context) PROFILE_SCOPE(context, TMZF_STALL, __FUNCTION__)

#define PROFILE_TICK() REQUIRED(tmTick(gTelemetryData.context))

#define PROFILE_EXIT(context) REQUIRED(tmLeave(context))

#define PROFILE_EXIT_EX(context, match_id, thread_id, filename, line) REQUIRED(tmLeaveEx(context, match_id, thread_id, filename, line))

#define PROFILE_TRY_LOCK(context, ptr, lock_name, ... ) REQUIRED(tmTryLock(context, ptr, lock_name, ##__VA_ARGS__ ))

#define PROFILE_TRY_LOCK_EX(context, matcher, threshold, filename, line, ptr, lock_name, ... ) REQUIRED(tmTryLockEx(context, matcher, threshold, filename, line, ptr, lock_name, ##__VA_ARGS__))

#define PROFILE_END_TRY_LOCK(context, ptr, result ) REQUIRED( tmEndTryLock(context, ptr, result ) )

#define PROFILE_END_TRY_LOCK_EX(context, match_id, filename, line, ptr, result ) REQUIRED( tmEndTryLockEx(context, match_id, filename, line, ptr, result ) )

#define PROFILE_BEGIN_TIME_SPAN(context, id, flags, name_format, ... ) REQUIRED( tmBeginTimeSpan(context, id, flags, name_format, ##__VA_ARGS__ ) )

#define PROFILE_END_TIME_SPAN(context, id, flags, name_format, ... ) REQUIRED( tmEndTimeSpan(context, id, flags, name_format, ##__VA_ARGS__ ) )

#define PROFILE_BEGIN_TIME_SPAN_AT(context, id, flags, timestamp, name_format, ... ) REQUIRED( tmBeginTimeSpanAt(context, id, flags, timestamp, name_format, ##__VA_ARGS__ ) )

#define PROFILE_END_TIME_SPAN_AT(context, id, flags, timestamp, name_format, ... ) REQUIRED( tmEndTimeSpanAt(context, id, flags, timestamp, name_format, ##__VA_ARGS__ ) )

#define PROFILE_SIGNAL_LOCK_COUNT(context, ptr, count, description, ... ) REQUIRED( tmSignalLockCount(context, ptr, count, description, ##__VA_ARGS__ ) )

#define PROFILE_SET_LOCK_STATE(context, ptr, state, description, ... ) REQUIRED( tmSetLockState(context, ptr, state, description, ##__VA_ARGS__ ) )

#define PROFILE_SET_LOCK_STATE_EX(context, filename, line, ptr, state, description, ... ) REQUIRED( tmSetLockStateEx(context, filename, line, ptr, state, description, ##__VA_ARGS__ ) )

#define PROFILE_SET_LOCK_STATE_MIN_TIME(context, buf, ptr, state, description, ... ) REQUIRED( tmSetLockStateMinTime(context, buf, ptr, state, description, ##__VA_ARGS__ ) )

#define PROFILE_SET_LOCK_STATE_MIN_TIME_EX(context, buf, filename, line, ptr, state, description, ... ) REQUIRED(tmSetLockStateMinTimeEx(context, buf, filename, line, ptr, state, description, ##__VA_ARGS__ ) )

#define PROFILE_THREAD_NAME(context, thread_id, name_format, ... ) REQUIRED( tmThreadName(context, thread_id, name_format, ##__VA_ARGS__ ) )

#define PROFILE_LOCK_NAME(context, ptr, name_format, ... ) REQUIRED(tmLockName(context, ptr, name_format, ##__VA_ARGS__ ) )

#define PROFILE_EMIT_ACCUMULATION_ZONE(context, zone_flags, start, count, total, zone_format, ... ) REQUIRED(tmEmitAccumulationZone(context, zone_flags, start, count, total, zone_format, ##__VA_ARGS__ ) )

#define PROFILE_SET_VARIABLE(context, key, value_format, ... ) REQUIRED(tmSetVariable(context, key, value_format, ##__VA_ARGS__ ) )

#define PROFILE_SET_TIMELINE_SECTION_NAME(context, name_format, ... ) REQUIRED(tmSetTimelineSectionName(context, name_format, ##__VA_ARGS__ ) )

#define PROFILE_ENTER(context, flags, zone_name, ... ) REQUIRED( tmEnter(context, flags, zone_name, ##__VA_ARGS__ ) )

#define PROFILE_ENTER_EX(context, match_id, thread_id, threshold, filename, line, flags, zone_name, ... ) REQUIRED(tmEnterEx(context, match_id, thread_id, threshold, filename, line, flags, zone_name, ##__VA_ARGS__ ) )

#define PROFILE_ALLOC(context, ptr, size, description, ... ) REQUIRED( tmAlloc(context, ptr, size, description, ##__VA_ARGS__ ) )

#define PROFILE_ALLOC_EX(context, filename, line_number, ptr, size, description, ... ) REQUIRED(tmAllocEx(context, filename, line_number, ptr, size, description, ##__VA_ARGS__ ) )

#define PROFILE_FREE(context, ptr) REQUIRED(tmFree(context, ptr))

#define PROFILE_MESSAGE(context, flags, format_string, ... ) REQUIRED(tmMessage(context, flags, format_string, __VA_ARGS__ ))
#define PROFILE_LOG(context, flags, format_string, ... ) PROFILE_MESSAGE(context, TMMF_SEVERITY_LOG|flags, format_string, __VA_ARGS__ )
#define PROFILE_WARNING(context, flags, format_string, ... ) PROFILE_MESSAGE(context, TMMF_SEVERITY_WARNING|flags, format_string, __VA_ARGS__ )
#define PROFILE_ERROR(context, flags, format_string, ... ) PROFILE_MESSAGE(context, TMMF_SEVERITY_ERROR|flags, format_string, __VA_ARGS__ )

#define PROFILE_PLOT(context, type, flags, value, name_format, ... ) REQUIRED( tmPlot(context, type, flags, value, name_format, ##__VA_ARGS__ ) )

#define PROFILE_PLOT_F32(context, type, flags, value, name_format, ... ) REQUIRED( tmPlotF32(context, type, flags, value, name_format, ##__VA_ARGS__ ) )

#define PROFILE_PLOT_F64(context, type, flags, value, name_format, ... ) REQUIRED( tmPlotF64(context, type, flags, value, name_format, ##__VA_ARGS__ ) )

#define PROFILE_PLOT_I32(context, type, flags, value, name_format, ... ) REQUIRED( tmPlotI32(context, type, flags, value, name_format, ##__VA_ARGS__ ) )

#define PROFILE_PLOT_U32(context, type, flags, value, name_format, ... ) REQUIRED( tmPlotU32(context, type, flags, value, name_format, ##__VA_ARGS__ ) )

#define PROFILE_PLOT_I64(context, type, flags, value, name_format, ... ) REQUIRED( tmPlotI64(context, type, flags, value, name_format, ##__VA_ARGS__ ) )

#define PROFILE_PLOT_U64(context, type, flags, value, name_format, ... ) REQUIRED( tmPlotU64(context, type, flags, value, name_format, ##__VA_ARGS__ ) )

#define PROFILE_PLOT_AT(context, timestamp, type, flags, value, name_format, ... ) REQUIRED ( tmPlotAt(context, timestamp, type, flags, value, name_format, ##__VA_ARGS__ ) )

#define PROFILE_BLOB(context, data, data_size, plugin_identifier, blob_name, ...) REQUIRED( tmBlob(context, data, data_size, plugin_identifier, blob_name, ##__VA_ARGS__ ) )

#define PROFILE_DISJOINT_BLOB(context, num_pieces, data, data_sizes, plugin_identifier, blob_name, ... ) REQUIRED( tmDisjointBlob(context, num_pieces, data, data_sizes, plugin_identifier, blob_name, ##__VA_ARGS__ ) )

#define PROFILE_SEND_CALLSTACK(context, callstack) REQUIRED(tmSendCallStack(context, callstack))

#define PROFILE_DYNAMIC_STRING(buffer) REQUIRED_REPLACE(tmDynamicString(gTelemetryData.context, buffer), buffer)

//End SVR_PROFILE_TELEMETRY
#elif defined(SVR_PROFILE_ATRACE)

#include <time.h>
//#include "android/trace.h"

#define ATRACE_TAG ATRACE_TAG_GRAPHICS

extern void *(*ATrace_beginSection) (const char* sectionName);
extern void *(*ATrace_endSection) (void);
extern void *(*ATrace_isEnabled) (void);

void InitializeATrace();
void ShutdownATrace();

#define ATRACE_INT(name,val)
#define ATRACE_INT64(name,val)

#define GROUP_GENERIC
#define GROUP_SENSORS
#define GROUP_TIMEWARP
#define GROUP_WORLDRENDER

//PROFILE_ENABLE_GROUP
#define PROFILE_ENABLE_GROUP(group, enable)

#define PROFILE_FASTTIME()

//PROFILE_INITIALIZE
#define PROFILE_INITIALIZE() InitializeATrace()

//PROFILE_SHUTDOWN
#define PROFILE_SHUTDOWN() ShutdownATrace()

#define PROFILE_SCOPE_FILTERED(context, threshold, flags, format, ...)

//PROFILE_SCOPE

#define SVR_ATRACE_BEGIN(format, ...) \
   { \
       static char atrace_begin_buf[1024]; \
	   if (strchr(format,'%')){ \
		   snprintf(atrace_begin_buf, 1024, format,##__VA_ARGS__); \
		   ATrace_beginSection(atrace_begin_buf); \
	   }else{ \
	       ATrace_beginSection(format); \
	   } \
   }

#define PROFILE_SCOPE(context, flags, format, ...) SVR_ATRACE_BEGIN(format,##__VA_ARGS__)

#define PROFILE_SCOPE_END() ATrace_endSection()

//PROFILE_SCOPE_DEFAULT
#define PROFILE_SCOPE_DEFAULT(context) PROFILE_SCOPE(context, TMZF_NONE, __FUNCTION__)
#define PROFILE_SCOPE_IDLE(context)
#define PROFILE_SCOPE_STALL(context)

//PROFILE_TICK
#define PROFILE_TICK()
/*
{ \
    struct timespec now; \
    clock_gettime(CLOCK_MONOTONIC, &now); \
    ATRACE_INT64("svr_tick_ns" ,(int64_t) now.tv_sec*1000000000LL + now.tv_nsec); \
    }
*/

// PROFILE_EXIT
#define PROFILE_EXIT(context) ATrace_endSection()

#define PROFILE_EXIT_EX(context, match_id, thread_id, filename, line)

#define PROFILE_TRY_LOCK(context, ptr, lock_name, ... )

#define PROFILE_TRY_LOCK_EX(context, matcher, threshold, filename, line, ptr, lock_name, ... )

#define PROFILE_END_TRY_LOCK(context, ptr, result )

#define PROFILE_END_TRY_LOCK_EX(context, match_id, filename, line, ptr, result )

// PROFILE_BEGIN_TIME_SPAN_AT
#define PROFILE_BEGIN_TIME_SPAN(context, id, flags, name_format, ... )

// PROFILE_END_TIME_SPAN
#define PROFILE_END_TIME_SPAN(context, id, flags, name_format, ... )

#define PROFILE_BEGIN_TIME_SPAN_AT(context, id, flags, timestamp, name_format, ... )

#define PROFILE_END_TIME_SPAN_AT(context, id, flags, timestamp, name_format, ... )

#define PROFILE_SIGNAL_LOCK_COUNT(context, ptr, count, description, ... )

#define PROFILE_SET_LOCK_STATE(context, ptr, state, description, ... )

#define PROFILE_SET_LOCK_STATE_EX(context, filename, line, ptr, state, description, ... )

#define PROFILE_SET_LOCK_STATE_MIN_TIME(context, buf, ptr, state, description, ... )

#define PROFILE_SET_LOCK_STATE_MIN_TIME_EX(context, buf, filename, line, ptr, state, description, ... )

// PROFILE_THREAD_NAME
#define PROFILE_THREAD_NAME(context, thread_id, name_format, ... ) ATRACE_INT(name_format, thread_id)

#define PROFILE_LOCK_NAME(context, ptr, name_format, ... )

#define PROFILE_EMIT_ACCUMULATION_ZONE(context, zone_flags, start, count, total, zone_format, ... )

#define PROFILE_SET_VARIABLE(context, key, value_format, ... )

#define PROFILE_SET_TIMELINE_SECTION_NAME(context, name_format, ... )

//PROFILE_ENTER
#define PROFILE_ENTER(context, flags, zone_name, ... ) SVR_ATRACE_BEGIN(zone_name,##__VA_ARGS__)

#define PROFILE_ENTER_EX(context, match_id, thread_id, threshold, filename, line, flags, zone_name, ... )

#define PROFILE_ALLOC(context, ptr, size, description, ... )

#define PROFILE_ALLOC_EX(context, filename, line_number, ptr, size, description, ... )

#define PROFILE_FREE(context, ptr) ATrace_endSection()

//PROFILE_MESSAGE
#define PROFILE_MESSAGE(context, flags, format_string, ... )
#define PROFILE_LOG(context, flags, format_string, ... )
#define PROFILE_WARNING(context, flags, format_string, ... )
#define PROFILE_ERROR(context, flags, format_string, ... )

#define PROFILE_PLOT(context, type, flags, value, name_format, ... )

#define PROFILE_PLOT_F32(context, type, flags, value, name_format, ... )

#define PROFILE_PLOT_F64(context, type, flags, value, name_format, ... )

#define PROFILE_PLOT_I32(context, type, flags, value, name_format, ... ) ATRACE_INT(name_format, value)

#define PROFILE_PLOT_U32(context, type, flags, value, name_format, ... ) ATRACE_INT(name_format, (int)value)

#define PROFILE_PLOT_I64(context, type, flags, value, name_format, ... ) ATRACE_INT64(name_format, value)

#define PROFILE_PLOT_U64(context, type, flags, value, name_format, ... ) ATRACE_INT64(name_format, (int64_t)value)

#define PROFILE_PLOT_AT(context, timestamp, type, flags, value, name_format, ... )

#define PROFILE_BLOB(context, data, data_size, plugin_identifier, blob_name, ...)

#define PROFILE_DISJOINT_BLOB(context, num_pieces, data, data_sizes, plugin_identifier, blob_name, ... )

#define PROFILE_SEND_CALLSTACK(context, callstack)

#define PROFILE_DYNAMIC_STRING(buffer)

#endif //defined(SVR_PROFILE_ATRACE)

#else  //defined(SVR_PROFILING_ENABLED)

#define GROUP_GENERIC              
#define GROUP_SENSORS              
#define GROUP_TIMEWARP              
#define GROUP_WORLDRENDER           

#define PROFILE_ENABLE_GROUP(group, enable) 

#define PROFILE_FASTTIME() 

#define PROFILE_INITIALIZE() 

#define PROFILE_SHUTDOWN() 

#define PROFILE_SCOPE_FILTERED(context, threshold, flags, format, ...) 

#define PROFILE_SCOPE(context, flags, format, ...) 

#define PROFILE_SCOPE_DEFAULT(context) 
#define PROFILE_SCOPE_IDLE(context)
#define PROFILE_SCOPE_STALL(context) 

#define PROFILE_TICK()

#define PROFILE_EXIT(context) 

#define PROFILE_EXIT_EX(context, match_id, thread_id, filename, line)

#define PROFILE_TRY_LOCK(context, ptr, lock_name, ... ) 

#define PROFILE_TRY_LOCK_EX(context, matcher, threshold, filename, line, ptr, lock_name, ... ) 

#define PROFILE_END_TRY_LOCK(context, ptr, result ) 

#define PROFILE_END_TRY_LOCK_EX(context, match_id, filename, line, ptr, result )

#define PROFILE_BEGIN_TIME_SPAN(context, id, flags, name_format, ... ) 

#define PROFILE_END_TIME_SPAN(context, id, flags, name_format, ... ) 

#define PROFILE_BEGIN_TIME_SPAN_AT(context, id, flags, timestamp, name_format, ... ) 

#define PROFILE_END_TIME_SPAN_AT(context, id, flags, timestamp, name_format, ... ) 

#define PROFILE_SIGNAL_LOCK_COUNT(context, ptr, count, description, ... ) 

#define PROFILE_SET_LOCK_STATE(context, ptr, state, description, ... ) 

#define PROFILE_SET_LOCK_STATE_EX(context, filename, line, ptr, state, description, ... ) 

#define PROFILE_SET_LOCK_STATE_MIN_TIME(context, buf, ptr, state, description, ... ) 

#define PROFILE_SET_LOCK_STATE_MIN_TIME_EX(context, buf, filename, line, ptr, state, description, ... ) 

#define PROFILE_THREAD_NAME(context, thread_id, name_format, ... ) 

#define PROFILE_LOCK_NAME(context, ptr, name_format, ... ) 

#define PROFILE_EMIT_ACCUMULATION_ZONE(context, zone_flags, start, count, total, zone_format, ... ) 

#define PROFILE_SET_VARIABLE(context, key, value_format, ... ) 

#define PROFILE_SET_TIMELINE_SECTION_NAME(context, name_format, ... ) 

#define PROFILE_ENTER(context, flags, zone_name, ... ) 

#define PROFILE_ENTER_EX(context, match_id, thread_id, threshold, filename, line, flags, zone_name, ... ) 

#define PROFILE_ALLOC(context, ptr, size, description, ... ) 

#define PROFILE_ALLOC_EX(context, filename, line_number, ptr, size, description, ... ) 

#define PROFILE_FREE(context, ptr) 

#define PROFILE_MESSAGE(context, flags, format_string, ... ) 
#define PROFILE_LOG(context, flags, format_string, ... ) 
#define PROFILE_WARNING(context, flags, format_string, ... ) 
#define PROFILE_ERROR(context, flags, format_string, ... ) 

#define PROFILE_PLOT(context, type, flags, value, name_format, ... ) 

#define PROFILE_PLOT_F32(context, type, flags, value, name_format, ... )

#define PROFILE_PLOT_F64(context, type, flags, value, name_format, ... ) 

#define PROFILE_PLOT_I32(context, type, flags, value, name_format, ... ) 

#define PROFILE_PLOT_U32(context, type, flags, value, name_format, ... )

#define PROFILE_PLOT_I64(context, type, flags, value, name_format, ... ) 

#define PROFILE_PLOT_U64(context, type, flags, value, name_format, ... ) 

#define PROFILE_PLOT_AT(context, timestamp, type, flags, value, name_format, ... ) 

#define PROFILE_BLOB(context, data, data_size, plugin_identifier, blob_name, ...) 

#define PROFILE_DISJOINT_BLOB(context, num_pieces, data, data_sizes, plugin_identifier, blob_name, ... )

#define PROFILE_SEND_CALLSTACK(context, callstack) 

#define PROFILE_DYNAMIC_STRING(buffer) 

#endif //defined(SVR_PROFILING_ENABLED)

#endif //_SVR_API_PROFILE_H_