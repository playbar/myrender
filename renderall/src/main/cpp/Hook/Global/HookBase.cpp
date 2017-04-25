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

bool HookBase::HookToFunction(void * hDLL, const char * szFunctionName, void * fpReplactToFunction, void ** fpOutRealFunction)
{
	bool bRet = false;
	if (hDLL != NULL && szFunctionName != NULL && fpOutRealFunction != NULL)
	{
		void *pFunc = dlsym(hDLL, szFunctionName);
		if (pFunc != NULL)
		{
			if (registerInlineHook((uint32_t)pFunc,
				(uint32_t)fpReplactToFunction,
				(uint32_t **)fpOutRealFunction) == DETOUR_OK)
			{
				if (inlineHook((uint32_t)pFunc) == DETOUR_OK)
				{
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