#include "MojingAtomic.h"

// Include Windows 8-Metro compatible Synchronization API
#if defined(MJ_OS_WIN32) && defined(NTDDI_WIN8) && (NTDDI_VERSION >= NTDDI_WIN8)
#include <synchapi.h>
#endif


namespace Baofeng
{
	namespace Mojing
	{

		// ***** Windows Lock implementation

#if defined(MJ_OS_WIN32)

		// ***** Standard Win32 Lock implementation

		// Constructors
		Lock::Lock(unsigned spinCount)
		{
#if defined(NTDDI_WIN8) && (NTDDI_VERSION >= NTDDI_WIN8)
			// On Windows 8 we use InitializeCriticalSectionEx due to Metro-Compatibility
			InitializeCriticalSectionEx(&cs, spinCount,
				MJ_DEBUG_SELECT(NULL, CRITICAL_SECTION_NO_DEBUG_INFO));
#else
			// Spin count init critical section function prototype for Window NT
			typedef BOOL(WINAPI *Function_InitializeCriticalSectionAndSpinCount)
				(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);


			// Try to load function dynamically so that we don't require NT
			// On Windows NT we will use InitializeCriticalSectionAndSpinCount
			static  bool initTried = 0;
			static  Function_InitializeCriticalSectionAndSpinCount pInitFn = 0;

			if (!initTried)
			{
				HMODULE hmodule = ::LoadLibrary(MJ_STR("kernel32.dll"));
				pInitFn = (Function_InitializeCriticalSectionAndSpinCount)
					::GetProcAddress(hmodule, "InitializeCriticalSectionAndSpinCount");
				initTried = true;
			}

			// Initialize the critical section
			if (pInitFn)
				pInitFn(&cs, spinCount);
			else
				::InitializeCriticalSection(&cs);
#endif

		}


		Lock::~Lock()
		{
			DeleteCriticalSection(&cs);
		}


#endif

	}
}
