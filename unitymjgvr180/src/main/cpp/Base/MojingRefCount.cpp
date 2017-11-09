#include "MojingRefCount.h"
#include "MojingAtomic.h"
#include "MojingLog.h"


namespace Baofeng
{
	namespace Mojing
	{

#ifdef MJ_CC_ARM
		void* ReturnArg0(void* p)
		{
			return p;
		}
#endif

		// ***** Reference Count Base implementation

		RefCountImplCore::~RefCountImplCore()
		{
			// RefCount can be either 1 or 0 here.
			//  0 if Release() was properly called.
			//  1 if the object was declared on stack or as an aggregate.
			MJ_ASSERT(RefCount <= 1);
		}

#ifdef MJ_BUILD_DEBUG
		void RefCountImplCore::reportInvalidDelete(void *pmem)
		{
			MJ_DEBUG_LOG(
				("Invalid delete call on ref-counted object at %p. Please use Release()", pmem));
			MJ_ASSERT(0);
		}
#endif

		RefCountNTSImplCore::~RefCountNTSImplCore()
		{
			// RefCount can be either 1 or 0 here.
			//  0 if Release() was properly called.
			//  1 if the object was declared on stack or as an aggregate.
			MJ_ASSERT(RefCount <= 1);
		}

#ifdef MJ_BUILD_DEBUG
		void RefCountNTSImplCore::reportInvalidDelete(void *pmem)
		{
			MJ_DEBUG_LOG(
				("Invalid delete call on ref-counted object at %p. Please use Release()", pmem));
			MJ_ASSERT(0);
		}
#endif


		// *** Thread-Safe RefCountImpl

		void    RefCountImpl::AddRef()
		{
			AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, 1);
		}
		void    RefCountImpl::Release()
		{
			if ((AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, -1) - 1) == 0)
				delete this;
		}

		// *** Thread-Safe RefCountVImpl w/virtual AddRef/Release

		void    RefCountVImpl::AddRef()
		{
			AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, 1);
		}
		void    RefCountVImpl::Release()
		{
			if ((AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, -1) - 1) == 0)
				delete this;
		}

		// *** NON-Thread-Safe RefCountImpl

		void    RefCountNTSImpl::Release() const
		{
			RefCount--;
			if (RefCount == 0)
				delete this;
		}


	}
}
