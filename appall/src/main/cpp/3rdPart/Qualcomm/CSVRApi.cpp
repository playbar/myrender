#include "CSVRApi.h"
#include <dlfcn.h>
#include "../../Base/MojingTypes.h"
#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#define FN_GetVersion 						"svrGetVersion"			
#define FN_Initialize						"svrInitialize"
#define FN_Shutdown							"svrShutdown"
#define FN_GetDeviceInfo					"svrGetDeviceInfo"
#define FN_SetPerformanceLevels				"svrSetPerformanceLevels"
#define FN_BeginVr							"svrBeginVr"
#define FN_EndVr							"svrEndVr"
#define FN_GetPredictedDisplayTime			"svrGetPredictedDisplayTime"
#define FN_GetPredictedHeadPose				"svrGetPredictedHeadPose"
#define FN_RecenterPose						"svrRecenterPose"
#define FN_GetSupportedTrackingModes		"svrGetSupportedTrackingModes"
#define FN_SetTrackingMode					"svrSetTrackingMode"
#define FN_SubmitFrame						"svrSubmitFrame"
#define FN_UpdateWarpmesh					"svrUpdateWarpmesh"
#define FN_CheckServiceIsAvaliable			"svrCheckServiceIsAvaliable"
#define FN_SetTimewarp						"svrSetTimewarp"
#define FN_InitializeQvrServiceOnly			"svrInitializeQvrServiceOnly"
#define FN_ReleaseQvrServiceOnly			"svrReleaseQvrServiceOnly"

#define FN_SetCpuPerfLevel			"_Z18svrSetCpuPerfLevel12svrPerfLevel"


#define GET_DLL_FUNCION(DLL , FUNC)  m_fp##FUNC = (FP_##FUNC)dlsym(DLL , FN_##FUNC)
#define GET_DLL_FUNCION_ERR(FUNC) {if (m_fp##FUNC == NULL ) { MOJING_ERROR(g_APIlogger , "Can not get "#FUNC" function pointer");}}

CSVRApi::CSVRApi():
m_fpGetVersion(NULL),
m_fpInitialize(NULL),
m_fpShutdown(NULL),
m_fpGetDeviceInfo(NULL),
m_fpSetPerformanceLevels(NULL),
m_fpBeginVr(NULL),
m_fpEndVr(NULL),
m_fpGetPredictedDisplayTime(NULL),
m_fpGetPredictedHeadPose(NULL),
m_fpRecenterPose(NULL),
m_fpGetSupportedTrackingModes(NULL),
m_fpSetTrackingMode(NULL),
m_fpSubmitFrame(NULL),
m_fpUpdateWarpmesh(NULL),
m_fpCheckServiceIsAvaliable(NULL),
m_fpSetTimewarp(NULL),
m_hDLL(NULL),
m_bInit(false)
{
}


CSVRApi::~CSVRApi()
{
}
bool CSVRApi::Init()
{
	MOJING_FUNC_TRACE(g_APIlogger);
	bool bRet = false;
	try
	{
		m_hDLL = dlopen("libsvrapi.so", RTLD_LAZY);
		if (m_hDLL)
		{
			GET_DLL_FUNCION(m_hDLL, GetVersion);
			GET_DLL_FUNCION(m_hDLL, Initialize);
			GET_DLL_FUNCION(m_hDLL, Shutdown);
			GET_DLL_FUNCION(m_hDLL, GetDeviceInfo);
			GET_DLL_FUNCION(m_hDLL, SetPerformanceLevels);
			GET_DLL_FUNCION(m_hDLL, BeginVr);
			GET_DLL_FUNCION(m_hDLL, EndVr);
			GET_DLL_FUNCION(m_hDLL, GetPredictedDisplayTime);
			GET_DLL_FUNCION(m_hDLL, GetPredictedHeadPose);
			GET_DLL_FUNCION(m_hDLL, RecenterPose);
			GET_DLL_FUNCION(m_hDLL, GetSupportedTrackingModes);
			GET_DLL_FUNCION(m_hDLL, SetTrackingMode);
			GET_DLL_FUNCION(m_hDLL, SubmitFrame);
			GET_DLL_FUNCION(m_hDLL, UpdateWarpmesh);
			GET_DLL_FUNCION(m_hDLL, CheckServiceIsAvaliable);
			GET_DLL_FUNCION(m_hDLL, SetTimewarp);
			GET_DLL_FUNCION(m_hDLL, InitializeQvrServiceOnly);
			GET_DLL_FUNCION(m_hDLL, ReleaseQvrServiceOnly);
			GET_DLL_FUNCION(m_hDLL, SetCpuPerfLevel);

			if (m_fpGetVersion != NULL
				&& m_fpInitialize != NULL
				&& m_fpShutdown != NULL
				&& m_fpGetDeviceInfo != NULL
				&& m_fpSetPerformanceLevels != NULL
				&& m_fpBeginVr != NULL
				&& m_fpEndVr != NULL
				&& m_fpGetPredictedDisplayTime != NULL
				&& m_fpGetPredictedHeadPose != NULL
				&& m_fpRecenterPose != NULL
				&& m_fpGetSupportedTrackingModes != NULL
				&& m_fpSetTrackingMode != NULL
				&& m_fpSubmitFrame != NULL
				&& m_fpUpdateWarpmesh != NULL
				&& m_fpCheckServiceIsAvaliable != NULL
				&& m_fpSetTimewarp != NULL
				&& m_fpInitializeQvrServiceOnly != NULL
				&& m_fpReleaseQvrServiceOnly != NULL
				&& m_fpSetCpuPerfLevel != NULL
				)
			{
				bRet = m_bInit = true;
				MOJING_TRACE(g_APIlogger , "svrApi init OK");
			}
			else
			{// 有函数指针拿不到
				GET_DLL_FUNCION_ERR(GetVersion);
				GET_DLL_FUNCION_ERR(Initialize);
				GET_DLL_FUNCION_ERR(Shutdown);
				GET_DLL_FUNCION_ERR(GetDeviceInfo);
				GET_DLL_FUNCION_ERR(SetPerformanceLevels);
				GET_DLL_FUNCION_ERR(BeginVr);
				GET_DLL_FUNCION_ERR(EndVr);
				GET_DLL_FUNCION_ERR(GetPredictedDisplayTime);
				GET_DLL_FUNCION_ERR(GetPredictedHeadPose);
				GET_DLL_FUNCION_ERR(RecenterPose);
				GET_DLL_FUNCION_ERR(GetSupportedTrackingModes);
				GET_DLL_FUNCION_ERR(SetTrackingMode);
				GET_DLL_FUNCION_ERR(SubmitFrame);
				GET_DLL_FUNCION_ERR(UpdateWarpmesh);
				GET_DLL_FUNCION_ERR(CheckServiceIsAvaliable);
				GET_DLL_FUNCION_ERR(SetTimewarp);
				GET_DLL_FUNCION_ERR(InitializeQvrServiceOnly);
				GET_DLL_FUNCION_ERR(ReleaseQvrServiceOnly);
				GET_DLL_FUNCION_ERR(SetCpuPerfLevel);
				Release();
			}
		}
		else// m_hDLL == NULL
		{
			const char* err = dlerror();
			MOJING_ERROR(g_APIlogger, "Can not load libary \"libsvrapi.so\"  Error = " << (err ? err : "Unknown"));
		}
	}
	catch (...)
	{
		MOJING_ERROR(g_APIlogger, "Can not load libary \"libsvrapi.so\"  , ****CRASH***");
	}
	return	 bRet;
}
void CSVRApi::Release()
{
	if (m_hDLL)
		dlclose(m_hDLL);
	m_fpGetVersion = NULL;
	m_fpInitialize = NULL;
	m_fpShutdown = NULL;
	m_fpGetDeviceInfo = NULL;
	m_fpSetPerformanceLevels = NULL;
	m_fpBeginVr = NULL;
	m_fpEndVr = NULL;
	m_fpGetPredictedDisplayTime = NULL;
	m_fpGetPredictedHeadPose = NULL;
	m_fpRecenterPose = NULL;
	m_fpGetSupportedTrackingModes = NULL;
	m_fpSetTrackingMode = NULL;
	m_fpSubmitFrame = NULL;
	m_fpUpdateWarpmesh = NULL;
}


const char* CSVRApi::GetVersion()
{
	if (m_fpGetVersion)
		return m_fpGetVersion();
	return "";
}

bool CSVRApi::Initialize(const svrInitParams* pInitParams)
{
	if (m_fpInitialize)
		return m_fpInitialize(pInitParams);
	
	return false;
}
bool CSVRApi::InitializeQvrServiceOnly()
{
	if (m_fpInitializeQvrServiceOnly)
		return m_fpInitializeQvrServiceOnly();
	return false;
}
void CSVRApi::Shutdown()
{
	if (m_fpShutdown)
		m_fpShutdown();
}

svrDeviceInfo CSVRApi::GetDeviceInfo()
{
	svrDeviceInfo Ret;
	memset(&Ret, 0, sizeof(svrDeviceInfo));
	if (m_fpGetDeviceInfo)
		Ret = m_fpGetDeviceInfo();
	return Ret;
}

void CSVRApi::SetPerformanceLevels(svrPerfLevel cpuPerfLevel, svrPerfLevel gpuPerfLevel)
{
	if (m_fpSetPerformanceLevels)
		m_fpSetPerformanceLevels(cpuPerfLevel , gpuPerfLevel);
}
void CSVRApi::SetCpuPerfLevel(svrPerfLevel cpuPerfLevel)
{
	if (m_fpSetCpuPerfLevel)
		m_fpSetCpuPerfLevel(cpuPerfLevel);

}
void CSVRApi::BeginVr(const svrBeginParams* pBeginParams)
{
	if (m_fpBeginVr)
		m_fpBeginVr(pBeginParams);
}

void CSVRApi::EndVr()
{
	if (m_fpEndVr)
		m_fpEndVr();
}

float CSVRApi::GetPredictedDisplayTime()
{
	if (m_fpGetPredictedDisplayTime)
		return m_fpGetPredictedDisplayTime();
	return 0;
}
svrHeadPoseState CSVRApi::GetPredictedHeadPose(float predictedTimeMs)
{
	svrHeadPoseState Ret;
	memset(&Ret, 0, sizeof(svrHeadPoseState));
	if (m_fpGetPredictedHeadPose)
		Ret = m_fpGetPredictedHeadPose(predictedTimeMs);
	return Ret;		 
}

void CSVRApi::RecenterPose()
{
	if (m_fpRecenterPose)
		m_fpRecenterPose();
}

unsigned int CSVRApi::GetSupportedTrackingModes()
{
	if (m_fpGetSupportedTrackingModes)
		return m_fpGetSupportedTrackingModes();
	return 0;
}
void CSVRApi::SetTrackingMode(unsigned int trackingModes)
{
	if (m_fpSetTrackingMode)
		m_fpSetTrackingMode(trackingModes);
}
void CSVRApi::SubmitFrame(const svrFrameParams* pFrameParams)
{
	if (m_fpSubmitFrame)
		m_fpSubmitFrame(pFrameParams);
}
void CSVRApi::UpdateWarpmesh(const float* pVertexLeft,
	const float* pVertexRight,
	const size_t numVertex,
	const unsigned int* pIndex,
	const size_t numIndex)
{
	if (m_fpUpdateWarpmesh)
		m_fpUpdateWarpmesh(pVertexLeft,
			pVertexRight,
			numVertex,
			pIndex,
			numIndex);
}
bool CSVRApi::CheckServiceIsAvaliable()
{
	if (m_fpCheckServiceIsAvaliable)
	{
		return m_fpCheckServiceIsAvaliable();
	}
	else
	{
		MOJING_ERROR(g_APIlogger, "function CheckServiceIsAvaliable is not found in libsvrapi.so");
#ifdef _DEBUG
		return true;
#else
		return false;
#endif
	}
		
}

void CSVRApi::SetTimewarp(bool enable)
{
	if (m_fpSetTimewarp)
	{
		m_fpSetTimewarp(enable);
	}
	else
	{
		MOJING_ERROR(g_APIlogger, "function SetTimewarp(bool) is not found in libsvrapi.so");
	}
}