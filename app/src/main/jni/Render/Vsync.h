#ifndef MJ_Vsync_h
#define MJ_Vsync_h

#include "../Base/MojingLockless.h"	// for LocklessUpdater
namespace Baofeng
{
	namespace Mojing
	{
#define DISPLY_FRAME_RATE_60		((double)60)
#define DISPLY_FRAME_RATE_75		((double)75)
#define DISPLY_FRAME_RATE			DISPLY_FRAME_RATE_60
#define DISPLY_FRAME_TIME_IN_NS	(1e9/(DISPLY_FRAME_RATE))
#define DISPLY_FRAME_TIME_IN_MS	(((double)1e3)/(DISPLY_FRAME_RATE))
#define DISPLY_FRAME_TIME_IN_SEC	(1.0/DISPLY_FRAME_RATE)
#define DISPLY_FRAME_TIME_AFTER_DRAW	(1 * DISPLY_FRAME_TIME_IN_SEC)
		class VsyncState
		{
		public:
			VsyncState() :vsyncPeriodNano(DISPLY_FRAME_TIME_IN_NS), vsyncCount(0), vsyncBaseNano(0){};
			long long vsyncCount;
			double	vsyncPeriodNano;
			double	vsyncBaseNano;
		};

		// This can be read without any locks, so a high priority rendering thread doesn't
		// have to worry about being blocked by a sensor thread that got preempted.
		extern LocklessUpdater<VsyncState>	UpdatedVsyncState;


		VsyncState GetVsyncState();
		// Estimates the current vsync count and fraction based on the most
		// current timing provided from java.  This does not interact with
		// the JVM at all.
		double			GetFractionalVsync();

		// Returns a time in seconds for the given fractional frame.
		double			FramePointTimeInSeconds(const double framePoint);

		// Returns the fractional frame for the given time in seconds.
		double			FramePointTimeInSecondsWithBlanking(const double framePoint);

		// Does a nanosleep() that will wake shortly after the given targetSeconds.
		// Returns the seconds that were requested to sleep, which will be <=
		// the time actually slept, which may be negative if already past the
		// framePoint.
		float 			SleepUntilTimePoint(const double targetSeconds, const bool busyWait);

	}	
}
#endif

