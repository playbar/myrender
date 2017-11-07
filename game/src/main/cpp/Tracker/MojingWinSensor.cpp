#include <stdio.h>
#include <unistd.h>
#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"
#include "../Base/MojingTypes.h"
#include "../Base/MojingTimer.h"
#include "../Base/MojingLog.h"
#include "MojingTemperature.h"
#include "MojingWinSensor.h"

#include "../LogTraker/LogInterface.h"
#ifdef ENABLE_LOGGER
extern MojingLogger g_Sensorlogger;
#endif

#define	ASENSOR_TYPE_ACCELEROMETER		1
#define	ASENSOR_TYPE_MAGNETIC_FIELD		2
#define	ASENSOR_TYPE_ORIENTATION		3
#define	ASENSOR_TYPE_GYROSCOPE			4
#define	ASENSOR_TYPE_LIGHT				5
#define	ASENSOR_TYPE_PRESSURE			6
#define	ASENSOR_TYPE_TEMPERATURE		7
#define	ASENSOR_TYPE_PROXIMITY			8
#define	ASENSOR_TYPE_GRAVITY			9
#define	ASENSOR_TYPE_LINEAR_ACCELERATION	10
#define	ASENSOR_TYPE_ROTATION_VECTOR		11
#define	ASENSOR_TYPE_RELATIVE_HUMIDITY		12
#define	ASENSOR_TYPE_AMBIENT_TEMPERATURE	13
#define	ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED	14	// Not declared in sensor.h for some reason.
#define	ASENSOR_TYPE_GAME_ROTATION_VECTOR	15
#define	ASENSOR_TYPE_GYROSCOPE_UNCALIBRATED	16
#define	ASENSOR_TYPE_SIGNIFICANT_MOTION		17
#define	ASENSOR_TYPE_STEP_DETECTOR			18
#define	ASENSOR_TYPE_STEP_COUNTER			19
#define	ASENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR	20


namespace Baofeng
{
	namespace Mojing
	{
		WinInternalSensor::WinInternalSensor()
		{

		}

		WinInternalSensor::~WinInternalSensor()
		{
			StopSensor();
		}

		int WinInternalSensor::CheckSensors() const
		{
			int nSensorState = SENSOR_OK;
  
            return nSensorState;
        }
        
		void WinInternalSensor::CreateSensor()
		{
			Manager *pManager = Manager::GetMojingManager();
			MojingSensorParameters * pSensorParameters = NULL;
			if (pManager)
			{
				pSensorParameters = pManager->GetParameters()->GetSensorParameters();
			}
		}

		void WinInternalSensor::DeleteSensor()
		{

		}

		int WinInternalSensor::Run()
		{
			CreateSensor();

			while (!GetExitFlag())
			{
				usleep(20);
			}

			DeleteSensor();

            MOJING_TRACE(g_Sensorlogger, "Exit MojingSenser" );
			return 0;
		}

		void WinInternalSensor::OnSensorData(MessageBodyFrame& sensorFrame)
		{
			if (m_pHandler)
			{
				if ((sensorFrame.LastSampleTime - sensorFrame.lastTempTime) > 0.2f)
				{
					float Temp = getTemperature();
					if (Temp > 0) 
						sensorFrame.Temperature = Temp;
					sensorFrame.lastTempTime = sensorFrame.LastSampleTime;
                    MOJING_TRACE(g_Sensorlogger, "SensorDevice - Temperature = "<< Temp);
				}
				m_pHandler->OnSensorData(sensorFrame);
			}
		}
	}
}
