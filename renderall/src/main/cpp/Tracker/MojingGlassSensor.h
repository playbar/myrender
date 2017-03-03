#pragma once

#include <android/sensor.h>
#include <android/looper.h>
#include <semaphore.h>

#include "MojingSensor.h"
#include "../Parameters/MojingParameters.h"

namespace Baofeng
{
	namespace Mojing
	{
#pragma  pack(push)
#pragma  pack(1)
		struct SensorInfoTarget
		{
			float  Orientation_X;
			float  Orientation_Y;
			float  Orientation_Z;
			float  Orientation_W;

			float  AngularAccel_X;
			float  AngularAccel_Y;
			float  AngularAccel_Z;

			float  LinearAccel_X;
			float  LinearAccel_Y;
			float  LinearAccel_Z;

			float  Temperature;

			uint32_t  Flags;

			double TimeS;
			double TimeDelta;
		};

		struct MMapedSensorData
		{
			sem_t semlock;
			int cnt;
			SensorInfoTarget data;
		};

		struct SocketSensorData
		{
			uint8_t tag[2];
			uint8_t version;
			uint8_t zero;
			SensorInfoTarget data;
		};

#pragma  pack(pop)

		class GlassSensor : public Sensor
		{
#define LOOPER_ID 1
#define NanoSecondToSecond(nanoSecond) ((nanoSecond) / 1000000000.0)
		public:
			GlassSensor();
			virtual ~GlassSensor();
			virtual int CheckSensors()const;
			virtual int GetMaxSampleRate();
		public:
			//void CreateSensor();
			//void DeleteSensor();
			virtual int Run();
		protected:
			virtual void OnSensorData(MessageBodyFrame& sensorFrame);
			virtual void OnSensorData(Quatf fOrientation, Vector3f fAngularAcceleration, Vector3f fLinearAcceleration, double dTimeInSeconds, float fTemperature);
		protected:
			// Sensor related:
			ASensorEventQueue* m_pQueue;
			ALooper* looper;
			ASensorRef pMagSensorUncalibrated;
			ASensorRef pMagneticFieldSensor;
//			jclass clazz_MojingSDKServiceManager;
//			jmethodID mid_onLowPowerStateChanged;
		private:
			void ReceiveSocketData(MojingSensorParameters * pSensorParameters);
			void ReadMMapData(MojingSensorParameters * pSensorParameters);

		};
	}
}
