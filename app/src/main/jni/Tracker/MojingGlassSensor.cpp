#include <stdio.h>
#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"
#include "../Base/MojingTypes.h"
#include "../Base/MojingTimer.h"
#include "../Base/MojingLog.h"
#include "../Platform/MojingPlatformBase.h"
//#include "../MojingSDKStatus.h"
#include "MojingTemperature.h"
#ifdef MJ_OS_ANDROID
#include "MojingGlassSensor.h"
#endif

#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <sys/stat.h>


#include <android/log.h>

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
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

#define SENSOR_LOW_POWER_BIT 0x01000000

//extern JavaVM* gJavaVM;
//extern jobject gobject_MojingSDKServiceManager;

namespace Baofeng
{
	namespace Mojing
	{
		void tohex(unsigned char * in, size_t insz, char * out, size_t outsz)
		{
		    unsigned char * pin = in;
		    const char * hex = "0123456789ABCDEF";
		    char * pout = out;
		    for(; pin < in+insz; pout +=3, pin++){
		        pout[0] = hex[(*pin>>4) & 0xF];
		        pout[1] = hex[ *pin     & 0xF];
		        pout[2] = ':';
		        if ((size_t)(pout + 3 - out) > outsz){
		            /* Better to truncate output string than overflow buffer */
		            /* it would be still better to either return a status */
		            /* or ensure the target buffer is large enough and it never happen */
		            break;
		        }
		    }
		    pout[-1] = 0;
		}

		int sem_timedwait_millsecs(sem_t *sem, long msecs)
		{
		    struct timespec ts;
		    clock_gettime(CLOCK_REALTIME, &ts);
		    long secs = msecs/1000;
		    msecs = msecs%1000;

		    long add = 0;
		    msecs = msecs*1000*1000 + ts.tv_nsec;
		    add = msecs / (1000*1000*1000);
		    ts.tv_sec += (add + secs);
		    ts.tv_nsec = msecs%(1000*1000*1000);

		    return sem_timedwait(sem, &ts);
		}

#define US_PER_SAMPLE(frequence)	(1000L * 1000 / (frequence))

		GlassSensor::GlassSensor()
		{
			//MOJING_FUNC_TRACE(g_Sensorlogger);
		}

		GlassSensor::~GlassSensor()
		{
			// In any case user forgot stop sensor......
			StopSensor();
		}

		int GlassSensor::CheckSensors()const
		{
			int nSensorState = SENSOR_OK;

			return nSensorState;
		}

		int GlassSensor::GetMaxSampleRate()
		{
			if (m_iMaxSampleRate == 0)
			{

			}
			return m_iMaxSampleRate;
		}

		void GlassSensor::CreateSensor()
		{
			Manager *pManager = Manager::GetMojingManager();
			MojingSensorParameters * pSensorParameters = NULL;
			if (pManager)
			{
				pSensorParameters = pManager->GetParameters()->GetSensorParameters();
			}
			
//			ASensorManager* pSensorManager = ASensorManager_getInstance();
//			looper = ALooper_forThread();
//			if (looper == NULL)
//			{
//				looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
//			}
//			m_pQueue = ASensorManager_createEventQueue(pSensorManager, looper, LOOPER_ID, NULL, NULL);
//
//			pMagSensorUncalibrated = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED);
//			pMagneticFieldSensor = NULL;
//
//			int miniDelay = 0;
//			int wantDelay = US_PER_SAMPLE(m_nSampleFrequence);// Hz ==> 10^-6(微秒)
//			MOJING_TRACE(g_Sensorlogger, "Wanted Sensor's rate = " << m_nSampleFrequence);
//#define MICRO_SECOND 1000000L
//			if (pMagSensorUncalibrated != NULL)
//			{
//				miniDelay = ASensor_getMinDelay(pMagSensorUncalibrated); //微秒/次
//				miniDelay = miniDelay > wantDelay ? miniDelay : wantDelay; //如果值<miniDelay 将出错, 我们希望最大速率
//				ASensorEventQueue_enableSensor(m_pQueue, pMagSensorUncalibrated);
//				ASensorEventQueue_setEventRate(m_pQueue, pMagSensorUncalibrated, miniDelay);
//				MOJING_TRACE(g_Sensorlogger, "MagUncalibrated Sensor's rate = " << MICRO_SECOND / miniDelay);
//				if (pSensorParameters)
//					pSensorParameters->SetMagSensorUncalibrated(MICRO_SECOND / miniDelay);
//			}
//			else if (pMagneticFieldSensor != NULL)
//			{
//				// If the phone do not support
//				miniDelay = ASensor_getMinDelay(pMagneticFieldSensor); //微秒/次
//				miniDelay = miniDelay > wantDelay ? miniDelay : wantDelay;
//				pMagneticFieldSensor = ASensorManager_getDefaultSensor(pSensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
//				ASensorEventQueue_enableSensor(m_pQueue, pMagneticFieldSensor);
//				ASensorEventQueue_setEventRate(m_pQueue, pMagneticFieldSensor, miniDelay);
//				MOJING_TRACE(g_Sensorlogger, "MagneticField Sensor's rate = " << MICRO_SECOND / miniDelay);
//				if (pSensorParameters)
//					pSensorParameters->SetMagneticFieldSensor (MICRO_SECOND / miniDelay);
//			}
//			else
//			{
//#ifdef MJ_OS_ANDROID
//				MOJING_WARN(g_Sensorlogger, "This phone do not have MagneticField Sensor.");
//#endif
//			}
		}

#define DELETE_SENSOR(pQueue, pSensor) if (pSensor != NULL) \
		{ \
			ASensorEventQueue_disableSensor(m_pQueue, pSensor);	\
			pSensor = NULL; \
		}
		
		void GlassSensor::DeleteSensor()
		{
//			DELETE_SENSOR(m_pQueue, pMagSensorUncalibrated);
//			DELETE_SENSOR(m_pQueue, pMagneticFieldSensor);
//			if (m_pQueue != NULL)
//			{
//				ASensorManager* pSensorManager = ASensorManager_getInstance();
//				ASensorManager_destroyEventQueue(pSensorManager, m_pQueue);
//				m_pQueue = NULL;
//			}
//			//ALooper_release(looper);	// TBD: need or not need
//			looper = NULL;
		}

		int g_GlassSensorCount = 0;
		int GlassSensor::Run()
		{
			MOJING_FUNC_TRACE(g_Sensorlogger);

			char buf[64];
			int myCount = g_GlassSensorCount++;
			sprintf(buf, "GlassSensor_%d", myCount);

			SetThreadName(buf);
			MOJING_TRACE(g_Sensorlogger, "Start GlassSensor " << myCount);

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
			if (theSample.Temperature < 0) theSample.Temperature = 2500;	// ��������õ��¶�ֵ�������ó�25��

//			__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "Start Glass Sensor %d \n", myCount);

			// for android sensor
			int ident = 0;
			int fd = 0;
			int events = 0;

			// mmap
			int mmap_fd;
			MMapedSensorData *mmap_ptr = NULL;

			enum {insz = 32, outsz = 3*insz};
			char str[outsz];

			uint32_t lastTimer = 0;
			uint32_t timeMS;
			uint32_t deltaTimer;

			while (!GetExitFlag()) {

				if (mmap_ptr == NULL)
				{
					mkdir("/sdcard/MojingSDK", 0777);
					mkdir("/sdcard/MojingSDK/mjsensor", 0777);

					char mmapRefPath[256] = {0};
					sprintf(mmapRefPath, "/sdcard/MojingSDK/mjsensor/%s", MojingPlatformBase::GetPlatform()->GetPackageName());

//					__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "fetch mmap %s\n", (const char*)&mmapRefPath);

					if ((mmap_fd = open((const char*)&mmapRefPath, O_CREAT | O_RDWR)) < 0)
					{
						__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "Open error(%d). path=%s\n", errno, (const char*)&mmapRefPath);
						MOJING_TRACE(g_Sensorlogger, "Open error(" << errno << "). path=" << (const char*)&mmapRefPath);
						sleep(1);
					}
					else
					{
						ftruncate(mmap_fd, sizeof(MMapedSensorData));

						if ((mmap_ptr = (MMapedSensorData *)mmap(NULL, sizeof(MMapedSensorData), PROT_READ | PROT_WRITE, MAP_SHARED, mmap_fd, 0)) == (void *)-1)
						{
							__android_log_print(ANDROID_LOG_WARN, "JNIMsg", "mmap error\n");
							mmap_ptr = NULL;
							sleep(1);
						}

						close(mmap_fd);
					}
				}

				// replace trywait with timedwait to reduce CPU usage
				if (mmap_ptr != NULL && 0 == sem_timedwait_millsecs(&mmap_ptr->semlock, 100))
				{
					timeMS = mmap_ptr->data.TimeMS;
//					timeMS = Timer::GetTicksMs();

					deltaTimer = timeMS - lastTimer;
					lastTimer = timeMS;

//					MOJING_TRACE(g_Sensorlogger, "MojingSensor cnt=" << mmap_ptr->cnt << " time=" << timeMS << " delta=" << deltaTimer);

//					tohex((unsigned char*)&(mmap_ptr->data), insz, str, outsz);
//					__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Mojing semlock=%d cnt=%d time=%d delta=%d\n", mmap_ptr->semlock, mmap_ptr->cnt, timeMS, deltaTimer);
//					__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "   READ: received data: %s\n", str);

					// Acceleration
//					theSample.Acceleration.x = ((float)((int16_t)((mmap_ptr->data.Accl_XMSB << 8) | mmap_ptr->data.Accl_XLSB))) / 16384 * 9.81;
//					theSample.Acceleration.y = -((float)((int16_t)((mmap_ptr->data.Accl_YMSB << 8) | mmap_ptr->data.Accl_YLSB))) / 16384 * 9.81;
//					theSample.Acceleration.z = -((float)((int16_t)((mmap_ptr->data.Accl_ZMSB << 8) | mmap_ptr->data.Accl_ZLSB))) / 16384 * 9.81;
					theSample.Acceleration.x = mmap_ptr->data.Accl_X;
					theSample.Acceleration.y = mmap_ptr->data.Accl_Y;
					theSample.Acceleration.z = mmap_ptr->data.Accl_Z;

#ifdef ENABLE_SENSOR_LOGGER
					MOJING_TRACE(g_Sensorlogger, "time = " << timeMS <<
						", Accel(" << theSample.Acceleration.x << ", " << theSample.Acceleration.y <<
						", " << theSample.Acceleration.z << ")");
#endif
//					__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "time = %d, Accel(%f, %f, %f)", timeMS, theSample.Acceleration.x, theSample.Acceleration.y, theSample.Acceleration.z);

					// Gyroscope
//					theSample.RotationRate.x = DegreeToRad(((float)((int16_t)((mmap_ptr->data.Gryo_XMSB << 8) | mmap_ptr->data.Gryo_XLSB))) / 131);
//					theSample.RotationRate.y = -DegreeToRad(((float)((int16_t)((mmap_ptr->data.Gryo_YMSB << 8) | mmap_ptr->data.Gryo_YLSB))) / 131);
//					theSample.RotationRate.z = -DegreeToRad(((float)((int16_t)((mmap_ptr->data.Gryo_ZMSB << 8) | mmap_ptr->data.Gryo_ZLSB))) / 131);
					theSample.RotationRate.x = mmap_ptr->data.Gryo_X;
					theSample.RotationRate.y = mmap_ptr->data.Gryo_Y;
					theSample.RotationRate.z = mmap_ptr->data.Gryo_Z;
					theSample.TimeDelta = timeMS / 1000.0 - theSample.LastSampleTime;
					theSample.LastSampleTime = timeMS / 1000.0;
					theSample.AbsoluteTimeSeconds = Timer::GetSeconds();
#ifdef ENABLE_SENSOR_LOGGER
					MOJING_TRACE(g_Sensorlogger, "time = " << timeMS <<
						", Gyro(" << theSample.RotationRate.x << ", " << theSample.RotationRate.y << ", " <<
						theSample.RotationRate.z << ")" << " TimeDelta=" << theSample.TimeDelta);
#endif
//					__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "time = %d, Gyro(%f, %f, %f) TimeDelta=%f", timeMS, theSample.RotationRate.x, theSample.RotationRate.y, theSample.RotationRate.z, theSample.TimeDelta);

					if (ui64FirstSampleTime == 0)
					{
						ui64Last50FirstSampleTime = ui64FirstSampleTime = timeMS;
						ui64SampleCount = 1;
						fMinTimeSpace = fMaxTimeSpace = theSample.TimeDelta;
					}
					else
					{
						ui64SampleCount++;
						if (ui64SampleCount)// 防溢出处理
							fAvgTimeSpace = (timeMS - ui64FirstSampleTime) / 1000.0 / ui64SampleCount;
						fMinTimeSpace = std::min(fMinTimeSpace, theSample.TimeDelta);
						fMaxTimeSpace = std::max(fMaxTimeSpace, theSample.TimeDelta);
						if (ui64SampleCount % 50 == 0)
						{
							float fLast50AvgTimeSpace = (timeMS - ui64Last50FirstSampleTime) / 1000.0 / 50;
							pSensorParameters->SetAvgSampleRate(1 / fAvgTimeSpace);
							pSensorParameters->SetMaxSampleRate(1 / fMinTimeSpace);
							pSensorParameters->SetMinSampleRate(1 / fMaxTimeSpace);
							pSensorParameters->SetLast50AvgSampleRate(1 / fLast50AvgTimeSpace);

							ui64Last50FirstSampleTime = timeMS;
						}
					}

					// Magnetic
//					theSample.MagneticField.Set(
//							((float)((int16_t)((mmap_ptr->data.Mag_XMSB << 8) | mmap_ptr->data.Mag_XLSB))) * 0.05,
//							-((float)((int16_t)((mmap_ptr->data.Mag_YMSB << 8) | mmap_ptr->data.Mag_YLSB))) * 0.05,
//							-((float)((int16_t)((mmap_ptr->data.Mag_ZMSB << 8) | mmap_ptr->data.Mag_ZLSB))) * 0.5);
					theSample.MagneticField.Set(mmap_ptr->data.Mag_X, mmap_ptr->data.Mag_Y, mmap_ptr->data.Mag_Z);
#ifdef ENABLE_SENSOR_LOGGER
					MOJING_TRACE(g_Sensorlogger, "time = " << timeMS <<
						", Mag(" << theSample.MagneticField.x << ", " << theSample.MagneticField.y << ", " <<
						theSample.MagneticField.z << ")");
#endif
//					__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "time = %d, Mag(%f, %f, %f)", timeMS, theSample.MagneticField.x, theSample.MagneticField.y, theSample.MagneticField.z);
					// Phone values are in micro-Tesla. Convert it to Gauss and flip axes.
					theSample.MagneticField *= 10000.0f / 1000000.0f;

					// Temperature
//					theSample.Temperature = ((float)((int16_t)((mmap_ptr->data.Temperature_MSB << 8) | mmap_ptr->data.Temperature_LSB))) / 326.8 + 25;
					theSample.Temperature = mmap_ptr->data.Temperature;
#ifdef ENABLE_SENSOR_LOGGER
					MOJING_TRACE(g_Sensorlogger, "SensorDevice - Temperature = " << theSample.Temperature);
#endif
//					__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Temperature: %f\n", theSample.Temperature);

//					while ((ident = ALooper_pollAll(3, &fd, &events, NULL) >= 0) && !GetExitFlag())
//					{
//						if (ident == LOOPER_ID)
//						{
//							ASensorEvent event;
//							while (ASensorEventQueue_getEvents(m_pQueue, &event, 1) > 0)
//							{
//								if (event.type == ASENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED)
//								{
//									theSample.MagneticField.Set(-event.uncalibrated_magnetic.y_uncalib,
//										event.uncalibrated_magnetic.x_uncalib,
//										event.uncalibrated_magnetic.z_uncalib);
//
//									theSample.MagneticBias.Set(-event.uncalibrated_magnetic.y_bias,
//										event.uncalibrated_magnetic.x_bias,
//										event.uncalibrated_magnetic.z_bias);
//
//	#ifdef ENABLE_SENSOR_LOGGER
//									MOJING_TRACE(g_Sensorlogger, "time = " << event.timestamp / 1000000 <<
//										", MagUncal(" << theSample.MagneticField.x << ", " << theSample.MagneticField.y << ", " <<
//										theSample.MagneticField.z << ")");
//	#endif
//									__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "time = %d, MagUncal(%f, %f, %f)", event.timestamp / 1000000, theSample.MagneticField.x, theSample.MagneticField.y, theSample.MagneticField.z);
//
//									// Phone values are in micro-Tesla. Convert it to Gauss and flip axes.
//									theSample.MagneticField *= 10000.0f / 1000000.0f;
//									theSample.MagneticBias *= 10000.0f / 1000000.0f;
//								}
//								else if (event.type == ASENSOR_TYPE_MAGNETIC_FIELD)
//								{
//									theSample.MagneticField.Set(-event.vector.y, event.vector.x, event.vector.z);
//
//	#ifdef ENABLE_SENSOR_LOGGER
//									MOJING_TRACE(g_Sensorlogger, "time = " << event.timestamp / 1000000 <<
//										", Mag(" << theSample.MagneticField.x << ", " << theSample.MagneticField.y << ", " <<
//										theSample.MagneticField.z << ")");
//	#endif
//									__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "time = %d, Mag(%f, %f, %f)", event.timestamp / 1000000, theSample.MagneticField.x, theSample.MagneticField.y, theSample.MagneticField.z);
//
//									// Phone values are in micro-Tesla. Convert it to Gauss and flip axes.
//									theSample.MagneticField *= 10000.0f / 1000000.0f;
//								}
//							}
//						}
//					}

					bool isLowPower = mmap_ptr->data.Flags & SENSOR_LOW_POWER_BIT;
					if (pSensorParameters->GetIsLowPower() != isLowPower)
					{
						pSensorParameters->SetIsLowPower(isLowPower);

//						MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
//						if (pStatus->GetEngineStatus() != ENGINE_UNREAL)
//						{
//							JNIEnv *env;
//							gJavaVM->AttachCurrentThread(&env, NULL);
//							clazz_MojingSDKServiceManager = (env)->GetObjectClass(gobject_MojingSDKServiceManager);
//							mid_onLowPowerStateChanged = (env)->GetStaticMethodID(clazz_MojingSDKServiceManager, "onLowPowerStateChanged", "(Z)V");
//							(env)->CallStaticObjectMethod(clazz_MojingSDKServiceManager, mid_onLowPowerStateChanged, isLowPower);
//							gJavaVM->DetachCurrentThread();
//						}
					}

					if (!GetExitFlag() && theSample.TimeDelta > 0.002f)
						OnSensorData(theSample);
				}

			}

//			__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "Exit Glass Sensor %d\n", myCount);

			DeleteSensor();
			MOJING_TRACE(g_Sensorlogger, "Exit GlassSensor " << myCount);
			return 0;
		}

		void GlassSensor::OnSensorData(MessageBodyFrame& sensorFrame)
		{
			if (m_pHandler)
			{
				m_pHandler->OnSensorData(sensorFrame);
			}
		}
	}
}
