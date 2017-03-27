#pragma once

#ifndef SVR_VERSION
#define SVR_VERSION 113
#endif

#if (SVR_VERSION == 101)
#include "1.0.1/svrApi.h"
#elif ( SVR_VERSION == 113)
#include "1.1.3/svrApi.h"
#endif


class CSVRApi
{
public:
	CSVRApi();
	virtual ~CSVRApi();
	bool Init();
	void Release();

	const char* GetVersion();
	bool Initialize(const svrInitParams* pInitParams);
	void Shutdown();
	svrDeviceInfo GetDeviceInfo();
	void SetPerformanceLevels(svrPerfLevel cpuPerfLevel, svrPerfLevel gpuPerfLevel);
	void BeginVr(const svrBeginParams* pBeginParams);
	void EndVr();
	float GetPredictedDisplayTime();
	svrHeadPoseState GetPredictedHeadPose(float predictedTimeMs);
	void RecenterPose();
	unsigned int GetSupportedTrackingModes();
	void SetTrackingMode(unsigned int trackingModes);
	void SubmitFrame(const svrFrameParams* pFrameParams);
	void UpdateWarpmesh(
		const float* pVertexLeft,
		const float* pVertexRight,
		const size_t numVertex,
		const unsigned int* pIndex,
		const size_t numIndex
		);
	bool CheckServiceIsAvaliable();
	void SetTimewarp(bool);
private:
	bool m_bInit;

};

