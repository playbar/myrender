#include <dlfcn.h>
#include <math.h>

#include "../Global/HookBase.h" 
#include "../../Base/MojingTimer.h"
#include "../../Base/MojingLog.h"
#include "../../3rdPart/Qualcomm/CSVRApi.h"
#include "../../MojingAPI.h"
#include "../../MojingManager.h"
#include "../../MojingSDKStatus.h"
#include "../../Reporter/GyroTempCalibrationReporter.h"
#include "../../Profile/MojingProfileKey.h"
#include "../../Profile/UserSettingProfile.h"
#include "../../Parameters/MojingParameters.h"
#include "../drawtex.h"
#include "HookGVRTools.h"

#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

//#define HOOK_DEBUG

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#define GET_DLL_FUNCION(DLL , FUNC)  m_fp_##FUNC = (FP_##FUNC)dlsym(DLL , FN_##FUNC)
bool g_bEnableDDTracker = true;
float g_fDDHeaderView[16] = {	1,0,0,0,
								0,1,0,0,
								0,0,1,0,
								0,0,0,1};

void * HookGVRTools::m_hGVR = NULL;
#if USING_SVR_SENSOR
CSVRApi HookGVRTools::m_SVRApi;
#endif

extern int gvrmajorversion;
extern int gvrminorversion;
extern bool gEnableHook;
#ifdef FRAME_SHOW_INFO
static bool gmultiview_enabled = false;
static int gvpwidth = 0;
static gvr_swap_chain* gSwapChain = NULL;
#endif
bool	HookGVRTools::m_bSVREnable = false;

FP_gvr_get_head_space_from_start_space_rotation HookGVRTools::m_fp_gvr_get_head_space_from_start_space_rotation = NULL;
FP_gvr_reset_tracking HookGVRTools::m_fp_gvr_reset_tracking = NULL;
FP_gvr_recenter_tracking HookGVRTools::m_fp_gvr_recenter_tracking = NULL;
FP_gvr_frame_submit HookGVRTools::m_fp_gvr_frame_submit = NULL;
FP_gvr_get_viewer_model HookGVRTools::m_fp_gvr_get_viewer_model = NULL;
FP_gvr_get_viewer_vendor HookGVRTools::m_fp_gvr_get_viewer_vendor = NULL;
FP_gvr_get_version_string HookGVRTools::m_fp_gvr_get_version_string = NULL;
FP_gvr_get_version HookGVRTools::m_fp_gvr_get_version = NULL;
#ifdef FRAME_SHOW_INFO
FP_gvr_initialize_gl HookGVRTools::m_fp_gvr_initialize_gl = NULL;
FP_gvr_frame_bind_buffer HookGVRTools::m_fp_gvr_frame_bind_buffer = NULL;
FP_gvr_frame_unbind HookGVRTools::m_fp_gvr_frame_unbind = NULL;
FP_gvr_get_maximum_effective_render_target_size HookGVRTools::m_fp_gvr_get_maximum_effective_render_target_size = NULL;
FP_gvr_swap_chain_get_buffer_count HookGVRTools::m_fp_gvr_swap_chain_get_buffer_count = NULL;
FP_gvr_swap_chain_acquire_frame HookGVRTools::m_fp_gvr_swap_chain_acquire_frame = NULL;
#endif

extern String ParseGlassKey(String sJson);
HookGVRTools::HookGVRTools()
{
}

HookGVRTools::~HookGVRTools()
{
	if (m_hGVR)
	{
		dlclose(m_hGVR);
	}
	m_hGVR = NULL;
}

#define HOOK_PARAMET(PP , __GVR_FUNCTION_NAME__) {strcpy(PP.szFunctionName , FN_##__GVR_FUNCTION_NAME__) ; PP.fpHookToFunction= (void*)HookGVRTools::HOOK_##__GVR_FUNCTION_NAME__;PP.fpRealFunction=NULL;}
#define HOOK_FUNCTION(__GVR_FUNCTION_NAME__) HookBase::HookToFunction(m_hGVR, FN_##__GVR_FUNCTION_NAME__, (void*)HookGVRTools::HOOK_##__GVR_FUNCTION_NAME__, (void**)&m_fp_##__GVR_FUNCTION_NAME__)
bool HookGVRTools::Init()
{
	MOJING_FUNC_TRACE(g_APIlogger);
	bool bRet = false;
	if (LoadGVR())
	{
#ifdef FRAME_SHOW_INFO
		HookParamet HP[6];
#else
		HookParamet HP[4];
#endif
		HOOK_PARAMET(HP[0], gvr_get_head_space_from_start_space_rotation);
		HOOK_PARAMET(HP[1], gvr_frame_submit);
		HOOK_PARAMET(HP[2], gvr_reset_tracking);
		HOOK_PARAMET(HP[3], gvr_recenter_tracking);
#ifdef FRAME_SHOW_INFO
		HOOK_PARAMET(HP[4], gvr_initialize_gl);
		HOOK_PARAMET(HP[5], gvr_swap_chain_acquire_frame);

		if (HookBase::HookToFunctions(m_hGVR, HP, 6) 
#else
		if (HookBase::HookToFunctions(m_hGVR, HP, 4)
#endif
			&& NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_viewer_model))
			&& NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_viewer_vendor))
			&& NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_version_string))
			&& NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_version))
#ifdef FRAME_SHOW_INFO
			&& NULL != (GET_DLL_FUNCION(m_hGVR, gvr_frame_bind_buffer))
			&& NULL != (GET_DLL_FUNCION(m_hGVR, gvr_frame_unbind))
			&& NULL != (GET_DLL_FUNCION(m_hGVR, gvr_swap_chain_get_buffer_count))
			&& NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_maximum_effective_render_target_size))
#endif
			)
		{
			// Get Function pointer HOOKed
			m_fp_gvr_get_head_space_from_start_space_rotation = (FP_gvr_get_head_space_from_start_space_rotation)HP[0].fpRealFunction;
			m_fp_gvr_frame_submit = (FP_gvr_frame_submit)HP[1].fpRealFunction;
			m_fp_gvr_reset_tracking = (FP_gvr_reset_tracking)HP[2].fpRealFunction;
			m_fp_gvr_recenter_tracking = (FP_gvr_recenter_tracking)HP[3].fpRealFunction;
#ifdef FRAME_SHOW_INFO
			m_fp_gvr_initialize_gl = (FP_gvr_initialize_gl)HP[4].fpRealFunction;
			m_fp_gvr_swap_chain_acquire_frame = (FP_gvr_swap_chain_acquire_frame)HP[5].fpRealFunction;
#endif
			String sEngenVersion = "GVR ";
			sEngenVersion += m_fp_gvr_get_version_string();
			MOJING_TRACE(g_APIlogger, "GVR version: " << sEngenVersion.ToCStr());
			MojingSDK_SetEngineVersion(sEngenVersion);

            gvr_version version;
            version = m_fp_gvr_get_version();
            gvrmajorversion = version.major;
            gvrminorversion = version.minor;
			//LOGE("gvr_version major=%d, minor=%d", gvrmajorversion, gvrminorversion);

			g_bEnableDDTracker = true;
			memset(g_fDDHeaderView, 0, sizeof(float)* 16);
			g_fDDHeaderView[0] = g_fDDHeaderView[5] = g_fDDHeaderView[10] = g_fDDHeaderView[15] = 1;

			MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();

			if (DEVICE_ABILITY_SVR & pDeviceParameters->GetAbility() && (pDeviceParameters->GetCurrentMachine().m_iID == 2))
			{
#if USING_SVR_SENSOR
				MOJING_TRACE(g_APIlogger, "DEVICE_ABILITY_SVR...");
				if (m_SVRApi.Init())
				{
					MOJING_TRACE(g_APIlogger, "DEVICE_ABILITY_SVR...1");
					if (m_SVRApi.InitializeQvrServiceOnly())
					{
						MOJING_TRACE(g_APIlogger, "DEVICE_ABILITY_SVR...2");
						m_bSVREnable = true;
						m_SVRApi.SetCpuPerfLevel(svrPerfLevel::kPerfMaximum);
						m_SVRApi.SetPerformanceLevels(svrPerfLevel::kPerfMaximum, svrPerfLevel::kPerfMaximum);
						
					}
				}
#else
				m_bSVREnable = true;
#endif
			}
			
			MOJING_TRACE(g_APIlogger, "Using DEVICE_ABILITY_SVR..." << (m_bSVREnable ? "SUCCEEDED" : "FAILD") );
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
	if (m_hGVR == NULL)
	{
		m_hGVR = dlopen("libgvr.so", RTLD_LAZY);
	}

	if (m_hGVR == NULL)
	{
		MOJING_ERROR(g_APIlogger, "dlopen libgvr.so err: " << dlerror());
	}
	return m_hGVR != NULL;
}

/************************************************************************/
/* 以下代码用于向报数接口提供当前正在使用的镜片									*/
/************************************************************************/
void HookGVRTools::ReportCurrentGlasses(const gvr_context *gvr)
{
	static int CheckCount = 0;
	static int LastReportThreadID = 0;
	static String LastReport;
	if (CheckCount++ % 18000 == 0)
	{	// 每一帧调用3次，18000次是6000帧。6000 / 60(fps) = 100秒检查一下
		// 在一体机Martix上，6000 / 90(fps) = 66.666秒检查一下，一分钟左右
		CheckCount = 1;
		int iThdID = gettid();
		if (LastReportThreadID != iThdID)
		{
			LastReportThreadID = iThdID;
			String sVendor = m_fp_gvr_get_viewer_vendor(gvr);
			String sModel = m_fp_gvr_get_viewer_model(gvr);
			String sReportString = sVendor + "@" + sModel;

			// GVR 模式下一定是DefaultGlass
			String sGlass = MojingSDK_GetMojingWorldKey(MOJING_WORLDKEY_DEFAULT);
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
}

#if 0
gvr_mat4f HookGVRTools::HOOK_gvr_get_head_space_from_start_space_rotation(const gvr_context *gvr, const gvr_clock_time_point time)
{
	gvr_mat4f Ret;
	memset(&Ret, 0, sizeof(gvr_mat4f));
	Ret.m[0][0] = Ret.m[1][1] = Ret.m[2][2] = Ret.m[3][3] = 1;
	static double timeBase = Baofeng::Mojing::Timer::GetSeconds();
	static double dSpeed = 10 * PI / 180;// 每秒1度的运动速度,以弧度/秒表述

	double timeNow = Baofeng::Mojing::Timer::GetSeconds();
	
	double lTimeOffsetInMS = (timeNow - timeBase);// 此处为以秒为单位的时间差
	// 以下代码用于模拟200Hz的 陀螺仪
	{
		long long lTT = lTimeOffsetInMS * 200.0f;
		lTimeOffsetInMS = lTT;
		lTimeOffsetInMS /= 200;
	}
	double fAngle = lTimeOffsetInMS * dSpeed;
	while (fAngle >= PI*2)
	{
		fAngle -= PI * 2;
	}

	float sina = sin(fAngle);
	float cosa = cos(fAngle);

	float fRet[16] = {
		cosa, 0, sina,0,
		0, 1, 0,0,
		-sina, 0, cosa,0,
		0,0,0,1	
	};
	memcpy(&Ret.m[0][0], fRet, 16 * sizeof(float));
	return Ret;
}

#else
gvr_mat4f HookGVRTools::HOOK_gvr_get_head_space_from_start_space_rotation(const gvr_context *gvr, const gvr_clock_time_point time)
{
#ifdef HOOK_DEBUG
	MOJING_FUNC_TRACE(g_APIlogger);
#endif
	gvr_mat4f Ret;
	memset(&Ret, 0, sizeof(gvr_mat4f));
	Ret.m[0][0] = Ret.m[1][1] = Ret.m[2][2] = Ret.m[3][3] = 1;
	if (m_fp_gvr_get_head_space_from_start_space_rotation)
	{
		Ret = m_fp_gvr_get_head_space_from_start_space_rotation(gvr, time);
	}
	
	//return Ret;
	
	//向报数接口提供当前正在使用的镜片
	ReportCurrentGlasses(gvr);

	if (!m_bSVREnable)
	{// 普通模式
		//MATRIX上未启用svr陀螺仪的APP，不修改陀螺仪来源
		MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
		if (pDeviceParameters && pDeviceParameters->GetIsMachine())
		{
			//MOJING_TRACE(g_APIlogger, "HOOK_gvr_get_head_space_from_start_space_rotation is InMachine.");
			return Ret;
		}

		MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
		bool bEnableMojingTracker = pStatus->GetTrackerStatus() == TRACKER_START;
		if (bEnableMojingTracker) //
		{
			bool bIsMJ5 = MojingSDK_IsHDMWorking();
			bool bSensorDataFromMJSDK = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->GetSensorDataFromMJSDK();
	#ifdef HOOK_DEBUG
			MOJING_TRACE(g_APIlogger, "HOOK_gvr_get_head_space_from_start_space_rotation IsMJ5: " << bIsMJ5 << " SensorDataFromMJSDK: " << bSensorDataFromMJSDK);
	#endif
			if (bIsMJ5 || bSensorDataFromMJSDK)
			{// 使用Mojing 陀螺仪
				static float fTemp[16];
				if (0 == MojingSDK_getPredictionHeadView(fTemp, 0/*time.monotonic_system_time_nanos * 1e-9*/))
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
			// else !bIsMJ5 && !bSensorDataFromMJSDK

			if (!g_bEnableDDTracker)
			{// DD陀螺仪已关闭
				memcpy(g_fDDHeaderView, Ret.m[0], 16 * sizeof(float));
				memset(&Ret, 0, sizeof(gvr_mat4f));
				Ret.m[0][0] = Ret.m[1][1] = Ret.m[2][2] = Ret.m[3][3] = 1;
			}
		}
		else// else !bEnableMojingTracker
		{
			memcpy(g_fDDHeaderView, Ret.m[0], 16 * sizeof(float));
			memset(&Ret, 0, sizeof(gvr_mat4f));
			Ret.m[0][0] = Ret.m[1][1] = Ret.m[2][2] = Ret.m[3][3] = 1;
		}
	}
	else//if (!m_bSVREnable)
	{
#if USING_SVR_SENSOR
		double dTimeNow = Baofeng::Mojing::Timer::GetSeconds() * 1e9;
		
		MOJING_TRACE(g_APIlogger , "Using SVR rotation...");
		double dTimeMS = time.monotonic_system_time_nanos - dTimeNow;
		dTimeMS *= 1e-6;
		svrHeadPoseState  HPS = m_SVRApi.GetPredictedHeadPose(fmax(dTimeMS, 0));
		Quatf Q(HPS.pose.rotation.x, HPS.pose.rotation.y, HPS.pose.rotation.z, HPS.pose.rotation.w);
		Matrix4f M4(Q);
		// 列矩阵转行矩阵
		for (int iX = 0; iX < 4; iX++)
		{
			for (int iY = 0; iY < 4; iY++)
			{
				Ret.m[iX][iY] = M4.M[iX][iY];
			}
		}
#else
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
#endif
	}
	return Ret;
}
#endif

SDK_VERIFY m_status = VERIFY_OK;
GLuint m_InfoTextureID = 0;
extern GLuint CreateInfoTexture();
extern int rendertid;
void HookGVRTools::HOOK_gvr_frame_submit(gvr_frame **frame, const gvr_buffer_viewport_list *list, gvr_mat4f head_space_from_start_space)
{
#ifdef FRAME_SHOW_INFO
	rendertid = gettid();
	SDK_VERIFY  status = MojingSDKStatus::GetSDKStatus()->GetVerfiyStatus();
	if (status != VERIFY_OK)
	{
		if (m_InfoTextureID == 0 || status != m_status)
		{
			m_status = status;
			m_InfoTextureID = CreateInfoTexture();
		}
#ifdef HOOK_DEBUG
		MOJING_TRACE(g_APIlogger, "HOOK_gvr_frame_submit Verify status: " << status << " TextureID : " << m_InfoTextureID);
#endif
		if (m_InfoTextureID != 0)
		{
			gUserData.textureId = m_InfoTextureID;
		}
	if( gEnableHook) {
		int32_t count = m_fp_gvr_swap_chain_get_buffer_count(gSwapChain);
		LOGE("tid=%d, framecount=%d", rendertid, count);
		glViewport(0, 0, gvpwidth, gvpwidth);
		for (int i = 0; i < count; ++i) {
			m_fp_gvr_frame_bind_buffer(*frame, i);
			DrawTex(&gUserData);
			m_fp_gvr_frame_unbind(*frame);
		}
	}

		LOGE("glbindbuffer end, vboID=%d, iboID=%d", gUserData.vboID, gUserData.textureId);
	}
#endif

	if (m_fp_gvr_frame_submit)
	{
		m_fp_gvr_frame_submit(frame, list, head_space_from_start_space);
	}

	static int iBeginTime = Baofeng::Mojing::Timer::GetSeconds();
	static int iFreamCount = 0;
	int iTimeNow = Baofeng::Mojing::Timer::GetSeconds();
#define FPS_TIME_INTERVAL 30
	iFreamCount++;
	if (iTimeNow >= iBeginTime + FPS_TIME_INTERVAL)
	{
		MOJING_TRACE(g_APIlogger, "FPS = " << iFreamCount * 1.0 / FPS_TIME_INTERVAL);
		iBeginTime = iTimeNow;
		iFreamCount = 0;	
	}
}

#ifdef FRAME_SHOW_INFO
gvr_frame *HookGVRTools::HOOK_gvr_swap_chain_acquire_frame(gvr_swap_chain* swap_chain)
{
	gSwapChain = swap_chain;
	return m_fp_gvr_swap_chain_acquire_frame(swap_chain);
}

void HookGVRTools::HOOK_gvr_initialize_gl(gvr_context* gvr)
{
    LOGE("HOOK_gvr_initialize_gl");
	if( m_fp_gvr_initialize_gl)
		m_fp_gvr_initialize_gl(gvr);
	if( gEnableHook) {
		InitTex(&gUserData, 1);
	}
	gvr_sizei size = m_fp_gvr_get_maximum_effective_render_target_size(gvr);
    gvpwidth = (7 * size.width) / 20;
    LOGE("w=%d, h=%d, wid=%d", size.width, size.height, gvpwidth);
	return;
}
#endif

void HookGVRTools::HOOK_gvr_reset_tracking( gvr_context *gvr)
{// 注锟解：锟剿猴拷锟斤拷锟斤拷DD锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷HOOK_gvr_recenter_tracking
#ifdef _DEBUG
	MOJING_FUNC_TRACE(g_APIlogger);
#endif
	bool bIsMJ5 = MojingSDK_IsHDMWorking();
	bool bSensorDataFromMJSDK = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->GetSensorDataFromMJSDK();
	if (bIsMJ5 || bSensorDataFromMJSDK || m_bSVREnable)
	{
		MojingSDK_ResetSensorOrientation();
	}
	m_fp_gvr_reset_tracking(gvr);
}

void HookGVRTools::HOOK_gvr_recenter_tracking(gvr_context *gvr)
{// 注意：此函数被用来替换 gvr_reset_tracking
#ifdef _DEBUG
	MOJING_FUNC_TRACE(g_APIlogger);
#endif
	bool bIsMJ5 = MojingSDK_IsHDMWorking();
	bool bSensorDataFromMJSDK = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->GetSensorDataFromMJSDK();
	if (bIsMJ5 || bSensorDataFromMJSDK || m_bSVREnable)
	{
		MojingSDK_ResetSensorOrientation();
	}
	m_fp_gvr_recenter_tracking(gvr);
}