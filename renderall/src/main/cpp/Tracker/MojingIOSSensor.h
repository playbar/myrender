#pragma once

#include "MojingSensor.h"

#ifdef _DEBUG
#undef ENABLE_SENSOR_LOGGER
#endif

namespace Baofeng
{
	namespace Mojing
	{
		class IOSInternalSensor : public Sensor
		{
		public:
			IOSInternalSensor();
			virtual ~IOSInternalSensor();

			virtual int CheckSensors() const;

		public:
			void CreateSensor();
			void DeleteSensor();
			virtual int Run();

		private:
			virtual void OnSensorData(MessageBodyFrame& sensorFrame);

		private:
			// Sensor related:
            
		};
	}
}
