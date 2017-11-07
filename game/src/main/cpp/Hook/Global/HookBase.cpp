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
//				usleep(10*1000);
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