#pragma once

#include <Sensors/Vector3d.h>
#include <Sensors/OrientationEKF.h>
#include <Sensors/GyroscopeBiasEstimator.h>
#include <Sensors/Matrix.h>
#include "../Base/MojingMath.h"
#include "../Base/MojingThreads.h"
#include "../Base/MojingString.h"
#include "../Parameters/MojingParameters.h"

#ifdef _DEBUG
//#define ENABLE_SENSOR_LOGGER
#endif

namespace Baofeng
{
	namespace Mojing
	{
#define SENSOR_OK			0x00
#define SENSOR_NoMag		0x01// 没有地磁
#define SENSOR_NoGryo		0x04// 没有陀螺仪
#define SENSOR_GryoTooSlow	0x08// 陀螺仪采样速率过低 < 50Hz
#define SENSOR_NoAccel		0x10// 没有加速度传感器
#define SENSOR_AccelTooSlow	0x20// 加速度传感器采样速率过低 < 50Hz

		struct MessageBodyFrame
		{
			Vector3f Acceleration;  // Acceleration in m/s^2.
			Vector3f RotationRate;  // Angular velocity in rad/s.	
			Vector3f MagneticField; // Magnetic field strength in Gauss.
			Vector3f MagneticBias;  // Magnetic field calibration bias in Gauss.
			float    Temperature;   // Temperature reading on sensor surface, in degrees Celsius.
			float    TimeDelta;     // Time passed since last Body Frame, in seconds.
			double	 LastSampleTime;
			double   lastTempTime;
			double   AbsoluteTimeSeconds;
		};

		struct HeadTrackerData : public MessageBodyFrame
		{
			Vector3dJ gyroBias;
			Vector3dJ latestGyro;
			Vector3dJ latestAcc;
			GyroscopeBiasEstimator gyroBiasEstimator;
			OrientationEKF tracker;
			long latestGyroEventClockTimeNs = 0;
			float initialSystemGyroBias[3];
			float neckModelTranslation[16];

			float neckModelFactor = 1.0;
            float displayRotation = 0.0;
            void setNeckModelFactor(float factor){
                if( factor < 0.0 || factor > 1.0 )
                    return;
                neckModelFactor = factor;
            }

            void getLastHeadView(float *headView){
                float rotation = 0.0f;
                displayRotation = rotation;
                float sensorToDisplay[16];
                float ekfToHeadTracker[16];
				float tmpHeadView[16];
				float tmpHeadView2[16];
                Matrix::setRotateEulerM(sensorToDisplay, 0, 0, 0, -rotation);
                Matrix::setRotateEulerM(ekfToHeadTracker, 0, -90, 0, rotation);

                    if (!tracker.isReady())
					{
                        return;
                    }
                    double secondsSinceLastGyroEvent = (GetTimeNano() - latestGyroEventClockTimeNs)/1000000000.0;
                    double secondsToPredictForward = secondsSinceLastGyroEvent + 0.057999998331069946;
                    double *mat = tracker.getPredictedGLMatrix(secondsToPredictForward);
                    for (int i = 0; i < 16; ++i) {
                        tmpHeadView[i] = (float)mat[i];
                    }

                Matrix::multiplyMM(tmpHeadView2, sensorToDisplay, tmpHeadView);
                Matrix::multiplyMM(headView, tmpHeadView2, ekfToHeadTracker);
                Matrix::setIdentityM(neckModelTranslation,  0);
                Matrix::translateM(neckModelTranslation,  0, (float) 0.0f,  ((-neckModelFactor) * 0.075f),  (neckModelFactor * 0.08f));
                Matrix::multiplyMM(tmpHeadView, neckModelTranslation, headView);
                Matrix::translateM(headView, 0, tmpHeadView,  0,  0.0f,  (neckModelFactor * 0.075f), (float) 0.0f);
				return;
            }

		};

		// SensorDataHandler is a base class from which users derive to receive Sensor Data
		class Sensor;
		class SensorDataHandler
		{
			friend class Sensor;
		public:
			SensorDataHandler();
			virtual ~SensorDataHandler();

			void RemoveHandlerFromSensor();
			virtual void OnSensorData(MessageBodyFrame&) = 0;
			virtual void OnSensorData(Quatf fOrientation, Vector3f fAngularAcceleration, Vector3f fLinearAcceleration, double dTimeInSeconds, float fTemperature) = 0;

		private:
			Sensor* m_pSensor;
		};

		class Sensor : public Thread
		{
		public:
			Sensor();
			virtual ~Sensor();

		public:
			virtual void Initialize(Parameters* pParameters);
			virtual int CheckSensors()const = 0;
			bool StartSensor(int nSampleFrequence = 100/*, const char *pRecordPath = NULL*/);
			virtual void StopSensor();

			void AttachHandler(SensorDataHandler* pHandler);
			void DetachHandler();

			virtual void GetFactoryCalibration(FactoryCalibration* pFactoryCalibration);
			virtual bool GetGyroOffsetReport(GyroOffsetReport* pReport);
			virtual int GetMaxSampleRate(){ return m_iMaxSampleRate; };
		private:
			virtual void OnSensorData(MessageBodyFrame& sensorFrame) = 0;

		protected:
			SensorDataHandler* m_pHandler;
			int m_nSampleFrequence;
			int m_iMaxSampleRate;
			Parameters* m_pParameters;
			MojingFactoryCalibrationParameters* m_pFactoryCalibrationParam;
			MojingGyroOffsetReportSet* m_pGyroOffsetReportSet;
		};
	}
}
