#include "Vsync.h"

#include <stdlib.h>

#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>			// for usleep
#include <sys/time.h>
#if defined(MJ_OS_WIN32)
#else
#include <sys/resource.h>
#endif
#include "../Base/MojingTimer.h"
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif

/*
 * As of 6/30/2014, I am seeing vsync frame timings of 16.71 ms for the 1080 S5,
 * and 16.91 ms for the 1440 s5, both of which are farther from the expected
 * 16.66 ms than I would like.
 */

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
static bool LogVsync = true;
#else
static bool LogVsync = false;
#endif // ENABLE_LOGGER

#ifdef MJ_OS_ANDROID
#include <jni.h>
extern "C"
{
	// The nativeVsync function is called from java with timing
	// information that GetFractionalVsync() and FramePointTimeInSeconds()
	// can use to closely estimate raster position.
	void Java_com_baofeng_mojing_MojingVrLib_nativeVsync(JNIEnv *jni, jclass clazz, jlong frameTimeNanos)
	{
// 		if ( LogVsync )
// 		{
// 			static long long prevFrameTimeNanos;
// 			char szLog[64];
// 			sprintf(szLog , "SetVsync: %5.5f ms", (frameTimeNanos - prevFrameTimeNanos) * 0.000001);
// 			MOJING_TRACE(g_APIlogger, szLog);
// 			prevFrameTimeNanos = frameTimeNanos;
// 		}
// 
		Baofeng::Mojing::VsyncState	state = Baofeng::Mojing::UpdatedVsyncState.GetState();

		// Round this, because different phone models have slightly different periods.
		state.vsyncCount += floor( 0.5 + ( frameTimeNanos - state.vsyncBaseNano ) / state.vsyncPeriodNano );
		state.vsyncPeriodNano = DISPLY_FRAME_TIME_IN_NS;
		state.vsyncBaseNano = frameTimeNanos;

		Baofeng::Mojing::UpdatedVsyncState.SetState(state);
	}
}	// extern "C"
#endif // MJ_OS_ANDROID

namespace Baofeng
{
	namespace Mojing
	{
		// This can be read without any locks, so a high priority rendering thread doesn't
		// have to worry about being blocked by a sensor thread that got preempted.
		LocklessUpdater<VsyncState> UpdatedVsyncState;

		// This is separate to allow easy switching to a fixed value.
		VsyncState GetVsyncState()
		{
			if (0)
			{	// constant
				static VsyncState	state;
				if (state.vsyncBaseNano == 0) {
					state.vsyncBaseNano = Timer::GetTicksNanos();
					state.vsyncPeriodNano = DISPLY_FRAME_TIME_IN_NS;
				}
				return state;
			}
			else
			{	// normal update
				return UpdatedVsyncState.GetState();
			}
		}
		// 返回以当前时间为参考累计同步帧数
		double GetFractionalVsync()
		{
			const VsyncState state = GetVsyncState();

			const int64_t t = Timer::GetTicksNanos();
			if (state.vsyncBaseNano == 0)
			{
				return 0;
			}
			const double vsync = (double)state.vsyncCount + (double)(t - state.vsyncBaseNano) / state.vsyncPeriodNano;
			return vsync;
		}
		// 返回以秒为单位的累计第framePoint帧的预计输出时间,0表示下一帧
		double FramePointTimeInSeconds(const double framePoint)
		{
			const VsyncState state = GetVsyncState();
			const double seconds = (state.vsyncBaseNano + (framePoint - state.vsyncCount) * state.vsyncPeriodNano) * 1e-9;
			return seconds;
		}

		double FramePointTimeInSecondsWithBlanking(const double framePoint)
		{
			const VsyncState state = GetVsyncState();
			static const double startBias = 0.0; // 8.0/1920.0/60.0;	// about 8 pixels into a 1920 screen at 60 hz
			static const double activeFraction = 112.0 / 135;
			const double seconds = (state.vsyncBaseNano + activeFraction * (framePoint - state.vsyncCount) * state.vsyncPeriodNano) * 1e-9 + startBias;
			return seconds;
		}
		// 睡到指定的时间点
		float SleepUntilTimePoint(const double targetSeconds, const bool busyWait)
		{
			const float sleepSeconds = targetSeconds - Timer::GetSeconds();
			if (sleepSeconds > 0)
			{
				if (busyWait)
				{
					while (targetSeconds - Timer::GetSeconds() > 0)
					{
					}
				}
				else
				{
					// I'm assuming we will never sleep more than one full second.
					timespec	t, rem;
					t.tv_sec = 0;
					t.tv_nsec = sleepSeconds * 1e9;
#ifndef MJ_OS_WIN32
					nanosleep(&t, &rem);
#endif
					const double overSleep = Timer::GetSeconds() - targetSeconds;
					if (overSleep > 0.001)
					{
				//			LOG( "Overslept %f seconds", overSleep );
					}
				}
			}
			return sleepSeconds;
		}
	}
}
