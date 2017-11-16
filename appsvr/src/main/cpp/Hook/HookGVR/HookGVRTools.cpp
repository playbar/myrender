#include <dlfcn.h>
#include "../Global/HookBase.h" 
#include "HookGVRTools.h"
#include "../../MojingAPI.h"
#include "../../Reporter/GyroTempCalibrationReporter.h"
#include "../../Profile/MojingProfileKey.h"

#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#define GET_DLL_FUNCION(DLL , FUNC)  m_fp_##FUNC = (FP_##FUNC)dlsym(DLL , FN_##FUNC)

void * HookGVRTools::m_hGVR = NULL;
FP_gvr_get_head_space_from_start_space_rotation HookGVRTools::m_fp_gvr_get_head_space_from_start_space_rotation = NULL;
FP_gvr_reset_tracking HookGVRTools::m_fp_gvr_reset_tracking = NULL;
FP_gvr_get_viewer_model HookGVRTools::m_fp_gvr_get_viewer_model = NULL;
FP_gvr_get_viewer_vendor HookGVRTools::m_fp_gvr_get_viewer_vendor = NULL;

extern String ParseGlassKey(String sJson);
HookGVRTools::HookGVRTools()
{
}


HookGVRTools::~HookGVRTools()
{
}

#define HOOK_PARAMET(PP , __GVR_FUNCTION_NAME__) {strcpy(PP.szFunctionName , FN_##__GVR_FUNCTION_NAME__) ; PP.fpHookToFunction= (void*)HookGVRTools::HOOK_##__GVR_FUNCTION_NAME__;PP.fpRealFunction=NULL;}
#define HOOK_FUNCTION(__GVR_FUNCTION_NAME__) HookBase::HookToFunction(m_hGVR, FN_##__GVR_FUNCTION_NAME__, (void*)HookGVRTools::HOOK_##__GVR_FUNCTION_NAME__, (void**)&m_fp_##__GVR_FUNCTION_NAME__)
bool HookGVRTools::Init()
{
	MOJING_FUNC_TRACE(g_APIlogger);
	bool bRet = false;
	if (LoadGVR())
	{
		HookParamet HP[2];
		HOOK_PARAMET(HP[0], gvr_get_head_space_from_start_space_rotation);
		HOOK_PARAMET(HP[1], gvr_reset_tracking);
		if (HookBase::HookToFunctions(m_hGVR, HP, 2) &&
			NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_viewer_model)) &&
			NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_viewer_vendor)))
		{
			m_fp_gvr_get_head_space_from_start_space_rotation = (FP_gvr_get_head_space_from_start_space_rotation)HP[0].fpRealFunction;
			m_fp_gvr_reset_tracking = (FP_gvr_reset_tracking)HP[1].fpRealFunction;
		}
			/*if (HOOK_FUNCTION(gvr_get_head_space_from_start_space_rotation) &&
                HOOK_FUNCTION(gvr_reset_tracking) &&
                NULL != (GET_DLL_FUNCION(m_hGVR , gvr_get_viewer_model)) &&
                NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_viewer_vendor)))
            {
                bRet = true;
            }*/
		else
		{
			MOJING_ERROR(g_APIlogger, "Can not get function pointer....");
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
	MOJING_FUNC_TRACE(g_APIlogger);
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
#ifdef _DEBUG
	MOJING_FUNC_TRACE(g_APIlogger);
#endif
	gvr_mat4f Ret;
	memset(&Ret, 0, sizeof(gvr_mat4f));
	Ret.m[0][0] = Ret.m[1][1] = Ret.m[2][2] = Ret.m[3][3] = 1;
	if (m_fp_gvr_get_head_space_from_start_space_rotation)
	{
		Ret = m_fp_gvr_get_head_space_from_start_space_rotation(gvr, time);
	}
	/************************************************************************/
	/* 以下代码用于向报数接口提供当前正在使用的镜片                         */
	/************************************************************************/
	static int CheckCount = 0;
	static int LastReportThreadID = 0;
	static String LastReport;
	if (CheckCount++ % 6000 == 0)
	{// 大概每分钟检查一下
		CheckCount = 1;
		int iThdID = gettid();
		if (LastReportThreadID != iThdID)
		{
			LastReportThreadID = iThdID;
			String sVendor = m_fp_gvr_get_viewer_vendor(gvr);
			String sModel = m_fp_gvr_get_viewer_model(gvr);
			String sReportString = sVendor + "@" + sModel;


			// GVR 模式下一定是DefaultGlass
			String sDefault = MojingSDK_GetDefaultMojingWorld("ZH");
			String sGlass = ParseGlassKey(sDefault);
			if (sGlass != "")
			{
				sGlass = MojingProfileKey::GetGlassKeyIDString(sGlass);

				if (sGlass != "UNKNOWN")
				{
					sReportString = sGlass;
				}
				else
				{
#ifdef _DEBUG
					MOJING_TRACE(g_APIlogger, "MojingSDK_GetDefaultMojingWorld is INVALID!");
#endif
				}
			}
			else
			{
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "MojingSDK_GetDefaultMojingWorld is EMPTY!");
#endif
			}
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "Current GVR glass = " << sReportString);
#endif
			GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetGlassesReportName(sReportString);
			if (LastReport != sReportString)
				LastReport = sReportString;
		}
	}
	/************************************************************************/
	/* 以上代码用于向报数接口提供当前正在使用的镜片                         */
	/************************************************************************/

	bool bIsMJ5 = MojingSDK_IsHDMWorking();
	if (bIsMJ5)
	{
		static float fTemp[16];
		if (0 == MojingSDK_getPredictionHeadView(fTemp, time.monotonic_system_time_nanos * 1e-9))
		{
			// 列矩阵转行矩阵
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
	return Ret;
}

void HookGVRTools::HOOK_gvr_reset_tracking(const gvr_context *gvr)
{
#ifdef _DEBUG
	MOJING_FUNC_TRACE(g_APIlogger);
#endif
	bool bIsMJ5 = MojingSDK_IsHDMWorking();
	if (bIsMJ5)
	{
		MojingSDK_ResetSensorOrientation();
	}
	m_fp_gvr_reset_tracking(gvr);
}