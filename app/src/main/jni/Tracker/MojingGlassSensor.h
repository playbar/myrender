#pragma once

#include <android/sensor.h>
#include <android/looper.h>
#include <semaphore.h>

#include "MojingSensor.h"

namespace Baofeng
{
	namespace Mojing
	{
	struct SensorDataTarget
			{
				float  Gryo_X;
				float  Gryo_Y;
				float  Gryo_Z;

				float  Accl_X;
				float  Accl_Y;
				float  Accl_Z;

				float  Mag_X;
				float  Mag_Y;
				float  Mag_Z;

				float  Temperature;

				uint32_t  Flags;

				uint32_t TimeMS;
			};

		struct MMapedSensorData
		{
			sem_t semlock;
			int cnt;
			SensorDataTarget data;
		};

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
			void CreateSensor();
			void DeleteSensor();
			virtual int Run();
		protected:
			virtual void OnSensorData(MessageBodyFrame& sensorFrame);
		protected:
			// Sensor related:
			ASensorEventQueue* m_pQueue;
			ALooper* looper;
			ASensorRef pMagSensorUncalibrated;
			ASensorRef pMagneticFieldSensor;
//			jclass clazz_MojingSDKServiceManager;
//			jmethodID mid_onLowPowerStateChanged;
		};
	}
}
