#include "HookBase.h"
#include <dlfcn.h>
#include "../Global/detour.h"
#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif
HookBase::HookBase()
{
}


HookBase::~HookBase()
{
}
bool HookBase::HookToFunctions(void * hDLL, HookParamet * pHookParamet, int iFunctionCount)
{
#ifdef _DEBUG
	MOJING_FUNC_TRACE(g_APIlogger);
#endif
	char szLog[512];

	bool bRet = false;
	if (hDLL != NULL)
	{
		for (int i = 0; i < iFunctionCount; i++)
		{
			HookParamet * pHookItem = pHookParamet + i;
			void *pFunc = dlsym(hDLL, pHookItem->szFunctionName);
			if (pFunc != NULL)
			{
				sprintf(szLog, "0x%08X(hDll) : %s ,  0x%08X(Real) --> 0x%08X(Hook)", (uint32_t)hDLL, pHookItem->szFunctionName, (uint32_t)pFunc, (uint32_t)pHookItem->fpHookToFunction);
#ifdef _DEBUG				
				MOJING_TRACE(g_APIlogger, "Try hook function : " << szLog);
#endif
				if (registerInlineHook((uint32_t)pFunc,
					(uint32_t)pHookItem->fpHookToFunction,
					(uint32_t **)&pHookItem->fpRealFunction) == DETOUR_OK)
				{
#ifdef _DEBUG
					MOJING_TRACE(g_APIlogger, "registerInlineHook OK = " << szLog);
#endif
					bRet = true;
				}
				else
				{
					MOJING_ERROR(g_APIlogger, "registerInlineHook FAILD = " << szLog);
					bRet = false;
					break;
				}
				usleep(10*1000);
			}
		}
		if (bRet)
		{
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "Call inlineHookAll ... = " << szLog);
#endif
			inlineHookAll();
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "Call inlineHookAll ... Done = " << szLog);
#endif
		}
	}

	return bRet;
}
bool HookBase::HookToFunction(void * hDLL, const char * szFunctionName, void * fpReplactToFunction, void ** fpOutRealFunction)
{
#ifdef _DEBUG
	char szLog[512];
#endif
	bool bRet = false;
	if (hDLL != NULL && szFunctionName != NULL && fpOutRealFunction != NULL)
	{
		void *pFunc = dlsym(hDLL, szFunctionName);

		
		if (pFunc != NULL)
		{
#ifdef _DEBUG
			sprintf(szLog, "0x%08X(hDll) : %s ,  0x%08X(Real) --> 0x%08X(Hook)", (uint32_t)hDLL, szFunctionName, (uint32_t)pFunc, (uint32_t)fpReplactToFunction);
			MOJING_TRACE(g_APIlogger , "Try hook function : " << szLog);
#endif
			if (registerInlineHook((uint32_t)pFunc,
				(uint32_t)fpReplactToFunction,
				(uint32_t **)fpOutRealFunction) == DETOUR_OK)
			{
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "registerInlineHook OK = " << szLog);
#endif

				if (inlineHook((uint32_t)pFunc) == DETOUR_OK)
				{
#ifdef _DEBUG
					MOJING_TRACE(g_APIlogger, "inlineHook OK = " << szLog);
#endif
					bRet = true;
				}
				else
				{
					MOJING_ERROR(g_APIlogger , "Try inlineHook error!! Fucntion name = " << szFunctionName);
				}
			}
			else
			{
				MOJING_ERROR(g_APIlogger, "Try registerInlineHook error!! Fucntion name = " << szFunctionName);
			}
		}
		else
		{
			MOJING_ERROR(g_APIlogger, "Can not find and Fucntion name = " << szFunctionName);
		}
	}
	else
	{
		MOJING_ERROR(g_APIlogger, "Invalid parmeat!!");
	}
	return bRet;
}


bool phrase_dev_num(char *devno, int *pmajor, int *pminor) {
	*pmajor = 0;
	*pminor = 0;
	if (devno != NULL) {
		char *second = strstr(devno, ":");
		if (second != NULL) {
			*pmajor = strtoul(devno + 0, NULL, 16);
			*pminor = strtoul(second + 1, NULL, 16);
			return true;
		}
	}
	return false;
}


bool phrase_proc_base_addr(char *addr, void **pbase_addr, void **pend_addr) {
	char *split = strchr(addr, '-');
	if (split != NULL) {
		if (pbase_addr != NULL) {
			*pbase_addr = (void *)strtoul(addr, NULL, 16);
		}
		if (pend_addr != NULL) {
			*pend_addr = (void *)strtoul(split + 1, NULL, 16);
		}
		return true;
	}
	return false;
}

void* getimagebase(const char *target) {
	FILE *fd = fopen("/proc/self/maps", "r");
	if (fd != NULL) {
		char buff[2048 + 1];
		while (fgets(buff, 2048, fd) != NULL) {
			const char *sep = "\t \r\n";
			char *line = NULL;
			char *addr = strtok_r(buff, sep, &line);
			if (!addr) {
				continue;
			}

			char *flags = strtok_r(NULL, sep, &line);
			if (!flags || flags[0] != 'r' || flags[3] == 's') {
				//
				/*
				1. mem section cound NOT be read, without 'r' flag.
				2. read from base addr of /dev/mail module would crash.
				i dont know how to handle it, just skip it.

				1f5573000-1f58f7000 rw-s 1f5573000 00:0c 6287 /dev/mali0

				*/
				continue;
			}
			strtok_r(NULL, sep, &line);  // offsets
			char *dev = strtok_r(NULL, sep, &line);  // dev number.
			int major = 0, minor = 0;
			if (!phrase_dev_num(dev, &major, &minor) || major == 0) {
				/*
				if dev major number equal to 0, mean the module must NOT be
				a shared or executable object loaded from disk.
				e.g:
				lookup symbol from [vdso] would crash.
				7f7b48a000-7f7b48c000 r-xp 00000000 00:00 0  [vdso]
				*/
				continue;
			}

			strtok_r(NULL, sep, &line);  // node

			char *filename = strtok_r(NULL, sep, &line); //module name
			if (!filename) {
				continue;
			}

			if (*filename == '\0') {
				continue;
			}
			//            LOGD("filename: %s ,target:%s",filename,target);
			//            std::string module_name(filename);

			if (strstr(filename, target) != NULL) {
				void *base_addr = NULL;
				void *end_addr = NULL;
				phrase_proc_base_addr(addr, &base_addr, &end_addr);
				void * image_base = base_addr;

				fclose(fd);
				return image_base;
			}
		}

	}
	fclose(fd);
	return (void *)-1;

}


bool HookBase::HookToSub(const char * szDLLName, uint32_t iSubOffset , void * fpReplactToFunction, void ** fpOutRealFunction) {
	bool bRet = false;
	if (szDLLName == NULL || iSubOffset == NULL || fpReplactToFunction==NULL ||fpOutRealFunction == NULL ) {
		return bRet;
	}
#ifdef _DEBUG
	char szLog[512];
#endif

#ifdef _DEBUG
	sprintf(szLog, "%s : Offset:0x%08X --> 0x%08X(Hook)", szDLLName, iSubOffset, (uint32_t)fpReplactToFunction);
	MOJING_TRACE(g_APIlogger, "Try hook function : " << szLog);
#endif

	
	void* base = getimagebase(szDLLName);
	if ((uint32_t)base == -1) {//should be uint64_t to be compatible with x64
#ifdef _DEBUG
		MOJING_TRACE(g_APIlogger, "can't find ELF  ");
#endif
		return bRet;
	}
	uint32_t iSubAddr = (uint32_t)base + iSubOffset + 1;//should be uint64_t to compatible with x64


	if (registerInlineHook(iSubAddr,
		(uint32_t)fpReplactToFunction,
		(uint32_t **)fpOutRealFunction) == DETOUR_OK)
	{
#ifdef _DEBUG
		MOJING_TRACE(g_APIlogger, "registerInlineHook OK  ");
#endif

		if (inlineHook(iSubAddr) == DETOUR_OK)
		{
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "inlineHook OK  ");
#endif
			bRet = true;
		}
		else
		{
			MOJING_ERROR(g_APIlogger, "Try inlineHook error!! Sub offset = " << iSubOffset);
		}
	}
	else
	{
		MOJING_ERROR(g_APIlogger, "Try registerInlineHook error!! Sub offset = " << iSubOffset);
	}
	return bRet;
}
