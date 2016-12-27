#pragma once

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
