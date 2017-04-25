#include <dlfcn.h>
#include "../Global/HookBase.h" 
#include "HookGVRTools.h"

#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

#ifdef _DEBUG
#define  HOOK_DEBUG
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

void * HookGVRTools::m_hGVR = NULL;
FP_gvr_get_head_space_from_start_space_rotation HookGVRTools::m_fp_gvr_get_head_space_from_start_space_rotation = NULL;

extern bool MojingSDK_IsHDMWorking();
extern int MojingSDK_getPredictionHeadView(float* pfViewMatrix, double time);
HookGVRTools::HookGVRTools()
{
}


HookGVRTools::~HookGVRTools()
{
}

bool HookGVRTools::Init()
{
	MOJING_FUNC_TRACE(g_APIlogger);
	bool bRet = false;
	if (LoadGVR())
	{
		if (HookBase::HookToFunction(m_hGVR , 
			FN_gvr_get_head_space_from_start_space_rotation , 
			(void*)HookGVRTools::HOOK_gvr_get_head_space_from_start_space_rotation,
			(void**)&m_fp_gvr_get_head_space_from_start_space_rotation
			))
		{
			bRet = true;
		}
		else
		{
			MOJING_ERROR(g_APIlogger, "hook error");
		}
	}
	else
	{
		MOJING_ERROR(g_APIlogger, "LoadGVR error");
	}

	return bRet;
}

bool HookGVRTools::LoadGVR()
{
	if (m_hGVR == nullptr)
	{
		m_hGVR = dlopen("libgvr.so", RTLD_LAZY);
	}

	if (m_hGVR == NULL)
	{
		MOJING_ERROR(g_APIlogger, "dlopen libgvr.so err: " << dlerror());
	}
	return m_hGVR != NULL;
}

gvr_mat4f HookGVRTools::HOOK_gvr_get_head_space_from_start_space_rotation(const gvr_context *gvr, const gvr_clock_time_point time)
{
	gvr_mat4f Ret;
	memset(&Ret, 0, sizeof(gvr_mat4f));
	Ret.m[0][0] = Ret.m[1][1] = Ret.m[2][2] = Ret.m[3][3] = 1;
	bool bGetSensorFromMJSDK = false;
	bool bIsMJ5 = MojingSDK_IsHDMWorking();
	if (bIsMJ5)
	{
#ifdef HOOK_DEBUG
		MOJING_TRACE(g_APIlogger , "Call MojingSDK_getPredictionHeadView");
#endif
		static float fTemp[16];
		bGetSensorFromMJSDK = (0 == MojingSDK_getPredictionHeadView(fTemp, time.monotonic_system_time_nanos * 1e-9));
		if (bGetSensorFromMJSDK)
		{
			// ÁÐ¾ØÕó×ªÐÐ¾ØÕó
			for (int iX = 0; iX < 4; iX++)
			{
				for (int iY = 0; iY < 4; iY++)
				{
					Ret.m[iY][iX] = fTemp[iX * 4 + iY];
				}
			}
		}
		else
		{
			MOJING_ERROR(g_APIlogger, "Call MojingSDK_getPredictionHeadView FAILD");
		}
	}

	if (!bGetSensorFromMJSDK)
	{
		if (m_fp_gvr_get_head_space_from_start_space_rotation)
		{
#ifdef HOOK_DEBUG
			MOJING_TRACE(g_APIlogger, "Call gvr_get_head_space_from_start_space_rotation"); 
#endif
			Ret = m_fp_gvr_get_head_space_from_start_space_rotation(gvr, time);
		}
		else
		{
#ifdef HOOK_DEBUG
			MOJING_ERROR(g_APIlogger, "gvr_get_head_space_from_start_space_rotation = NULL");
#endif
		}
	}
	return Ret;
}