#pragma once

#ifndef SVR_VERSION
#define SVR_VERSION 113
#endif

#if (SVR_VERSION == 101)
#include "1.0.1/svrApi.h"
#elif ( SVR_VERSION == 113)
#include "1.1.3/svrApi.h"
#endif
//! \brief Returns the VR SDK version string
typedef const char* (*FP_GetVersion)();

//! \brief Initializes VR components 
//! \param pInitParams svrInitParams structure
typedef bool (*FP_Initialize)(const svrInitParams* pInitParams);

//! \brief Releases VR components
typedef void(*FP_Shutdown)();

//! \brief Queries for device specific information
//! \return svrDeviceInfo structure containing device specific information (resolution, fov, etc..)
typedef  svrDeviceInfo(*FP_GetDeviceInfo)();

//! \brief Requests specific brackets of CPU/GPU performance
//! \param cpuPerfLevel Requested performance level for CPU
//! \param gpuPerfLevel Requested performance level for GPU
typedef  void(*FP_SetPerformanceLevels)(svrPerfLevel cpuPerfLevel, svrPerfLevel gpuPerfLevel);

//! \brief Enables VR services
//! \param pBeginParams svrBeginParams structure
typedef  void(*FP_BeginVr)(const svrBeginParams* pBeginParams);

//! \brief Disables VR services
typedef  void(*FP_EndVr)();

//! \brief Calculates a predicted time when the current frame would be displayed
//! \return Predicted display time for the current frame in milliseconds
typedef  float(*FP_GetPredictedDisplayTime)();

//! \brief Calculates a predicted head pose
//! \param predictedTimeMs Time ahead of the current time in ms to predict a head pose for
//! \return The predicted head pose and relevant pose state information
typedef  svrHeadPoseState(*FP_GetPredictedHeadPose)(float predictedTimeMs);

//! \brief Re centers the head pose at the current position
typedef  void(*FP_RecenterPose)();

//! \brief Returns the supported tracking types
//! \return Bitfield of svrTrackingType values indicating the supported tracking modes
typedef  unsigned int(*FP_GetSupportedTrackingModes)();

//! \brief Sets the current head tracking mode
//! \param trackingModes Bitfield of svrTrackingType values indicating the tracking modes to enable
typedef  void(*FP_SetTrackingMode)(unsigned int trackingModes);

//! \brief Submits a frame to asynchronous time warp
//! \param pFrameParams svrFrameParams structure
typedef  void(*FP_SubmitFrame)(const svrFrameParams* pFrameParams);

typedef void(*FP_UpdateWarpmesh) (
	const float* pVertexLeft,
	const float* pVertexRight,
	const size_t numVertex,
	const unsigned int* pIndex,
	const size_t numIndex
	);

typedef bool(*FP_CheckServiceIsAvaliable)();

typedef void(*FP_SetTimewarp)(bool enable);

/*************************************************************/
typedef bool(*FP_InitializeQvrServiceOnly)();
typedef void(*FP_ReleaseQvrServiceOnly)();
typedef int32_t(*FP_SetCpuPerfLevel)(svrPerfLevel level);
/*************************************************************/

class CSVRApi
{
public:
	CSVRApi();
	virtual ~CSVRApi();
	bool Init();
	void Release();

	const char* GetVersion();
	bool Initialize(const svrInitParams* pInitParams);
	bool InitializeQvrServiceOnly();

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
	void SetCpuPerfLevel(svrPerfLevel cpuPerfLevel);
private:
	void * m_hDLL;
	bool m_bInit;
	FP_GetVersion 				m_fpGetVersion;
	FP_Initialize 				m_fpInitialize;
	FP_Shutdown 				m_fpShutdown;
	FP_GetDeviceInfo 			m_fpGetDeviceInfo;
	FP_SetPerformanceLevels 	m_fpSetPerformanceLevels;
	FP_BeginVr 					m_fpBeginVr;
	FP_EndVr 					m_fpEndVr;
	FP_GetPredictedDisplayTime 	m_fpGetPredictedDisplayTime;
	FP_GetPredictedHeadPose 	m_fpGetPredictedHeadPose;
	FP_RecenterPose 			m_fpRecenterPose;
	FP_GetSupportedTrackingModes m_fpGetSupportedTrackingModes;
	FP_SetTrackingMode 			m_fpSetTrackingMode;
	FP_SubmitFrame 				m_fpSubmitFrame;
	FP_UpdateWarpmesh			m_fpUpdateWarpmesh;
	FP_CheckServiceIsAvaliable  m_fpCheckServiceIsAvaliable;
	FP_SetTimewarp				m_fpSetTimewarp;

	FP_InitializeQvrServiceOnly	m_fpInitializeQvrServiceOnly;
	FP_ReleaseQvrServiceOnly	m_fpReleaseQvrServiceOnly;
	FP_SetCpuPerfLevel	m_fpSetCpuPerfLevel;
};

