#include "CSVRApiSrc.h"
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


CSVRApi::CSVRApi():
m_bInit(false)
{
}


CSVRApi::~CSVRApi()
{
}
bool CSVRApi::Init()
{
	MOJING_FUNC_TRACE(g_APIlogger);
	bool bRet = true;
	m_bInit = true;
	return	 bRet;
}
void CSVRApi::Release()
{

}


const char* CSVRApi::GetVersion()
{
	const char *pre = NULL;
	pre = svrGetVersion();
	return pre;
}

bool CSVRApi::Initialize(const svrInitParams* pInitParams)
{
	bool re = false;
	re = svrInitialize(pInitParams);
	return re;
}

void CSVRApi::Shutdown()
{
	svrShutdown();
}

svrDeviceInfo CSVRApi::GetDeviceInfo()
{
	svrDeviceInfo Ret;
	memset(&Ret, 0, sizeof(svrDeviceInfo));
	Ret = svrGetDeviceInfo();
	return Ret;
}

void CSVRApi::SetPerformanceLevels(svrPerfLevel cpuPerfLevel, svrPerfLevel gpuPerfLevel)
{
	svrSetPerformanceLevels(cpuPerfLevel, gpuPerfLevel);
//	if (m_fpSetPerformanceLevels)
//		m_fpSetPerformanceLevels(cpuPerfLevel , gpuPerfLevel);
}

void CSVRApi::BeginVr(const svrBeginParams* pBeginParams)
{
	svrBeginVr(pBeginParams);
//	if (m_fpBeginVr)
//		m_fpBeginVr(pBeginParams);
}

void CSVRApi::EndVr()
{
	svrEndVr();
}

float CSVRApi::GetPredictedDisplayTime()
{
	float  re = 0.0f;
	re = svrGetPredictedDisplayTime();
	return re;
}
svrHeadPoseState CSVRApi::GetPredictedHeadPose(float predictedTimeMs)
{
	svrHeadPoseState Ret;
	memset(&Ret, 0, sizeof(svrHeadPoseState));
	Ret = svrGetPredictedHeadPose(predictedTimeMs);
	return Ret;		 
}

void CSVRApi::RecenterPose()
{
	svrRecenterPose();
}

unsigned int CSVRApi::GetSupportedTrackingModes()
{
	unsigned int re = 0;
	re = svrGetSupportedTrackingModes();
	return 0;
}
void CSVRApi::SetTrackingMode(unsigned int trackingModes)
{
	svrSetTrackingMode(trackingModes);
}
void CSVRApi::SubmitFrame(const svrFrameParams* pFrameParams)
{
	svrSubmitFrame(pFrameParams);
}
void CSVRApi::UpdateWarpmesh(const float* pVertexLeft,
	const float* pVertexRight,
	const size_t numVertex,
	const unsigned int* pIndex,
	const size_t numIndex)
{
	svrUpdateWarpmesh(pVertexLeft, pVertexRight, numVertex, pIndex, numIndex);
}
bool CSVRApi::CheckServiceIsAvaliable()
{
	bool re = false;
	re = svrCheckServiceIsAvaliable();
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

//	if (m_fpSetTimewarp)
//	{
//		m_fpSetTimewarp(enable);
//	}
//	else
//	{
//		MOJING_ERROR(g_APIlogger, "function SetTimewarp(bool) is not found in libsvrapi.so");
//	}
}