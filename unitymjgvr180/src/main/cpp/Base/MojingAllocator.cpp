#include "MojingAllocator.h"
#include "MojingLog.h"
#ifdef MJ_OS_MAC
 #include <stdlib.h>
#else
#include <malloc.h>
#include <string.h>

#endif


#include <stdio.h>

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
		DefaultAllocator::~DefaultAllocator()
		{
#if defined (_DEBUG) 
			bool bNoLeake = true;
			while (m_AllocTable.size() != 0)
			{
				bNoLeake = false;
				__tagAllocatorInfo AI = m_AllocTable.begin()->second;
#ifdef MJ_OS_ANDROID
				__android_log_print(ANDROID_LOG_INFO, "Allocator", "Memory leake at : 0x%08X , Size = %d bytes , Alloc at %s , line %d",
					(char*)AI.m_pPosition + 32, AI.m_iSize, AI.m_pszFile, AI.m_iLine);
#else
				printf("Memory leake at : 0x%08I64X , Size = %d bytes , Alloc at %s , line %d",
					(char*)AI.m_pPosition + 32, AI.m_iSize, AI.m_pszFile, AI.m_iLine);
#endif
				free(AI.m_pszFile);
				free(AI.m_pPosition);
				m_AllocTable.erase(m_AllocTable.begin());
			}
			if (!bNoLeake)
			{
				MJ_ASSERT(0);
			}
#endif
		}
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

			__tagAllocatorInfo AI;
			AI.m_iLine = line;
			AI.m_pszFile = (char*)malloc(strlen(file) + 1 + 32);
			strcpy(AI.m_pszFile, file);

			AI.m_iSize = size;
			int iAllocSize = (size / 64 + 1) * 64;
			if ((size % 64) != 0)
			{
				iAllocSize += 64;
			}
			AI.m_iAllocSize = iAllocSize;

			unsigned char *pBuffer = (unsigned char *)malloc(iAllocSize);
			memset(pBuffer , 0xAA , 32);
			memset(pBuffer + 32, 0xBB, size); 
			memset(pBuffer + 32 + size, 0xCC, iAllocSize - size - 32);
			AI.m_pPosition = pBuffer;


			void * pRet =  ((unsigned char *)AI.m_pPosition) + 32;
#ifdef MJ_OS_ANDROID
			//__android_log_print(ANDROID_LOG_INFO, "Allocator", "Append Address 0x%08lX ", (long long)pRet);
#else
			printf("Append Address 0x%08I64X ", (long long)pRet);
#endif
			BeforeMapOP();
			m_AllocTable[pRet] = AI;
			AfterMapOP();
			return pRet;
#endif
			 
		}
		void DefaultAllocator::FreeDebug(void *p)
		{
#if defined(MJ_CC_MSVC) && defined(_CRTDBG_MAP_ALLOC)
			free(p);
#else 
			BeforeMapOP();
#ifdef MJ_OS_ANDROID
			//__android_log_print(ANDROID_LOG_INFO, "Allocator", "Remove Address 0x%08lX", (long)p);
#else
			printf("Remove Address 0x%08I64X", (long long)p);
#endif
			map<void*, __tagAllocatorInfo>::iterator it = m_AllocTable.find(p);
			if (it == m_AllocTable.end())
			{
#ifdef MJ_OS_ANDROID
				MJ_ASSERT_LOG(it != m_AllocTable.end(), ("Address 0x%08lX not Alloc !! ", (long)p));
#else
				printf("Address 0x%08I64X not Alloc !! ", (long long)p);
#endif
			}
			
			__tagAllocatorInfo AI = it->second;
			// HEAD CHECK
			int iCheck = 0;
			while (iCheck < 32)
			{
#ifdef MJ_OS_ANDROID
				MJ_ASSERT_LOG(AI.m_pPosition[iCheck++] == 0xAA, ("Memory head check FAILD !! Size = %d bytes , Alloc at %s , line %d", AI.m_iSize , AI.m_pszFile, AI.m_iLine));
#else
				printf("Memory head check FAILD !! Size = %d bytes , Alloc at %s , line %d", AI.m_iSize , AI.m_pszFile, AI.m_iLine);
#endif
			}

			iCheck = AI.m_iSize + 32;

			while (iCheck < AI.m_iAllocSize)
			{
#ifdef MJ_OS_ANDROID
				MJ_ASSERT_LOG(AI.m_pPosition[iCheck++] == 0xCC, ("Memory end check FAILD !! Size = %d bytes , Alloc at %s , line %d", AI.m_iSize, AI.m_pszFile, AI.m_iLine));
#else
				printf("Memory end check FAILD !! Size = %d bytes , Alloc at %s , line %d", AI.m_iSize, AI.m_pszFile, AI.m_iLine);
#endif
			}

			memset(AI.m_pPosition, 0xDD, AI.m_iAllocSize);
			m_AllocTable.erase(it);
			
			free(AI.m_pszFile);
			free(AI.m_pPosition);
			
			AfterMapOP();
#endif
		}
		void* DefaultAllocator::Realloc(void* p, UPInt newSize)
		{
			return realloc(p, newSize);
		}
		void* DefaultAllocator::ReallocDebug(void* p, UPInt newSize)
		{	
#if defined(MJ_CC_MSVC) && defined(_CRTDBG_MAP_ALLOC)
			return realloc(p, newSize);
#else
			void *pRet = NULL;
			BeforeMapOP();
			map<void*, __tagAllocatorInfo>::iterator it = m_AllocTable.find(p);
			if (it == m_AllocTable.end())
			{
				printf("Address 0x%08I64X not Alloc !! ", (long long)p);
			}
			else
			{
				__tagAllocatorInfo AI = it->second;
				// HEAD CHECK
				int iCheck = 0;
				while (iCheck < 32)
				{
#ifdef MJ_OS_ANDROID
					MJ_ASSERT_LOG(AI.m_pPosition[iCheck++] == 0xAA, ("Memory head check FAILD !! Size = %d bytes , Alloc at %s , line %d", AI.m_iSize, AI.m_pszFile, AI.m_iLine));
#else
					printf("Memory head check FAILD !! Size = %d bytes , Alloc at %s , line %d", AI.m_iSize, AI.m_pszFile, AI.m_iLine);
#endif
				}

				iCheck = AI.m_iSize + 32;

				while (iCheck < AI.m_iAllocSize)
				{
#ifdef MJ_OS_ANDROID
					MJ_ASSERT_LOG(AI.m_pPosition[iCheck++] == 0xCC, ("Memory end check FAILD !! Size = %d bytes , Alloc at %s , line %d", AI.m_iSize, AI.m_pszFile, AI.m_iLine));
#else
					printf("Memory end check FAILD !! Size = %d bytes , Alloc at %s , line %d", AI.m_iSize, AI.m_pszFile, AI.m_iLine);
#endif
				}
				int iNewSize = (newSize / 64 +1 )* 64;
				if (newSize % 64 != 0)
					iNewSize += 64;
				
				unsigned char * pNewPos = (unsigned char *)realloc(AI.m_pPosition, iNewSize);
				MJ_ASSERT_LOG(pNewPos != NULL , (" Realloc FAILED!"));
				if (pNewPos != NULL)
				{
					if (NULL == strstr(AI.m_pszFile, "--Reallocd--"))
					{
						strcpy(AI.m_pszFile, "--Reallocd--");
					}
					it->second = AI;

					AI.m_pPosition = pNewPos;
					AI.m_iSize = iNewSize;
					memset(AI.m_pPosition, 0xAA, 32);
					memset(AI.m_pPosition + 32 + newSize, 0xCC, iNewSize - newSize - 32);
					
					pRet = AI.m_pPosition + 32;
					m_AllocTable[pRet] = AI;
				}
				else
				{
					
				}
			}
			AfterMapOP();
			return pRet;
#endif
		}
		void DefaultAllocator::Free(void *p)
		{
			return free(p);
		}


	}
}
