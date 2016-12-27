#include <stdio.h>
#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"
#include "../Base/MojingTypes.h"
#include "../Base/MojingTimer.h"
#include "../Base/MojingLog.h"
#include "MojingTemperature.h"
#import <UIKit/UIKit.h>
#import<CoreFoundation/CoreFoundation.h>
#include <CoreMotion/CMMotionManager.h>
#include "MojingIOSSensor.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif

#include "../MojingSDKStatus.h"


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
#define LOOPER_ID 1
#define US_PER_SAMPLE(frequence)	(1000L * 1000 / (frequence))
// #define max(a, b) ((a)>(b))?(a):(b) //MojingTypes已经增加此定义
#define NanoSecondToSecond(nanoSecond) ((nanoSecond) / 1000000000.0)

        CMMotionManager *motionManager;
        CMAccelerometerData *newestAccel;
        CMGyroData *newestGyro;
        CMMagnetometerData *newestMagnetometer;
        NSOperationQueue *queueAccel = nil;
        NSOperationQueue *queueGyro = nil;
        NSOperationQueue *queueMagnet = nil;
		IOSInternalSensor::IOSInternalSensor()
		{
            motionManager = nil;
            newestGyro = nil;
            newestAccel = nil;
            newestMagnetometer = nil;
		}

		IOSInternalSensor::~IOSInternalSensor()
		{
			// In any case user forgot stop sensor......
			StopSensor();
		}

		int IOSInternalSensor::CheckSensors() const
		{
			int nSensorState = SENSOR_OK;
            motionManager = [[CMMotionManager alloc] init];
            queueAccel = [[NSOperationQueue alloc] init];
            queueGyro = [[NSOperationQueue alloc] init];
            queueMagnet = [[NSOperationQueue alloc] init];
            NSTimeInterval interval = 1.0/m_nSampleFrequence;
            
            // Check Accelerometer
            if (!motionManager.accelerometerAvailable)
            {
                nSensorState |= SENSOR_NoAccel;
            }
            else
            {
                motionManager.accelerometerUpdateInterval = interval;
                [motionManager startAccelerometerUpdatesToQueue:queueAccel withHandler:^(CMAccelerometerData *latestAcc, NSError *error)
                 {
                     if(error)
                     {
                        MOJING_ERROR(g_Sensorlogger, "MojingSDK CheckSensors Get accelerometer failed!");
                     }
                     else
                     {
                        if(newestAccel == nil)
                            newestAccel = latestAcc;
                     }
                 }];
            }
            
            // Check GyroScope
            if (!motionManager.gyroAvailable)
            {
                nSensorState |= SENSOR_NoGryo;
            }
            else
            {
                motionManager.gyroUpdateInterval = interval;
                [motionManager startGyroUpdatesToQueue:queueGyro withHandler:^(CMGyroData *latestGyro,NSError *error)
                 {
                     if(error)
                     {
                         MOJING_ERROR(g_Sensorlogger, "MojingSDK CheckSensors Get Gyro failed!");
                     }
                     else
                     {
                         if(newestGyro == nil)
                             newestGyro = latestGyro;
                     }
                 }];
            }
            
            // Check MagSensorUncalibrated
            if ((!motionManager.magnetometerAvailable) && (!motionManager))
            {
                nSensorState |= SENSOR_NoMag;
            }
            else
            {
                motionManager.magnetometerUpdateInterval = interval;
                [motionManager startMagnetometerUpdatesToQueue:queueMagnet withHandler:^(CMMagnetometerData *lastestMagnet, NSError *error)
                 {
                     if(error)
                     {
                         MOJING_ERROR(g_Sensorlogger, "MojingSDK CheckSensors Get magnetometer failed!");
                     }
                     else
                     {
                         if(newestMagnetometer == nil)
                             newestMagnetometer = lastestMagnet;
                     }
                 }];
            }
            return nSensorState;
        }
        
        void IOSInternalSensor::CreateSensor()
        {
            Manager *pManager = Manager::GetMojingManager();
			MojingSensorParameters * pSensorParameters = NULL;
			if (pManager)
			{
				pSensorParameters = pManager->GetParameters()->GetSensorParameters();
			}
			// Xu Xiang: Prepare for get sensor event 
			// Initialize on same thread as read.
						
			//int miniDelay = 0;
			//int wantDelay = US_PER_SAMPLE(m_nSampleFrequence);// Hz ==> 10^-6(微秒)
			MOJING_TRACE(g_Sensorlogger, "Wanted Sensor's rate = " << m_nSampleFrequence);
#define MICRO_SECOND 1000000L
		}

#define DELETE_SENSOR(pQueue, pSensor) if (pSensor != NULL) \
		{ \
			ASensorEventQueue_disableSensor(m_pQueue, pSensor);	\
			pSensor = NULL; \
		}

		void IOSInternalSensor::DeleteSensor()
		{
            [motionManager stopAccelerometerUpdates];
            [motionManager stopGyroUpdates];
            [motionManager stopMagnetometerUpdates];
		}

		int g_startcount = 0;
		int IOSInternalSensor::Run()
		{
			char buf[64];
			int myCount = g_startcount++;
			sprintf(buf, "MojingSenser_%d", myCount);

			SetThreadName(buf);

			CreateSensor();
            
            float fMaxTimeSpace = 0;
			float fMinTimeSpace = 0;
			float fAvgTimeSpace = 0;
			UInt64 ui64SampleCount = 0;
			UInt64 ui64FirstSampleTime = 0; 
			MojingSensorParameters *pSensorParameters = Manager::GetMojingManager()->GetParameters()->GetSensorParameters();

            MessageBodyFrame theSample;
			memset(&theSample, 0, sizeof(MessageBodyFrame));
            theSample.LastSampleTime = theSample.lastTempTime = NanoSecondToSecond(Timer::GetTicksNanos());
			theSample.Temperature = getTemperature();
			if (theSample.Temperature < 0) theSample.Temperature = 2500;
            NSTimeInterval sleepInterval = 0.5/m_nSampleFrequence;
            
			while (!GetExitFlag())
			{
                //ASENSOR_TYPE_ACCELEROMETER
                if(newestAccel != nil)
                {
                MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
				if (pStatus->GetEngineStatus() == ENGINE_UNREAL)
                {
                    theSample.Acceleration.x = newestAccel.acceleration.y;
                    theSample.Acceleration.y = -newestAccel.acceleration.x;
                    theSample.Acceleration.z = newestAccel.acceleration.z;
                }
                else
                {
                    theSample.Acceleration.x = -newestAccel.acceleration.y;
                    theSample.Acceleration.y = newestAccel.acceleration.x;
                    theSample.Acceleration.z = newestAccel.acceleration.z;
                }
                    newestAccel = nil;
#ifdef ENABLE_SENSOR_LOGGER
                    MOJING_TRACE(g_Sensorlogger,  "Accel(" << theSample.Acceleration.x << ", " << theSample.Acceleration.y <<
                                 ", " << theSample.Acceleration.z << ")");
#endif
                }

                //ASENSOR_TYPE_GYROSCOPE
                if(newestGyro != nil)
                {
                	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
                	if (pStatus->GetEngineStatus() == ENGINE_UNREAL)
                    {
	                    theSample.RotationRate.x = newestGyro.rotationRate.y;
	                    theSample.RotationRate.y = -newestGyro.rotationRate.x;
	                    theSample.RotationRate.z = newestGyro.rotationRate.z;
	                }
	                else
	                {
	                    theSample.RotationRate.x = -newestGyro.rotationRate.y;
	                    theSample.RotationRate.y = newestGyro.rotationRate.x;
	                    theSample.RotationRate.z = newestGyro.rotationRate.z;
	                }
                    newestGyro = nil;
                    //theSample.TimeDelta = (float)[[NSDate date] timeIntervalSince1970] - theSample.LastSampleTime;
                    //theSample.LastSampleTime = (float)[[NSDate date] timeIntervalSince1970];
                    theSample.TimeDelta =  NanoSecondToSecond(Timer::GetTicksNanos()) - theSample.LastSampleTime;
                    theSample.LastSampleTime = NanoSecondToSecond(Timer::GetTicksNanos());
                    theSample.AbsoluteTimeSeconds = Timer::GetSeconds();
                    if (ui64FirstSampleTime == 0)
                    {
                        ui64SampleCount = 1;
                        fMinTimeSpace = fMaxTimeSpace = theSample.TimeDelta;
                    }
                    else
                    {
                        ui64SampleCount++;
                        fMinTimeSpace = std::min(fMinTimeSpace, theSample.TimeDelta);
                        fMaxTimeSpace = std::max(fMaxTimeSpace, theSample.TimeDelta);
                        if (ui64SampleCount % 50 == 0)
                        {
                            //float fLast50AvgTimeSpace =
                            pSensorParameters->SetAvgSampleRate(1 / fAvgTimeSpace);
                            pSensorParameters->SetMaxSampleRate(1 / fMinTimeSpace);
                            pSensorParameters->SetMinSampleRate(1 / fMaxTimeSpace);
                            //pSensorParameters->SetLast50AvgSampleRate(1 / fLast50AvgTimeSpace);
                        }
                    }
#ifdef ENABLE_SENSOR_LOGGER
                    MOJING_TRACE(g_Sensorlogger, "Gyro(" << theSample.RotationRate.x << ", " << theSample.RotationRate.y << ", " <<
                                 theSample.RotationRate.z << ")");
#endif
                    if (theSample.TimeDelta > 0.002f)
                        OnSensorData(theSample);
                }
                //ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED
                if(newestMagnetometer != nil)
                {
                    theSample.MagneticField.Set(-newestMagnetometer.magneticField.y,
                                                newestMagnetometer.magneticField.x,
                                                newestMagnetometer.magneticField.z);
                    newestMagnetometer = nil;
#ifdef ENABLE_SENSOR_LOGGER
                    MOJING_TRACE(g_Sensorlogger, "MagUncal(" << theSample.MagneticField.x << ", " << theSample.MagneticField.y << ", " <<
                                 theSample.MagneticField.z << ")");
#endif
                    //theSample.MagneticBias.Set(-newestMagnetometer.magneticBias.y_bias,
                    //                           newestMagnetometer.magneticBias.x_bias,
                    //                           newestMagnetometer.magneticBias.z_bias);

                    // Phone values are in micro-Tesla. Convert it to Gauss and flip axes.
                    theSample.MagneticBias *= 10000.0f / 1000000.0f;
                    theSample.MagneticField *= 10000.0f / 1000000.0f;
                }
#if 0
                //ASENSOR_TYPE_MAGNETIC_FIELD
                {
                    
                    CMMagnetometerData *newestMagnetometer = motionManager.magnetometerData;
                    theSample.MagneticField.Set(-newestMagnetometer.magneticField.y, newestMagnetometer.magneticField.x, newestMagnetometer.magneticField.z);
                    // Phone values are in micro-Tesla. Convert it to Gauss and flip axes.
                    theSample.MagneticField *= 10000.0f / 1000000.0f;
                }
#endif
                [NSThread sleepForTimeInterval:sleepInterval];
			}

			DeleteSensor();
            MOJING_TRACE(g_Sensorlogger, "Exit MojingSenser " << myCount);
			return 0;
		}

		void IOSInternalSensor::OnSensorData(MessageBodyFrame& sensorFrame)
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
