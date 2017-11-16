#pragma once
#include "../../Base/MojingTypes.h"
#ifdef MJ_OS_ANDROID
struct HookParamet
{
	char szFunctionName[128];
	void *fpHookToFunction;
	void *fpRealFunction;
};

class HookBase
{
public:
	HookBase();
	virtual ~HookBase();

	static bool HookToFunctions(void * hDLL, HookParamet * pHookParamet, int iFunctionCount);
	static bool HookToFunction(void * hDLL , const char * szFunctionName , void * fpReplactToFunction , void ** fpOutRealFunction);
};
#endif

