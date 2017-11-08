#include <dlfcn.h>
#include <string.h>
#include "MojingLoadSo.h"

/*
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif
*/

/* For debug builds, always enable the debug traces in this library */
#ifndef NDEBUG
#include <android/log.h>
#define LOGV(...)  ((void)__android_log_print(ANDROID_LOG_VERBOSE, "MojingLoadSo", __VA_ARGS__))
#else
#define LOGV(...)  ((void)0)
#endif

MojingLoadSo::MojingLoadSo(char* sPath)
: m_Handle(0)
{
	strncpy(m_Path, sPath, 255);
	m_Handle = dlopen(sPath, RTLD_NOW);
	if (!m_Handle) {
		LOGV("Load %s error: %s", sPath, dlerror());
		//MOJING_ERROR(g_APIlogger, "Load dylib failed: " << sPath);
		return;
	}
	//MOJING_TRACE(g_APIlogger, "Load dylib succeed: " << sPath);
	LOGV("Load %s succeed.", sPath);
}

bool MojingLoadSo::IsLoaded()
{
	return m_Handle ? true : false;
}

void * MojingLoadSo::FindFunction(char * sFuncName)
{
	const char * error = 0;
	void * pFunc = 0;

	if (m_Handle != 0)
	{
		pFunc = dlsym(m_Handle, sFuncName);
		if (pFunc == 0)
		{
			error = dlerror();
			LOGV("Get function %s address in %s error: %s", sFuncName, m_Path, error);
			//MOJING_ERROR(g_APIlogger, "Get function failed. func nam: " << sFuncName << " libpath: " << m_Path << " errmsg: " << error);
			return 0;
		}
		//MOJING_TRACE(g_APIlogger, "Get function succeed. func name: " << sFuncName << " libpath: " << m_Path);
		LOGV("Get function %s address in %s succeed.", sFuncName, m_Path);
	}
	else
	{
		LOGV("Get function %s address in %s error: m_Handle in NULL.", sFuncName, m_Path);
	}

	return pFunc;
}

MojingLoadSo::~MojingLoadSo()
{
	if (m_Handle != 0)
	{
		dlclose(m_Handle);
		m_Handle = 0;
	}
}
