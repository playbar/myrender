#include <stdio.h>
#include "../MojingManager.h"
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

#include<arpa/inet.h>
#include<sys/socket.h>

#include <android/log.h>

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_Sensorlogger;
#endif

#define PORT 16168   //The port on which to listen for incoming data

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
		static int sem_timedwait_millsecs(sem_t *sem, long msecs)
		{
			struct timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);
			long secs = msecs / 1000;
			msecs = msecs % 1000;

			long add = 0;
			msecs = msecs * 1000 * 1000 + ts.tv_nsec;
			add = msecs / (1000 * 1000 * 1000);
			ts.tv_sec += (add + secs);
			ts.tv_nsec = msecs % (1000 * 1000 * 1000);

			return sem_timedwait(sem, &ts);
		}

		static int64_t getTimeNsec() {
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC, &now);
			return (int64_t) now.tv_sec*1000000000LL + now.tv_nsec;
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

		void GlassSensor::ReceiveSocketData(MojingSensorParameters * pSensorParameters)
		{
			MOJING_FUNC_TRACE(g_Sensorlogger);

			SocketSensorData *mmap_ptr = NULL;
			SocketSensorData sdata;

			int s = -1;
			struct sockaddr_in si_other;
			socklen_t slen = sizeof(si_other), recv_len;

			if (pSensorParameters)
			{
				s = pSensorParameters->GetSocket();
				if (s != -1)
				{
					mmap_ptr = &sdata;
				}
				else
				{
					__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "socket not init");
				}
			}

			while (!GetExitFlag())
			{
				if (mmap_ptr != NULL && s != -1 && (recv_len = recvfrom(s, mmap_ptr, sizeof(SocketSensorData), 0, (struct sockaddr *)&si_other, &slen)) != -1)
				{
					//__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "sensor data time %f delta %f", mmap_ptr->data.TimeS, mmap_ptr->data.TimeDelta);

					bool isLowPower = mmap_ptr->data.Flags & SENSOR_LOW_POWER_BIT;
					if (pSensorParameters && pSensorParameters->GetIsLowPower() != isLowPower)
					{
						pSensorParameters->SetIsLowPower(isLowPower);
					}

					if (!GetExitFlag() && mmap_ptr->data.TimeDelta > 0.002f)
					{
						Quatf fOrientation;
						Vector3f fAngularAcceleration;
						Vector3f fLinearAcceleration;

						fOrientation.x = mmap_ptr->data.Orientation_X;
						fOrientation.y = mmap_ptr->data.Orientation_Y;
						fOrientation.z = mmap_ptr->data.Orientation_Z;
						fOrientation.w = mmap_ptr->data.Orientation_W;

						fAngularAcceleration.x = mmap_ptr->data.AngularAccel_X;
						fAngularAcceleration.y = mmap_ptr->data.AngularAccel_Y;
						fAngularAcceleration.z = mmap_ptr->data.AngularAccel_Z;

						fLinearAcceleration.x = mmap_ptr->data.LinearAccel_X;
						fLinearAcceleration.y = mmap_ptr->data.LinearAccel_Y;
						fLinearAcceleration.z = mmap_ptr->data.LinearAccel_Z;

#ifdef ENABLE_SENSOR_LOGGER
						__android_log_print(ANDROID_LOG_DEBUG,
							"JNIMsg",
							"Sensor data from service. Orientation (%f / %f / %f / %f), AngularAcceleration(%f / %f / %f), LinearAcceleration(%f / %f / %f), LinearAcceleration: %f\n", \
							fOrientation.x, fOrientation.y, fOrientation.z, fOrientation.w, mmap_ptr->data.Temperature, \
							fAngularAcceleration.x, fAngularAcceleration.y, fAngularAcceleration.z, \
							fLinearAcceleration.x, fLinearAcceleration.y, fLinearAcceleration.z);
#endif
						OnSensorData(fOrientation, fAngularAcceleration, fLinearAcceleration, mmap_ptr->data.TimeS, mmap_ptr->data.Temperature);
					}
				}
			}

			//close(s);
		}

		// mmap
		void GlassSensor::ReadMMapData(MojingSensorParameters * pSensorParameters)
		{
			MOJING_FUNC_TRACE(g_Sensorlogger);
			int mmap_fd;
			MMapedSensorData *mmap_ptr = NULL;
			while (!GetExitFlag())
			{
				if (mmap_ptr == NULL)
				{
					mkdir("/sdcard/MojingSDK", 0777);
					mkdir("/sdcard/MojingSDK/mjsensor", 0777);

					char mmapRefPath[256] = { 0 };
					sprintf(mmapRefPath, "/sdcard/MojingSDK/mjsensor/%s", MojingPlatformBase::GetPlatform()->GetPackageName());

					//__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "fetch mmap %s\n", (const char*)&mmapRefPath);

					if ((mmap_fd = open((const char*)&mmapRefPath, O_CREAT | O_RDWR)) < 0)
					{
#ifdef _DEBUG
						__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "Open error(%d). path=%s\n", errno, (const char*)&mmapRefPath);
						MOJING_TRACE(g_Sensorlogger, "Open error(" << errno << "). path=" << (const char*)&mmapRefPath);
#endif
						sleep(1);
					}
					else
					{
						ftruncate(mmap_fd, sizeof(MMapedSensorData));
						if ((mmap_ptr = (MMapedSensorData *)mmap(NULL, sizeof(MMapedSensorData), PROT_READ | PROT_WRITE, MAP_SHARED, mmap_fd, 0)) == (void *)-1)
						{
#ifdef _DEBUG
							__android_log_print(ANDROID_LOG_WARN, "JNIMsg", "mmap error\n");
#endif
							mmap_ptr = NULL;
							sleep(1);
						}

						close(mmap_fd);
					}
				}

				// replace trywait with timedwait to reduce CPU usage
				if (mmap_ptr != NULL && 0 == sem_timedwait_millsecs(&mmap_ptr->semlock, 100))
				{
					//__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "sensor data time %f delta %f", mmap_ptr->data.TimeS, mmap_ptr->data.TimeDelta);

					bool isLowPower = mmap_ptr->data.Flags & SENSOR_LOW_POWER_BIT;
					if (pSensorParameters && pSensorParameters->GetIsLowPower() != isLowPower)
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

					if (!GetExitFlag() && mmap_ptr->data.TimeDelta > 0.002f)
					{
						Quatf fOrientation;
						Vector3f fAngularAcceleration;
						Vector3f fLinearAcceleration;

						fOrientation.x = mmap_ptr->data.Orientation_X;
						fOrientation.y = mmap_ptr->data.Orientation_Y;
						fOrientation.z = mmap_ptr->data.Orientation_Z;
						fOrientation.w = mmap_ptr->data.Orientation_W;

						fAngularAcceleration.x = mmap_ptr->data.AngularAccel_X;
						fAngularAcceleration.y = mmap_ptr->data.AngularAccel_Y;
						fAngularAcceleration.z = mmap_ptr->data.AngularAccel_Z;

						fLinearAcceleration.x = mmap_ptr->data.LinearAccel_X;
						fLinearAcceleration.y = mmap_ptr->data.LinearAccel_Y;
						fLinearAcceleration.z = mmap_ptr->data.LinearAccel_Z;

#ifdef ENABLE_SENSOR_LOGGER
						__android_log_print(ANDROID_LOG_DEBUG,
							"JNIMsg",
							"Sensor data from service. Orientation (%f / %f / %f / %f), AngularAcceleration(%f / %f / %f), LinearAcceleration(%f / %f / %f), LinearAcceleration: %f\n", \
							fOrientation.x, fOrientation.y, fOrientation.z, fOrientation.w, mmap_ptr->data.Temperature, \
							fAngularAcceleration.x, fAngularAcceleration.y, fAngularAcceleration.z, \
							fLinearAcceleration.x, fLinearAcceleration.y, fLinearAcceleration.z);
#endif
						OnSensorData(fOrientation, fAngularAcceleration, fLinearAcceleration, mmap_ptr->data.TimeS, mmap_ptr->data.Temperature);
					}
				}
			}
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
			__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "Start GlassSensor %d", myCount);

			MojingSensorParameters * pSensorParameters = NULL;
			Manager *pManager = Manager::GetMojingManager();
			if (pManager)
			{
				pSensorParameters = pManager->GetParameters()->GetSensorParameters();
			}

			if (strcmp(pManager->GetParameters()->GetDeviceParameters()->GetModel(), "KE-01") == 0) //KE-01分体机时采用socket模式传输
			{
				ReceiveSocketData(pSensorParameters);
			}
			else //其他采用mmap模式传输
			{
				ReadMMapData(pSensorParameters);
			}

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

		void GlassSensor::OnSensorData(Quatf fOrientation, Vector3f fAngularAcceleration, Vector3f fLinearAcceleration, double dTimeInSeconds, float fTemperature)
		{
			if (m_pHandler)
			{
				m_pHandler->OnSensorData(fOrientation, fAngularAcceleration, fLinearAcceleration, dTimeInSeconds, fTemperature);
			}
		}
	}
}
