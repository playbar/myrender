#include <stdio.h>
#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"
#include "../Base/MojingTypes.h"
#include "../Base/MojingTimer.h"
#include "../Base/MojingLog.h"
#include "MojingTemperature.h"
#ifdef MJ_OS_ANDROID
#include "MojingAndroidSensor.h"
#endif

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_Sensorlogger;
extern MojingLogger g_APIlogger;
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

#define US_PER_SAMPLE(frequence)	(1000L * 1000 / (frequence))
// #define max(a, b) ((a)>(b))?(a):(b) //MojingTypes已经增加此定义
		AndroidInternalSensor::AndroidInternalSensor()
			: m_pQueue(NULL)
			, pAccelerometerSensor(NULL)
			, pGyroScopeSensor(NULL)
			, pMagSensorUncalibrated(NULL)
			, pMagneticFieldSensor(NULL)
			, looper(NULL)
		{
		}

		AndroidInternalSensor::~AndroidInternalSensor()
		{
			// In any case user forgot stop sensor......
			StopSensor();
		}

		int AndroidInternalSensor::CheckSensors()const
		{
			int nSensorState = SENSOR_OK;

			ASensorManager* pSensorManager = ASensorManager_getInstance();

			// Check Accelerometer
			ASensorRef pSensor = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_ACCELEROMETER);
			if (pSensor == NULL)
			{
				nSensorState |= SENSOR_NoAccel;
			}
			else
			{
				int miniDelay = ASensor_getMinDelay(pSensor);
				if (miniDelay > US_PER_SAMPLE(50))
				{
					nSensorState |= SENSOR_AccelTooSlow;
				}
			}

			// Check GyroScope
			pSensor = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_GYROSCOPE);
			if (pSensor == NULL)
			{
				nSensorState |= SENSOR_NoGryo;
			}
			else
			{
				int miniDelay = ASensor_getMinDelay(pSensor);
				if (miniDelay > US_PER_SAMPLE(50))
				{
					nSensorState |= SENSOR_GryoTooSlow;
				}
			}

			// Check MagSensorUncalibrated
			ASensorRef pSensorMag;
			pSensor = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED);
			pSensorMag = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
			if ((pSensor == NULL) && (pSensorMag == NULL))
			{
				nSensorState |= SENSOR_NoMag;
			}
			return nSensorState;
		}

		int AndroidInternalSensor::GetMaxSampleRate()
		{
			if (m_iMaxSampleRate == 0)
			{
				ASensorManager* pSensorManager = ASensorManager_getInstance();
				if (pSensorManager)
				{
					ASensorRef pSensor = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_GYROSCOPE);
					if (pSensor)
					{
						int iMiniDelay = ASensor_getMinDelay(pSensor);
						if (iMiniDelay)
						{
							m_iMaxSampleRate = US_PER_SAMPLE(iMiniDelay);
						}
						else
						{
							MOJING_ERROR(g_Sensorlogger, "ASensor_getMinDelay==0");
						}
					}
				}
			}
			return m_iMaxSampleRate;
		}
		void AndroidInternalSensor::CreateSensor()
		{
			Manager *pManager = Manager::GetMojingManager();
			MojingSensorParameters * pSensorParameters = NULL;
			if (pManager)
			{
				pSensorParameters = pManager->GetParameters()->GetSensorParameters();
			}
			// Xu Xiang: Prepare for get sensor event 
			// Initialize on same thread as read.
			ASensorManager* pSensorManager = ASensorManager_getInstance();
			looper = ALooper_forThread();
			if (looper == NULL)
			{
				looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
			}
			m_pQueue = ASensorManager_createEventQueue(pSensorManager, looper, LOOPER_ID, NULL, NULL);
			
			

			pAccelerometerSensor = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_ACCELEROMETER);
			pGyroScopeSensor = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_GYROSCOPE);
			pMagSensorUncalibrated = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED);
			pMagneticFieldSensor = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);

			int miniDelay = 0;
			int wantDelay = US_PER_SAMPLE(m_nSampleFrequence);// Hz ==> 10^-6(微秒)
			MOJING_TRACE(g_Sensorlogger, "Wanted Sensor's rate = " << m_nSampleFrequence);
#define MICRO_SECOND 1000000L
			if (pMagSensorUncalibrated != NULL) //优先使用ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED
			{
				miniDelay = ASensor_getMinDelay(pMagSensorUncalibrated); //微秒/次
				miniDelay = miniDelay > wantDelay ? miniDelay : wantDelay; //如果值<miniDelay 将出错, 我们希望最大速率
				ASensorEventQueue_enableSensor(m_pQueue, pMagSensorUncalibrated);
				ASensorEventQueue_setEventRate(m_pQueue, pMagSensorUncalibrated, miniDelay);
				MOJING_TRACE(g_Sensorlogger, "MagUncalibrated Sensor's rate = " << MICRO_SECOND / miniDelay);
				if (pSensorParameters)
				{
					pSensorParameters->SetMagSensorUncalibrated(MICRO_SECOND / miniDelay);
				}
			}
			else if (pMagneticFieldSensor != NULL)
			{
				// If the phone do not support
				miniDelay = ASensor_getMinDelay(pMagneticFieldSensor); //微秒/次
				miniDelay = miniDelay > wantDelay ? miniDelay : wantDelay;
				ASensorEventQueue_enableSensor(m_pQueue, pMagneticFieldSensor);
				ASensorEventQueue_setEventRate(m_pQueue, pMagneticFieldSensor, miniDelay);
				MOJING_TRACE(g_Sensorlogger, "MagneticField Sensor's rate = " << MICRO_SECOND / miniDelay);
				if (pSensorParameters)
					pSensorParameters->SetMagneticFieldSensor (MICRO_SECOND / miniDelay);
			}
			else
			{
#ifdef MJ_OS_ANDROID
				MOJING_WARN(g_Sensorlogger, "This phone do not have MagneticField Sensor.");
#endif
			}

			if (pGyroScopeSensor != NULL)
			{
				miniDelay = ASensor_getMinDelay(pGyroScopeSensor); //微秒/次
				miniDelay = miniDelay > wantDelay ? miniDelay : wantDelay;
				ASensorEventQueue_enableSensor(m_pQueue, pGyroScopeSensor);
				ASensorEventQueue_setEventRate(m_pQueue, pGyroScopeSensor, miniDelay);
				MOJING_TRACE(g_Sensorlogger, "GyroScope Sensor's rate = " << MICRO_SECOND / miniDelay);
				if (pSensorParameters)
					pSensorParameters->SetGyroScopeSensor(MICRO_SECOND / miniDelay);
			}
			else
			{
#ifdef MJ_OS_ANDROID
				MOJING_ERROR(g_Sensorlogger, "This phone do not have GyroScope Sensor.");
#endif
			}

			if (pAccelerometerSensor != NULL)
			{
				miniDelay = ASensor_getMinDelay(pAccelerometerSensor); //微秒/次
				miniDelay = miniDelay > wantDelay ? miniDelay : wantDelay;
				ASensorEventQueue_enableSensor(m_pQueue, pAccelerometerSensor);
				ASensorEventQueue_setEventRate(m_pQueue, pAccelerometerSensor, miniDelay);
				MOJING_TRACE(g_Sensorlogger, "Accelerometer Sensor's rate = " << MICRO_SECOND / miniDelay);
				if (pSensorParameters)
					pSensorParameters->SetAccelerometerSensor(MICRO_SECOND / miniDelay);
			}
			else
			{
#ifdef MJ_OS_ANDROID
				MOJING_WARN(g_Sensorlogger, "This phone do not have Accelerometer Sensor.");
#endif
			}
		}

#define DELETE_SENSOR(pQueue, pSensor) if (pSensor != NULL) \
		{ \
			ASensorEventQueue_disableSensor(m_pQueue, pSensor);	\
			pSensor = NULL; \
		}

		void AndroidInternalSensor::DeleteSensor()
		{
			DELETE_SENSOR(m_pQueue, pAccelerometerSensor);
			DELETE_SENSOR(m_pQueue, pGyroScopeSensor);
			DELETE_SENSOR(m_pQueue, pMagSensorUncalibrated);
			DELETE_SENSOR(m_pQueue, pMagneticFieldSensor);
			if (m_pQueue != NULL)
			{
				ASensorManager* pSensorManager = ASensorManager_getInstance();
				ASensorManager_destroyEventQueue(pSensorManager, m_pQueue);
				m_pQueue = NULL;
			}
			//ALooper_release(looper);	// TBD: need or not need
			looper = NULL;
		}

		int g_startcount = 0;
		int AndroidInternalSensor::Run()
		{
			char buf[64];
			int myCount = g_startcount++;
			sprintf(buf, "MojingSenser_%d", myCount);

			SetThreadName(buf);
			MOJING_TRACE(g_Sensorlogger, "Start MojingSenser " << myCount);

			CreateSensor();
			/*以下是为了统计陀螺仪的实际采样速度*/
			float fMaxTimeSpace = 0;
			float fMinTimeSpace = 0;
			float fAvgTimeSpace = 0;
			UInt64 ui64SampleCount = 0;
			UInt64 ui64FirstSampleTime = 0; 
			UInt64 ui64Last50FirstSampleTime = 0;
			MojingSensorParameters *pSensorParameters = Manager::GetMojingManager()->GetParameters()->GetSensorParameters();
			
			/*以上是为了统计陀螺仪的实际采样速度*/
			MessageBodyFrame theSample;
			memset(&theSample, 0, sizeof(MessageBodyFrame));
			theSample.Temperature = getTemperature();
			if (theSample.Temperature < 0) theSample.Temperature = 2500;	// 如果不能拿到温度值，则设置成25度

			// 有些手机刚开始启动陀螺仪时，给出的数据完全不靠谱，需要扔掉几个采样
			// 这样可以把给初始时间的获取，也放在这里
			int ident = 0;
			int fd = 0;
			int events = 0;
			int skipSamples = 10;
			while (skipSamples >0 && !GetExitFlag())
			{
				while ((ident = ALooper_pollAll(3, &fd, &events, NULL)) >= 0)
				{
					if (ident == LOOPER_ID)
					{
						ASensorEvent event;
						while (ASensorEventQueue_getEvents(m_pQueue, &event, 1) > 0)
						{
							if (event.type == ASENSOR_TYPE_GYROSCOPE)
							{
								skipSamples--;
								theSample.LastSampleTime = theSample.lastTempTime = NanoSecondToSecond(event.timestamp);
#ifdef ENABLE_SENSOR_LOGGER
								MOJING_TRACE(g_Sensorlogger, "Skip Gyro (" << event.vector.x << ", " << event.vector.y << ", " <<
									event.vector.z << ")");
#endif
							}
						}
					}
				}
			}

			while (!GetExitFlag())
			{
				while ((ident = ALooper_pollAll(3, &fd, &events, NULL) >= 0) && !GetExitFlag())
				{
					if (ident == LOOPER_ID)
					{
						ASensorEvent event;
						while (ASensorEventQueue_getEvents(m_pQueue, &event, 1) > 0)
						{
							//if (theSample.LastSampleTime == 0)
							//{
							//	theSample.LastSampleTime = theSample.lastTempTime = NanoSecondToSecond(event.timestamp);
							//}
							if (event.type == ASENSOR_TYPE_ACCELEROMETER)
							{
								//SensorData *pAccel = new SensorData;
								theSample.Acceleration.x = -event.acceleration.y;
								theSample.Acceleration.y = event.acceleration.x;
								theSample.Acceleration.z = event.acceleration.z;
#ifdef ENABLE_SENSOR_LOGGER
								MOJING_TRACE(g_Sensorlogger, "time = " << event.timestamp / 1000000 <<
									", Accel(" << theSample.Acceleration.x << ", " << theSample.Acceleration.y <<
									", " << theSample.Acceleration.z << ")");
#endif							
							}
							else if (event.type == ASENSOR_TYPE_GYROSCOPE)
							{
								theSample.RotationRate.x = -event.vector.y;
								theSample.RotationRate.y = event.vector.x;
								theSample.RotationRate.z = event.vector.z;
								/*******************
								注意：下面的代码原本为：
								theSample.TimeDelta = NanoSecondToSecond(event.timestamp) - theSample.LastSampleTime;
								这就意味着如果发送间隔超过了500Hz，那么永远不会有连续两个采样的 间隔是小于阈值0.002秒的。这种情况下就会
								导致几乎所有的采样都被过滤掉。
								将代码修改为只记录上一次被发送出去的数据的时间戳，并且新来的采样只和这个时间比较来确定两个被处理的采样之间的间隔。
								*******************/
								
								static double dLastSampleTime = 0;
								theSample.TimeDelta = NanoSecondToSecond(event.timestamp) - dLastSampleTime;
								theSample.LastSampleTime = NanoSecondToSecond(event.timestamp);
								theSample.AbsoluteTimeSeconds = Timer::GetSeconds();
								if (ui64FirstSampleTime == 0)
								{
									ui64Last50FirstSampleTime = ui64FirstSampleTime = event.timestamp;
									ui64SampleCount = 1;
									fMinTimeSpace = fMaxTimeSpace = theSample.TimeDelta;
								}
								else
								{
									ui64SampleCount++;
									if (ui64SampleCount)// 防溢出处理
										fAvgTimeSpace = NanoSecondToSecond(event.timestamp - ui64FirstSampleTime) / ui64SampleCount;
									fMinTimeSpace = std::min(fMinTimeSpace, theSample.TimeDelta);
									fMaxTimeSpace = std::max(fMaxTimeSpace, theSample.TimeDelta);
									if (ui64SampleCount % 50 == 0)
									{
										float fLast50AvgTimeSpace = NanoSecondToSecond(event.timestamp - ui64Last50FirstSampleTime) / 50;
										pSensorParameters->SetAvgSampleRate(1 / fAvgTimeSpace);
										pSensorParameters->SetMaxSampleRate(1 / fMinTimeSpace);
										pSensorParameters->SetMinSampleRate(1 / fMaxTimeSpace);
										pSensorParameters->SetLast50AvgSampleRate(1 / fLast50AvgTimeSpace);
										ui64Last50FirstSampleTime = event.timestamp;
									}
								}
								
#ifdef ENABLE_SENSOR_LOGGER
								MOJING_TRACE(g_Sensorlogger, "time = " << event.timestamp / 1000000 <<
									", Gyro(" << theSample.RotationRate.x << ", " << theSample.RotationRate.y << ", " <<
									theSample.RotationRate.z << ")");
#endif
// #ifdef _DEBUG
// 								static int64_t i64Last = 0;
// 								if (i64Last == 0)
// 								{
// 									MOJING_TRACE(g_APIlogger, "Gyro Time = " << Timer::FormatDoubleTime(NanoSecondToSecond(event.timestamp)));
// 								}
// 								else
// 								{
// 									MOJING_TRACE(g_APIlogger, "Gyro Time = " << Timer::FormatDoubleTime(NanoSecondToSecond(event.timestamp)) << " AVG = " << 1 / NanoSecondToSecond(event.timestamp - i64Last) << " Hz");
// 								}
// 									i64Last = event.timestamp;
// #endif
								static double dTimeLimit = -1;
								if (dTimeLimit < 0)
								{
									MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
									if (pDeviceParameters)
									{
										if (pDeviceParameters->GetIsMachine())
										{
											dTimeLimit = 0;// 不做区别											
										}
										else
										{
											dTimeLimit = 0.002f;// 超过500Hz认为是发生了数据抖动
										}
									}
									else
									{
										dTimeLimit = 0.002f;// 默认不许超过500Hz
									}
								}
								if (!GetExitFlag() && theSample.TimeDelta > dTimeLimit)
								{
									dLastSampleTime = theSample.LastSampleTime;
									{
										double dSysTimeOffset = (Timer::GetSeconds() - NanoSecondToSecond(event.timestamp));
										//MOJING_TRACE(g_Sensorlogger , Timer::FormatDoubleTimeInMS(dSysTimeOffset));
									}
									OnSensorData(theSample);
								}
							}
							else if (event.type == ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED)
							{
								theSample.MagneticField.Set(-event.uncalibrated_magnetic.y_uncalib,
									event.uncalibrated_magnetic.x_uncalib,
									event.uncalibrated_magnetic.z_uncalib);

								theSample.MagneticBias.Set(-event.uncalibrated_magnetic.y_bias,
									event.uncalibrated_magnetic.x_bias,
									event.uncalibrated_magnetic.z_bias);

								// Phone values are in micro-Tesla. Convert it to Gauss and flip axes.
								theSample.MagneticField *= 10000.0f / 1000000.0f;
								theSample.MagneticBias *= 10000.0f / 1000000.0f;

#ifdef ENABLE_SENSOR_LOGGER
								MOJING_TRACE(g_Sensorlogger, "time = " << event.timestamp / 1000000 <<
									", MagUncal(" << theSample.MagneticField.x << ", " << theSample.MagneticField.y << ", " <<
									theSample.MagneticField.z << ")");
#endif
							}
							else if (event.type == ASENSOR_TYPE_MAGNETIC_FIELD)
							{
								theSample.MagneticField.Set(-event.vector.y, event.vector.x, event.vector.z);
#ifdef ENABLE_SENSOR_LOGGER
								MOJING_TRACE(g_Sensorlogger, "time = " << event.timestamp / 1000000 <<
									", Mag(" << theSample.MagneticField.x << ", " << theSample.MagneticField.y << ", " <<
									theSample.MagneticField.z << ")");
#endif
								// Phone values are in micro-Tesla. Convert it to Gauss and flip axes.
								theSample.MagneticField *= 10000.0f / 1000000.0f;
							}
						}
					}
				}
			}

			DeleteSensor();
			MOJING_TRACE(g_Sensorlogger, "Exit MojingSenser " << myCount);
			return 0;
		}

		void AndroidInternalSensor::OnSensorData(MessageBodyFrame& sensorFrame)
		{
			if (m_pHandler)
			{
				// 设置温度
				if ((sensorFrame.LastSampleTime - sensorFrame.lastTempTime) > 0.2f)
				{
					// 200ms获取温度一次
					float Temp = getTemperature();
					if (Temp > 0) 
						sensorFrame.Temperature = Temp;
					sensorFrame.lastTempTime = sensorFrame.LastSampleTime;
#ifdef ENABLE_SENSOR_LOGGER
					MOJING_TRACE(g_Sensorlogger, "SensorDevice - Temperature = "<< Temp);
#endif
				}
				
				m_pHandler->OnSensorData(sensorFrame);
			}
		}
	}
}
