#include <dlfcn.h>
#include <math.h>
#include <Base/MojingLog.h>
#include <Hook/drawtex.h>
#include <EGL/egl.h>
#include "../Global/HookBase.h" 
#include "../../Base/MojingTimer.h"
#include "../../3rdPart/Qualcomm/CSVRApi.h"

#include "HookGVRTools.h"
#include "../../MojingAPI.h"
#include "../../MojingManager.h"
#include "../../MojingSDKStatus.h"
#include "../../Reporter/GyroTempCalibrationReporter.h"
#include "../../Profile/MojingProfileKey.h"
#include "../../Profile/UserSettingProfile.h"
#include "../../Parameters/MojingParameters.h"

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
static bool gmultiview_enabled = false;
static int gvpwidth = 0;

bool	HookGVRTools::m_bSVREnable = false;

#define TEST_WARP 1
//#define TEST_WARP_RENDER 1
//#define TEST_WARP_REPROJECTION 1
double m_dRotateSpeed = 0;

FP_gvr_get_head_space_from_start_space_rotation HookGVRTools::m_fp_gvr_get_head_space_from_start_space_rotation = NULL;
FP_gvr_reset_tracking HookGVRTools::m_fp_gvr_reset_tracking = NULL;
FP_gvr_recenter_tracking HookGVRTools::m_fp_gvr_recenter_tracking = NULL;
FP_gvr_frame_submit HookGVRTools::m_fp_gvr_frame_submit = NULL;
FP_gvr_get_viewer_model HookGVRTools::m_fp_gvr_get_viewer_model = NULL;
FP_gvr_get_viewer_vendor HookGVRTools::m_fp_gvr_get_viewer_vendor = NULL;
FP_gvr_get_version_string HookGVRTools::m_fp_gvr_get_version_string = NULL;
//FP_gvr_on_surface_created_reprojection_thread HookGVRTools::m_fp_gvr_on_surface_created_reprojection_thread = NULL;
FP_gvr_render_reprojection_thread HookGVRTools::m_fp_gvr_render_reprojection_thread = NULL;
FP_gvr_get_version HookGVRTools::m_fp_gvr_get_version = NULL;
FP_gvr_initialize_gl HookGVRTools::m_fp_gvr_initialize_gl = NULL;
FP_gvr_frame_bind_buffer HookGVRTools::m_fp_gvr_frame_bind_buffer = NULL;
FP_gvr_frame_unbind HookGVRTools::m_fp_gvr_frame_unbind = NULL;
FP_gvr_is_feature_supported HookGVRTools::m_fp_gvr_is_feature_supported = NULL;
FP_gvr_get_maximum_effective_render_target_size HookGVRTools::m_fp_gvr_get_maximum_effective_render_target_size = NULL;

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
		HookParamet HP[7];
		HOOK_PARAMET(HP[0], gvr_get_head_space_from_start_space_rotation);
		HOOK_PARAMET(HP[1], gvr_reset_tracking);
		HOOK_PARAMET(HP[2], gvr_recenter_tracking);
		HOOK_PARAMET(HP[3], gvr_frame_submit);
		HOOK_PARAMET(HP[4], gvr_initialize_gl);
		HOOK_PARAMET(HP[5], gvr_is_feature_supported);
		HOOK_PARAMET(HP[6], gvr_render_reprojection_thread);

		if (HookBase::HookToFunctions(m_hGVR, HP, 7) &&
			NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_viewer_model)) &&
			NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_viewer_vendor)) &&
			NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_version_string))&&
            NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_version)) &&
			NULL != (GET_DLL_FUNCION(m_hGVR, gvr_frame_bind_buffer))
			&& NULL != (GET_DLL_FUNCION(m_hGVR, gvr_frame_unbind))
			&& NULL != (GET_DLL_FUNCION(m_hGVR, gvr_get_maximum_effective_render_target_size)))
		{
			// Get Function pointer HOOKed
			m_fp_gvr_get_head_space_from_start_space_rotation = (FP_gvr_get_head_space_from_start_space_rotation)HP[0].fpRealFunction;
			m_fp_gvr_reset_tracking = (FP_gvr_reset_tracking)HP[1].fpRealFunction;
			m_fp_gvr_recenter_tracking = (FP_gvr_recenter_tracking)HP[2].fpRealFunction;
			m_fp_gvr_frame_submit = (FP_gvr_frame_submit)HP[3].fpRealFunction;
			m_fp_gvr_initialize_gl = (FP_gvr_initialize_gl)HP[4].fpRealFunction;
			m_fp_gvr_is_feature_supported = (FP_gvr_is_feature_supported)HP[5].fpRealFunction;
            m_fp_gvr_render_reprojection_thread = (FP_gvr_render_reprojection_thread)HP[6].fpRealFunction;
//			m_fp_gvr_frame_unbind = (FP_gvr_frame_unbind)HP[6].fpRealFunction;

			String sEngenVersion = "GVR ";
			sEngenVersion += m_fp_gvr_get_version_string();
			MOJING_TRACE(g_APIlogger, "GVR version: " << sEngenVersion.ToCStr());
			MojingSDK_SetEngineVersion(sEngenVersion);

            gvr_version version;
            version = m_fp_gvr_get_version();
            gvrmajorversion = version.major;
            gvrminorversion = version.minor;
			LOGE("gvr_version major=%d, minor=%d", gvrmajorversion, gvrminorversion);

//
//			gvr_sizei size = m_fp_gvr_get_maximum_effective_render_target_size(gvr);
//			gvpwidth = (7 * size.width) / 20;
//			LOGE("w=%d, h=%d, wid=%d", size.width, size.height, gvpwidth);

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

//gvr_version HookGVRTools::HOOK_gvr_get_version()
//{
//    LOGE("HOOK_gvr_get_version");
//	gvr_version version;
//	memset(&version, 0, sizeof(gvr_version));
////	if( m_fp_gvr_get_version)
////		version = m_fp_gvr_get_version();
//	return version;
//}

#if 0
gvr_mat4f HookGVRTools::HOOK_gvr_get_head_space_from_start_space_rotation(const gvr_context *gvr, const gvr_clock_time_point time)
{
	gvr_mat4f Ret;
	memset(&Ret, 0, sizeof(gvr_mat4f));
	Ret.m[0][0] = Ret.m[1][1] = Ret.m[2][2] = Ret.m[3][3] = 1;
	static double timeBase = Baofeng::Mojing::Timer::GetSeconds();
	static double dSpeed = 10 * PI / 180;// ÿ��1�ȵ��˶��ٶ�,�Ի���/�����

	double timeNow = Baofeng::Mojing::Timer::GetSeconds();
	
	double lTimeOffsetInMS = (timeNow - timeBase);// �˴�Ϊ����Ϊ��λ��ʱ���
	// ���´�������ģ��200Hz�� ������
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

#if TEST_WARP
	static Quatd qLastRotate;
	static int64_t i64LastTime = 0;
	Matrix4d m4RotateSpeed;
	for (int iX = 0; iX < 4; iX++)
	{
		for (int iY = 0; iY < 4; iY++)
		{
			m4RotateSpeed.M[iX][iY] = Ret.m[iY][iX];
		}
	}
	Quatd qCurrentRotate(m4RotateSpeed);
	qCurrentRotate.Normalize();

	if (0 == i64LastTime)
	{
		qLastRotate = qCurrentRotate;
		i64LastTime = time.monotonic_system_time_nanos;
		m_dRotateSpeed = 0;
	}
	else
	{
		double d64TimeDiffSec = (time.monotonic_system_time_nanos - i64LastTime)*1e-9;
		if (d64TimeDiffSec != 0)
		{
			// Á½¸öÏòÁ¿×ö¼õ·¨£¿
			Quatd Rotated = qLastRotate * qCurrentRotate.Inverted();
//			double dAngle = abs(acos(Rotated.w) * 2 * 180 / PI - 180);
			double dAngle = acos(Rotated.w) * 2 * 180 / PI;
			// dSpeedÊÇ¶È/ÃëµÄ±ê¼Ç
            qLastRotate = qCurrentRotate;
            m_dRotateSpeed = dAngle / d64TimeDiffSec;
		}
	}
//    LOGE("rotatespeed=%f", m_dRotateSpeed);

#endif

    return Ret;

	/************************************************************************/
	/* ���´������������ӿ��ṩ��ǰ����ʹ�õľ�Ƭ                         */
	/************************************************************************/
	static int CheckCount = 0;
	static int LastReportThreadID = 0;
	static String LastReport;
	if (CheckCount++ % 18000 == 0)
	{	// ÿһ֡����3�Σ�18000����6000֡��6000 / 60(fps) = 100����һ��
		// ��һ���Martix�ϣ�6000 / 90(fps) = 66.666����һ�£�һ��������
		CheckCount = 1;
		int iThdID = gettid();
		if (LastReportThreadID != iThdID)
		{
			LastReportThreadID = iThdID;
			String sVendor = m_fp_gvr_get_viewer_vendor(gvr);
			String sModel = m_fp_gvr_get_viewer_model(gvr);
			String sReportString = sVendor + "@" + sModel;


			// GVR ģʽ��һ����DefaultGlass
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
	/************************************************************************/
	/* ���ϴ������������ӿ��ṩ��ǰ����ʹ�õľ�Ƭ                         */
	/************************************************************************/

	if (!m_bSVREnable)
	{// ��ͨģʽ
		//MATRIX��δ����svr�����ǵ�APP�����޸���������Դ
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
			{// ʹ��Mojing ������
				static float fTemp[16];
				if (0 == MojingSDK_getPredictionHeadView(fTemp, 0/*time.monotonic_system_time_nanos * 1e-9*/))
				{
					// �о���ת�о���
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
			{// DD�������ѹر�
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
		// �о���ת�о���
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
			// �о���ת�о���
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

extern int rendertid;
extern int gwidth;
extern int gheight;
void HookGVRTools::HOOK_gvr_frame_submit(gvr_frame **frame, const gvr_buffer_viewport_list *list, gvr_mat4f head_space_from_start_space)
{
    LOGE("HOOK_gvr_frame_submit, tid=%d", gettid());

#ifdef  TEST_WARP_RENDER
    if( m_dRotateSpeed > 0.1f )
		glClearColor ( 1.0f, 1.0f, 1.0f, 1.0f );
	else
		glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

	m_fp_gvr_frame_bind_buffer(*frame, 0);
    glClear ( GL_COLOR_BUFFER_BIT );
	m_fp_gvr_frame_unbind(*frame);

	m_fp_gvr_frame_bind_buffer(*frame, 1);
    glClear ( GL_COLOR_BUFFER_BIT );
	m_fp_gvr_frame_unbind(*frame);
#else

	glViewport(0, 0, gvpwidth, gvpwidth );
	m_fp_gvr_frame_bind_buffer(*frame, 0);
//    glClearColor ( 1.0f, 1.0f, 0.0f, 0.0f );
//    glClear ( GL_COLOR_BUFFER_BIT );
	DrawTex(&gUserData);
	m_fp_gvr_frame_unbind(*frame);

	m_fp_gvr_frame_bind_buffer(*frame, 1);
//	glViewport(0, 0, 960, 1080);
//    glClearColor ( 1.0f, 1.0f, 0.0f, 0.0f );
//    glClear ( GL_COLOR_BUFFER_BIT );
	DrawTex(&gUserData);
	m_fp_gvr_frame_unbind(*frame);

#endif

    rendertid = gettid();
	if (m_fp_gvr_frame_submit)
	{
		m_fp_gvr_frame_submit(frame, list, head_space_from_start_space);
	}

	static int iBeginTime = Baofeng::Mojing::Timer::GetSeconds();
	static int iFreamCount = 0;
	int iTimeNow = Baofeng::Mojing::Timer::GetSeconds();
#define FPS_TIME_INTERVAL 5
	iFreamCount++;
	if (iTimeNow >= iBeginTime + FPS_TIME_INTERVAL)
	{
        LOGE("FPS=%f", iFreamCount * 1.0 / FPS_TIME_INTERVAL);
		MOJING_TRACE(g_APIlogger, "FPS = " << iFreamCount * 1.0 / FPS_TIME_INTERVAL);
		iBeginTime = iTimeNow;
		iFreamCount = 0;
		
	}

}


//int HookGVRTools::HOOK_gvr_on_surface_created_reprojection_thread(const gvr_context *gvr)
//{
//    LOGE("HOOK_gvr_on_surface_created_reprojection_thread");
//	int re = 0;
//	if(m_fp_gvr_on_surface_created_reprojection_thread)
//		re = m_fp_gvr_on_surface_created_reprojection_thread(gvr);
//	return re;
//}

//extern int needswapbuffer;
int HookGVRTools::HOOK_gvr_render_reprojection_thread(const gvr_context *gvr)
{
//    LOGE("HOOK_gvr_render_reprojection_thread begin, tid=%d", gettid());
    int re = 0;
#ifdef TEST_WARP_REPROJECTION
	LOGE("rotatespeed=%f", m_dRotateSpeed);
//	re = m_fp_gvr_render_reprojection_thread(gvr);
    if( m_dRotateSpeed > 0.1f )
        glClearColor ( 1.0f, 1.0f, 1.0f, 1.0f );
    else
        glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear ( GL_COLOR_BUFFER_BIT );
//    eglSwapBuffers(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW));
    re = m_fp_gvr_render_reprojection_thread(gvr);
#else
    if( m_fp_gvr_render_reprojection_thread) {
//        needswapbuffer = 1;
//        LOGE("rotatespeed=%f", m_dRotateSpeed);
//        if( m_dRotateSpeed > 0.1f )
//            glClearColor ( 1.0f, 1.0f, 1.0f, 1.0f );
//        else
//            glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
//        glClear ( GL_COLOR_BUFFER_BIT );
		re = m_fp_gvr_render_reprojection_thread(gvr);
	}
#endif
//	glFinish();
//    LOGE("HOOK_gvr_render_reprojection_thread end, tid=%d", gettid());
    return re;
}

void HookGVRTools::HOOK_gvr_initialize_gl(gvr_context* gvr)
{
    LOGE("HOOK_gvr_initialize_gl");
	if( m_fp_gvr_initialize_gl)
		m_fp_gvr_initialize_gl(gvr);
	InitTex(&gUserData, 0);
	gvr_sizei size = m_fp_gvr_get_maximum_effective_render_target_size(gvr);
    gvpwidth = (7 * size.width) / 20;
    LOGE("w=%d, h=%d, wid=%d", size.width, size.height, gvpwidth);
	return;
}

bool HookGVRTools::HOOK_gvr_is_feature_supported(const gvr_context* gvr, int32_t feature)
{
	LOGE("HOOK_gvr_is_feature_supported");
	bool re = false;
	if( m_fp_gvr_is_feature_supported)
		re = m_fp_gvr_is_feature_supported(gvr, feature);
	if( 1 == feature) {
		gmultiview_enabled = re;
	}
	return re;
}

void HookGVRTools::HOOK_gvr_frame_unbind(gvr_frame* frame)
{
	LOGE("Hook_gvr_frame_unbind");
//	m_fp_gvr_frame_bind_buffer(*frame, 0);
//	if(gmultiview_enabled) {
//		DrawTex(&gUserData);
//	} else{
//		glViewport(gwidth, 0, gwidth, gheight);
//		DrawTex(&gUserData);
//		glViewport(0, 0, gwidth, gheight);
//		DrawTex(&gUserData);
//	}

//	glClearColor ( 1.0f, 1.0f, 0.0f, 0.0f );
//	glClear ( GL_COLOR_BUFFER_BIT );

	if( m_fp_gvr_frame_unbind)
		m_fp_gvr_frame_unbind(frame);
    return;
}


void HookGVRTools::HOOK_gvr_reset_tracking( gvr_context *gvr)
{// ע�⣺�˺�����DD����������HOOK_gvr_recenter_tracking
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
{// ע�⣺�˺����������滻 gvr_reset_tracking
#ifdef _DEBUG
	MOJING_FUNC_TRACE(g_APIlogger);
#endif
//	bool bIsMJ5 = MojingSDK_IsHDMWorking();
//	bool bSensorDataFromMJSDK = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->GetSensorDataFromMJSDK();
//	if (bIsMJ5 || bSensorDataFromMJSDK || m_bSVREnable)
//	{
//		MojingSDK_ResetSensorOrientation();
//	}
	m_fp_gvr_recenter_tracking(gvr);
}