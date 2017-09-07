#pragma once

#include "../Base/MojingMath.h"
#include "../Base/MojingString.h"
#include "MojingSensorCalibration.h"
#include "MojingSensorFusion.h"

#define USE_SENSORDATA_FROMJAVA

namespace Baofeng
{
	namespace Mojing
	{
		class Tracker
		{
		public:
			Tracker();
			virtual ~Tracker();

#ifdef MJ_OS_ANDROID
			bool StartTrackChecker(int nSampleFrequence = 100);
#endif
			int CheckSensors()const;
			int GetMaxSensorsSampleRate();
			String GetCheckSensorString()const;
			bool StartTrack(int nSampleFrequence = 100, /*const char *pRecordPath = NULL, */const char * lpszModel = NULL, const char * szGlassName = NULL);

			void getLastHeadView(Matrix4f& mHeadView);
			
			Quatf getPredictionPosition(const double& absoluteTimeSeconds, double * pdOutSensotTime = NULL);
			void getLastHeadEulerAngles(float *yaw, float *pitch, float *roll);
			void getLastHeadQuarternion(float *x, float *y, float *z, float *w);
			double getLastSensorState(float* fArray);
			void StopTrack();
			// Resets orientation.
			void ResetTracker();
			// Recenters the orientation on the yaw axis.
			void RecenterYaw();
			void Recenter();
			Matrix4f GetMatrix();
			
			static Quatf   CalculateTimeWarpMatrix(Quatf  qFrom, Quatf qTo);
			static Matrix4f CalculateTimeWarpMatrix2( Quatf  qFrom, Quatf qTo);
			//const ovrSensorState sensor = ovrHmd_GetSensorState(InitParms.Hmd, timePoint, false);
			//const Matrix4f warp = CalculateTimeWarpMatrix2(
			//	currentWarpSource.WarpParms.Images[eye][0].Pose.Pose.Orientation,
			//	sensor.Predicted.Pose.Orientation)

			// For 0.5.1
			//sensor[scan] = ovr_GetSensorStateInternal(timePoint);
			//const Matrix4f warp = CalculateTimeWarpMatrix2(
			//	currentWarpSource.WarpParms.Images[eye][0].Pose.Pose.Orientation,
			//	sensor[scan].Predicted.Pose.Orientation) * velocity;
			//timeWarps[0][scan] = Matrix4f(currentWarpSource.WarpParms.Images[eye][0].TexCoordsFromTanAngles) * warp;
			//int GetCalibrationResetCount()const; 
			float GetCalibrationRate()const;
			void SetCalibrationRate(float fRate)const;
			//void ResetCalibrationResetCount();
			void SendSensorData(float* pArray, double dLastSampleTime);
			CLASS_MEMBER(bool, m_b, DataFromExternal);

		private:
			int m_nSkipSamples;
			bool m_bSensorfusionInit;
			MessageBodyFrame m_sensorFrame;
			friend class Manager;

		private:
			SensorFusion* m_pSensorfusion;
			SensorState m_pLastSensorState;
			void SetYaw();
		private:
			volatile int RefCount;
		};
	}
}
