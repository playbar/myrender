#pragma once

#include <android/sensor.h>
#include <android/looper.h>

#include "MojingSensor.h"

namespace Baofeng
{
	namespace Mojing
	{
		class AndroidInternalSensor : public Sensor
		{
#define LOOPER_ID 1
#define NanoSecondToSecond(nanoSecond) ((nanoSecond) / 1000000000.0)
		public:
			AndroidInternalSensor();
			virtual ~AndroidInternalSensor();
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
			ASensorRef pAccelerometerSensor;
			ASensorRef pGyroScopeSensor;
			ASensorRef pMagSensorUncalibrated;
			ASensorRef pMagneticFieldSensor;
		};
	}
}
