#include "MojingSystem.h"
#include "MojingThreads.h"
#include "MojingTimer.h"


namespace Baofeng
{
	namespace Mojing
	{
		// *****  OVR::System Implementation

		// Initializes System core, installing allocator.
		void System::Init(Log* log, Allocator *palloc)
		{
			if (!Allocator::GetInstance())
			{
				Log::SetGlobalLog(log);
				Timer::initializeTimerSystem();
				Allocator::setInstance(palloc);
#ifdef MJ_ENABLE_THREADS
				Thread::InitThreadList();
#endif
			}
			else
			{
				MJ_DEBUG_LOG(("System::Init failed - duplicate call."));
			}
		}

		void System::Destroy()
		{
			if (Allocator::GetInstance())
			{
#ifdef MJ_ENABLE_THREADS
				// Wait for all threads to finish; this must be done so that memory
				// allocator and all destructors finalize correctly.
				Thread::FinishAllThreads();
#endif

				// Shutdown heap and destroy SysAlloc singleton, if any.
				Allocator::GetInstance()->onSystemShutdown();
				Allocator::setInstance(0);

				Timer::shutdownTimerSystem();
				Log::SetGlobalLog(Log::GetDefaultLog());
			}
			else
			{
				MJ_DEBUG_LOG(("System::Destroy failed - System not initialized."));
			}
		}

		// Returns 'true' if system was properly initialized.
		bool System::IsInitialized()
		{
			return Allocator::GetInstance() != 0;
		}
	}
} // OVR

