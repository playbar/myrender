#ifndef MJ_System_h
#define MJ_System_h

#include "MojingAllocator.h"
#include "MojingLog.h"

namespace Baofeng 
{
	namespace Mojing
	{

		// ***** System Core Initialization class

		// System initialization must take place before any other MJ_Kernel objects are used;
		// this is done my calling System::Init(). Among other things, this is necessary to
		// initialize the memory allocator. Similarly, System::Destroy must be
		// called before program exist for proper cleanup. Both of these tasks can be achieved by
		// simply creating System object first, allowing its constructor/destructor do the work.

		// TBD: Require additional System class for Oculus Rift API?

		class System
		{
		public:

			// System constructor expects allocator to be specified, if it is being substituted.
			System(Log* log = Log::ConfigureDefaultLog(LogMask_Debug),
				Allocator* palloc = DefaultAllocator::InitSystemSingleton())
			{
				Init(log, palloc);
			}

			~System()
			{
				Destroy();
			}

			// Returns 'true' if system was properly initialized.
			static bool MJ_CDECL IsInitialized();

			// Initializes System core.  Users can override memory implementation by passing
			// a different Allocator here.
			static void MJ_CDECL Init(Log* log = Log::ConfigureDefaultLog(LogMask_Debug),
				Allocator *palloc = DefaultAllocator::InitSystemSingleton());

			// De-initializes System more, finalizing the threading system and destroying
			// the global memory allocator.
			static void MJ_CDECL Destroy();
		};
	}
} // OVR

#endif
