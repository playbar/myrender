#include "MojingAllocator.h"
#ifdef MJ_OS_MAC
 #include <stdlib.h>
#else
 #include <malloc.h>
#endif

namespace Baofeng
{
	namespace Mojing
	{

		//-----------------------------------------------------------------------------------
		// ***** Allocator

		Allocator* Allocator::pInstance = 0;

		// Default AlignedAlloc implementation will delegate to Alloc/Free after doing rounding.
		void* Allocator::AllocAligned(UPInt size, UPInt align)
		{
			MJ_ASSERT((align & (align - 1)) == 0);
			align = (align > sizeof(UPInt)) ? align : sizeof(UPInt);
			UPInt p = (UPInt)Alloc(size + align);
			UPInt aligned = 0;
			if (p)
			{
				aligned = (UPInt(p) + align - 1) & ~(align - 1);
				if (aligned == p)
					aligned += align;
				*(((UPInt*)aligned) - 1) = aligned - p;
			}
			return (void*)aligned;
		}

		void Allocator::FreeAligned(void* p)
		{
			UPInt src = UPInt(p) - *(((UPInt*)p) - 1);
			Free((void*)src);
		}


		//------------------------------------------------------------------------
		// ***** Default Allocator

		// This allocator is created and used if no other allocator is installed.
		// Default allocator delegates to system malloc.

		void* DefaultAllocator::Alloc(UPInt size)
		{
			return malloc(size);
		}
		void* DefaultAllocator::AllocDebug(UPInt size, const char* file, unsigned line)
		{
#if defined(MJ_CC_MSVC) && defined(_CRTDBG_MAP_ALLOC)
			return _malloc_dbg(size, _NORMAL_BLOCK, file, line);
#else
			MJ_UNUSED2(file, line);
			return malloc(size);
#endif
		}

		void* DefaultAllocator::Realloc(void* p, UPInt newSize)
		{
			return realloc(p, newSize);
		}
		void DefaultAllocator::Free(void *p)
		{
			return free(p);
		}


	}
}
