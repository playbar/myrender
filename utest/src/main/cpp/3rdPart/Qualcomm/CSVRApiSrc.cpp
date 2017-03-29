#include "CSVRApiSrc.h"
#include <dlfcn.h>
#include <Base/MojingLog.h>
#include "../../Base/MojingTypes.h"
#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif


CSVRApi::CSVRApi():
m_bInit(false)
{
	LOGE("F:%s", __FUNCTION__);
}


CSVRApi::~CSVRApi()
{
	LOGE("F:%s", __FUNCTION__);
}
bool CSVRApi::Init()
{
	MOJING_FUNC_TRACE(g_APIlogger);
	bool bRet = true;
	m_bInit = true;
	LOGE("F:%s", __FUNCTION__);
	return	 bRet;
}
void CSVRApi::Release()
{
	LOGE("F:%s", __FUNCTION__);
}


const char* CSVRApi::GetVersion()
{
	const char *pre = NULL;
	pre = svrGetVersion();
	LOGE("F:%s", __FUNCTION__);
	return pre;
}

bool CSVRApi::Initialize(const svrInitParams* pInitParams)
{
	bool re = false;
	re = svrInitialize(pInitParams);
	LOGE("F:%s", __FUNCTION__);
	return re;
}

void CSVRApi::Shutdown()
{
	svrShutdown();
	LOGE("F:%s", __FUNCTION__);
}

svrDeviceInfo CSVRApi::GetDeviceInfo()
{
	svrDeviceInfo Ret;
	memset(&Ret, 0, sizeof(svrDeviceInfo));
	Ret = svrGetDeviceInfo();
	LOGE("F:%s", __FUNCTION__);
	return Ret;
}

void CSVRApi::SetPerformanceLevels(svrPerfLevel cpuPerfLevel, svrPerfLevel gpuPerfLevel)
{
	svrSetPerformanceLevels(cpuPerfLevel, gpuPerfLevel);
	LOGE("F:%s", __FUNCTION__);
//	if (m_fpSetPerformanceLevels)
//		m_fpSetPerformanceLevels(cpuPerfLevel , gpuPerfLevel);
}

void CSVRApi::BeginVr(const svrBeginParams* pBeginParams)
{
	int ire = svrBeginVr(pBeginParams);
	if( ire != 0 ){
		LOGI("svrBeginVr return error, val:%d", ire);
	}
	LOGE("F:%s", __FUNCTION__);
//	if (m_fpBeginVr)
//		m_fpBeginVr(pBeginParams);
}

void CSVRApi::EndVr()
{
	svrEndVr();
	LOGE("F:%s", __FUNCTION__);
}

float CSVRApi::GetPredictedDisplayTime()
{
	float  re = 0.0f;
	re = svrGetPredictedDisplayTime();
	LOGE("F:%s", __FUNCTION__);
	return re;
}
svrHeadPoseState CSVRApi::GetPredictedHeadPose(float predictedTimeMs)
{
	svrHeadPoseState Ret;
	memset(&Ret, 0, sizeof(svrHeadPoseState));
	Ret = svrGetPredictedHeadPose(predictedTimeMs);
	LOGE("F:%s", __FUNCTION__);
	return Ret;		 
}

void CSVRApi::RecenterPose()
{
	svrRecenterPose();
	LOGE("F:%s", __FUNCTION__);
}

unsigned int CSVRApi::GetSupportedTrackingModes()
{
	unsigned int re = 0;
	re = svrGetSupportedTrackingModes();
	LOGE("F:%s", __FUNCTION__);
	return 0;
}
void CSVRApi::SetTrackingMode(unsigned int trackingModes)
{
	svrSetTrackingMode(trackingModes);
	LOGE("F:%s", __FUNCTION__);
}
void CSVRApi::SubmitFrame(const svrFrameParams* pFrameParams)
{
	svrSubmitFrame(pFrameParams);
	LOGE("F:%s", __FUNCTION__);
}
void CSVRApi::UpdateWarpmesh(const float* pVertexLeft,
	const float* pVertexRight,
	const size_t numVertex,
	const unsigned int* pIndex,
	const size_t numIndex)
{
	svrUpdateWarpmesh(pVertexLeft, pVertexRight, numVertex, pIndex, numIndex);
	LOGE("F:%s", __FUNCTION__);
}
bool CSVRApi::CheckServiceIsAvaliable()
{
	bool re = false;
	re = svrCheckServiceIsAvaliable();
	LOGE("F:%s", __FUNCTION__);
	return re;
//	if (m_fpCheckServiceIsAvaliable)
//	{
//		return m_fpCheckServiceIsAvaliable();
//	}
//	else
//	{
//		MOJING_ERROR(g_APIlogger, "fukEyeBufferStereoSeparatenction CheckServiceIsAvaliable is not found in libsvrapi.so");
//		return false;
//	}
		
}

void CSVRApi::SetTimewarp(bool enable)
{
	svrSetTimewarp(enable);
	LOGE("F:%s", __FUNCTION__);

//	if (m_fpSetTimewarp)
//	{
//		m_fpSetTimewarp(enable);
//	}
//	else
//	{
//		MOJING_ERROR(g_APIlogger, "function SetTimewarp(bool) is not found in libsvrapi.so");
//	}
}