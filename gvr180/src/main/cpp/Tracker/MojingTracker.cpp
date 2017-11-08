#include <unistd.h>
#ifdef MJ_OS_ANDROID
#include <android/sensor.h>
#include <android/looper.h>
#endif

#include <stdio.h>
#include "../Base/MojingTimer.h"
#include "../Base/MojingLog.h"
#include "../Base/MojingThreads.h"
#include "../MojingManager.h"
#include "../MojingSDKStatus.h"
#include "MojingTracker.h"
#include "MojingTemperature.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
MojingLogger g_Sensorlogger("MojingSensor");
extern MojingLogger g_APIlogger;
#endif

#define NanoSecondToSecond(nanoSecond) ((nanoSecond) / 1000000000.0)

namespace Baofeng
{
	namespace Mojing
	{
		Tracker::Tracker()
		: RefCount(0)
		{
			m_nSkipSamples = 10;
			m_bSensorfusionInit = false;
			SetDataFromExternal(false);
			m_pSensorfusion = new SensorFusion();
		}

		Tracker::~Tracker()
		{
			StopTrack();
			delete m_pSensorfusion;
		}
		int Tracker::CheckSensors()const
		{
			Manager* pManager = Manager::GetMojingManager();
			if (pManager)
			{
				Sensor* pSensor = pManager->GetSensor();
				if (pSensor)
				{
					return pSensor->CheckSensors();
				}
			}
			return -1;
		}
		int Tracker::GetMaxSensorsSampleRate()
		{
			Manager* pManager = Manager::GetMojingManager();
			if (pManager)
			{
				Sensor* pSensor = pManager->GetSensor();
				if (pSensor)
					return pSensor->GetMaxSampleRate();
			}
			return -1;
		}
		String Tracker::GetCheckSensorString()const
		{
			String strRet;
			int iCheckSensor = CheckSensors();
			if (iCheckSensor == 0)
				strRet = "OK";
			else
			{
				if (iCheckSensor & SENSOR_NoMag)
					strRet += "SENSOR_NoMag;";
				if (iCheckSensor & SENSOR_NoGryo)
					strRet += "SENSOR_NoGryo;";
				if (iCheckSensor & SENSOR_GryoTooSlow)
					strRet += "SENSOR_GryoTooSlow;";
				if (iCheckSensor & SENSOR_NoMag)
					strRet += "SENSOR_NoAccel;";
				if (iCheckSensor & SENSOR_NoAccel)
					strRet += "SENSOR_AccelTooSlow;";
				if (iCheckSensor & SENSOR_AccelTooSlow)
					strRet += "SENSOR_NoMag;";

			}
			return strRet;
		}

		Quatf   Tracker::CalculateTimeWarpMatrix(Quatf inFrom, Quatf inTo)
		{
			bool fromValid = inFrom.LengthSq() > 0.95f;
			bool toValid = inTo.LengthSq() > 0.95f;
			if (!fromValid)
			{
				MOJING_ERROR(g_APIlogger, "Invalid Srouce view rotate");
				if (toValid)
				{
					inFrom = inTo;
				}
				else
				{
					inFrom = Quatf(0.0f, 0.0f, 0.0f, 1.0f); // just force identity
				}
			}
			if (!toValid)
			{
				MOJING_ERROR(g_APIlogger, "Invalid Targer view rotate");
				if (fromValid)
				{
					inTo = inFrom;
				}
				else
				{
					inTo = Quatf(0.0f, 0.0f, 0.0f, 1.0f); // just force identity
				}
			}
			/************************************************************************
			 注意：                                                               
				下面的代码如果写成
					(inFrom * inTo.Inverted()).Inverted(),那么预测的图像方向将与运动方
					向一致,这就导致了运动时的卡顿。				
			/************************************************************************/
			// 20170731 
			Quatf qFix = (inFrom * inTo.Inverted())/*.Inverted()*/;
			// qFix.z *= -1;
			return qFix;
		}
		Matrix4f Tracker::CalculateTimeWarpMatrix2( Quatf inFrom, Quatf inTo)
		{
			// FIXME: this is a horrible hack to fix a zero quaternion that's passed in
			
			Matrix4f mRet = Matrix4f(CalculateTimeWarpMatrix(inFrom , inTo));
			return mRet;
// 
// 			Matrix4f		lastSensorMatrix_To = Matrix4f(inTo);
// 			Matrix4f		lastViewMatrix_From = Matrix4f(inFrom);
// 
//			return (lastSensorMatrix_To.Inverted() * lastViewMatrix_From); // .Inverted();
		}

		void Tracker::SetYaw()
		{
			m_pSensorfusion->SetYawCorrectionEnabled(true);
			// restore the yaw from the last sensor state when we re-connect to a sensor
			float yaw, pitch, roll;
			m_pLastSensorState.Predicted.Transform.Orientation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z >(&yaw, &pitch, &roll);
			m_pSensorfusion->SetYaw(yaw);
		}

		bool Tracker::StartTrack(int nSampleFrequence/*, const char *pRecordPath*/, const char * lpszModel /* = NULL*/, const char* szGlassName)
		{
			MOJING_FUNC_TRACE(g_Sensorlogger);
			MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
			if (AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, 1) >= 1)
			{
				MOJING_TRACE(g_Sensorlogger, "Start Tracker do not match! Start already. ");
				pStatus->SetTrackerStatus(TRACKER_START);
			}
			else
			{
				pStatus->SetTrackerStatus(TRACKER_START_NOW);
				MOJING_TRACE(g_Sensorlogger, "Set sensor frequence as " << nSampleFrequence << " / " << GetMaxSensorsSampleRate() << "(Max)");
				Manager* pManager = Manager::GetMojingManager();
				pManager->SetSensor(szGlassName);
				Sensor* pSensor = pManager->GetSensor();
				int nCheck = pSensor->CheckSensors();

				if (nCheck & (SENSOR_GryoTooSlow | SENSOR_NoGryo))
				{
					pStatus->SetTrackerStatus(TRACKER_STOP);
					MOJING_ERROR(g_APIlogger, "GryoTooSlow or SENSOR_NoGryo , CheckSensors = " << nCheck);
					AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, -1);
					return false;
				}
				Parameters* pParameters = pManager->GetParameters();

				m_pSensorfusion->Initialize(pSensor, pParameters, lpszModel);
				if (GetDataFromExternal() && szGlassName == NULL)
				{
#ifdef MJ_OS_ANDROID
					pStatus->SetSensorOrigin(SENSOR_FROM_JAVA);
#endif
					MOJING_TRACE(g_Sensorlogger, "Use sensor data from SDK_Java.");
					m_bSensorfusionInit = true;
					m_nSkipSamples = 10;
					memset(&m_sensorFrame, 0, sizeof(MessageBodyFrame));
					m_sensorFrame.LastSampleTime = m_sensorFrame.lastTempTime = NanoSecondToSecond(Timer::GetTicksNanos());
					m_sensorFrame.Temperature = getTemperature();
					if (m_sensorFrame.Temperature < 0) m_sensorFrame.Temperature = 2500;	// 如果不能拿到温度值，则设置成25度
				}
				else
				{
#ifdef MJ_OS_ANDROID
					if (szGlassName)
					{
						pStatus->SetSensorOrigin(SENSOR_FROM_SERVICE);
						MOJING_TRACE(g_Sensorlogger, "Use sensor data from Service. Name: " << szGlassName );
					}
					else
					{
						pStatus->SetSensorOrigin(SENSOR_FROM_NATIVE);
						MOJING_TRACE(g_Sensorlogger, "Use sensor data from SDK_Native.");
					}
#endif
					if (!pSensor->StartSensor(nSampleFrequence/*, pRecordPath*/))
					{
						pStatus->SetTrackerStatus(TRACKER_STOP);
						MOJING_ERROR(g_APIlogger, "pSensor->StartSensor failed");
						AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, -1);
						return false;
					}
					int iCount = 0;
					MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
					if (pStatus->GetEngineStatus() != ENGINE_UNREAL)
					{	// 特殊处理：在进行等待第一个Sample的等待的时候，Unreal 4.11以及4.12
						// 会陷入闪退。疑似等待时间过长导致。暂时屏蔽掉这里的代码，详细原因待查。
						while (!m_pSensorfusion->m_bHasData && iCount++ < 1000)
						{// 5ms
							usleep(5000);
						}
					}
					if (iCount > 1000)
					{// 由于陀螺仪故障导致完全无法获取数据，这种情况下要通知调用方初始化失败 
						pSensor->StopSensor();
						pStatus->SetTrackerStatus(TRACKER_STOP);
						MOJING_ERROR(g_APIlogger, "Can not get any data in 5 Secs");
						// Sensor data failed maybe temporary so recover RefCount
						AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, -1);
						return false;
					}
					else
					{
						pStatus->SetTrackerStatus(TRACKER_START);
					}

					SetYaw();
				}
			}
			return true;
		}
#ifdef MJ_OS_ANDROID
		bool Tracker::StartTrackChecker(int nSampleFrequence /*= 100*/)
		{
			bool bRet = false;
			MOJING_FUNC_TRACE(g_Sensorlogger);

			if (AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, 1) >= 1)
			{
				MOJING_TRACE(g_Sensorlogger, "Start Tracker do not match! Start already. ");
			}
			else
			{
				Manager* pManager = Manager::GetMojingManager();
				Sensor* pSensor = pManager->GetCheckSensor();

				int nCheck = pSensor->CheckSensors();

				if (nCheck & (SENSOR_GryoTooSlow | SENSOR_NoGryo))
				{
					MOJING_ERROR(g_Sensorlogger, "GryoTooSlow | SENSOR_NoGryo");
				}

				bRet = pSensor->StartSensor(nSampleFrequence);
			}
			return bRet;
		}
#endif
		void Tracker::StopTrack()
		{
			MOJING_FUNC_TRACE(g_Sensorlogger);
			MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
			if (pStatus->GetTrackerStatus() != TRACKER_START)
			{
#ifdef MJ_OS_ANDROID
				if (pStatus->GetTrackerStatus() == TRACKER_START_NOW && pStatus->GetSensorOrigin() == SENSOR_FROM_JAVA)
				{
					//陀螺仪来源是SDK_JAVA， 可能由于数据未收到而未将状态置为TRACKER_START或TRACKER_STOP， 此时仍因运行继续继续Stop操作
				}
				else
#endif
				{
					return;
				}
			}
			int count = AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, -1);
			if (count < 0)// ==0表示已经退出过了，不要在下面+1
			{
				m_bSensorfusionInit = false;
				AtomicOps<int>::ExchangeAdd_NoSync(&RefCount, 1);
				MOJING_TRACE(g_Sensorlogger, "Stop Tracker without Start Tracker first! ");
				pStatus->SetTrackerStatus(TRACKER_STOP);
			}
			else if (count == 1)
			{
				// It really need stop
				m_bSensorfusionInit = false;
				pStatus->SetTrackerStatus(TRACKER_STOP_NOW);
				if (m_pSensorfusion)
				{ 
					Manager* pManager = Manager::GetMojingManager();
					Sensor* pSensor = pManager->GetSensor();
					pSensor->StopSensor();
					//delete m_pSensorfusion;
					//m_pSensorfusion = NULL;
				}
				pStatus->SetTrackerStatus(TRACKER_STOP);
			}
			else
			{
				MOJING_TRACE(g_Sensorlogger, "Stop Tracker mismatch. Too many Start.");
			}
		}

		void Tracker::RecenterYaw(void)
		{
			m_pSensorfusion->RecenterYaw();
		}
		
		void Tracker::Recenter()
		{
			m_pSensorfusion->Recenter();
		}

		void Tracker::ResetTracker(void)
		{
			m_pSensorfusion->Reset();
		}

		void Tracker::getLastHeadView(Matrix4f& mHeadView)
		{
			if (m_pSensorfusion)
			{
				SensorState sstate = m_pSensorfusion->GetPredictionForTime(0);
				if (m_pSensorfusion->m_bHasData)
				{
					m_pLastSensorState = sstate;
				}
			}

			// Convert sensor.Recorded to view matrix
			mHeadView = Matrix4f(m_pLastSensorState.Predicted.Transform.Orientation);
		}

		double Tracker::getLastSensorState(float* fArray)
		{
			if (m_pSensorfusion && m_pSensorfusion->m_bHasData)
			{
				SensorState sstate = m_pSensorfusion->GetPredictionForTime(0);
				fArray[0] = sstate.Predicted.Transform.Orientation.x;
				fArray[1] = sstate.Predicted.Transform.Orientation.y;
				fArray[2] = sstate.Predicted.Transform.Orientation.z;
				fArray[3] = sstate.Predicted.Transform.Orientation.w;
				fArray[4] = sstate.Predicted.AngularAcceleration.x;
				fArray[5] = sstate.Predicted.AngularAcceleration.y;
				fArray[6] = sstate.Predicted.AngularAcceleration.z;
				fArray[7] = sstate.Predicted.LinearAcceleration.x;
				fArray[8] = sstate.Predicted.LinearAcceleration.y;
				fArray[9] = sstate.Predicted.LinearAcceleration.z;
#ifdef ENABLE_SENSOR_LOGGER
				__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", \
					"getLastSensorState, Orientation (%f / %f / %f / %f), AngularAcceleration(%f / %f / %f), LinearAcceleration(%f / %f / %f)\n", \
					fArray[0], fArray[1],fArray[2], fArray[3],fArray[4], fArray[5],fArray[6], fArray[7],fArray[8],fArray[9]);	
#endif
				return sstate.Predicted.TimeInSeconds;
			}
			return 0;
		}

		Quatf Tracker::getPredictionPosition(const double& absoluteTimeSeconds , double * pdOutSensotTime /*= NULL*/)
		{
			Quatf Ret;
			if (m_pSensorfusion)
			{
				SensorState sstate = m_pSensorfusion->GetPredictionForTime(absoluteTimeSeconds, pdOutSensotTime);
				if (m_pSensorfusion->m_bHasData)
				{
					m_pLastSensorState = sstate;
				}

				//if (sensorState.Predicted.Transform.Orientation != Quatf(0, 0, 0, 1))
				{
					Ret = m_pLastSensorState.Predicted.Transform.Orientation;
				}
				//else
				//{
					// 无法预测,因为传入的时间小于当前时间
				//	Ret = sensorState.Recorded.Transform.Orientation;
				//}
			}
			// Convert sensor.Predicted to view matrix
			return Ret;
		}

#define CLAMP(x , min , max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : x))
		void Tracker::getLastHeadEulerAngles(float *yaw, float *pitch, float *roll)
		{
			SensorState sstate = m_pSensorfusion->GetPredictionForTime(0);
			if (m_pSensorfusion->m_bHasData)
			{
				m_pLastSensorState = sstate;
			}
			// m_pLastSensorState.Recorded.Transform.Orientation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(yaw, pitch, roll);
			float x = m_pLastSensorState.Predicted.Transform.Orientation.x;
			float y = m_pLastSensorState.Predicted.Transform.Orientation.y;
			float z = m_pLastSensorState.Predicted.Transform.Orientation.z;
			float w = m_pLastSensorState.Predicted.Transform.Orientation.w;

			*roll = atan2(2 * (w * z + x * y), 1 - 2 * (z * z + x * x));
			*pitch = asin(CLAMP(2 * (w * x - y * z), -1.0f, 1.0f));
			*yaw = atan2(2 * (w * y + z * x), 1 - 2 * (x * x + y * y));
		}

		void Tracker::getLastHeadQuarternion(float *x, float *y, float *z, float *w)
		{
			SensorState sstate = m_pSensorfusion->GetPredictionForTime(0);
			if (m_pSensorfusion->m_bHasData)
			{
				m_pLastSensorState = sstate;
			}
			*x = m_pLastSensorState.Predicted.Transform.Orientation.x;
			*y = m_pLastSensorState.Predicted.Transform.Orientation.y;
			*z = m_pLastSensorState.Predicted.Transform.Orientation.z;
			*w = m_pLastSensorState.Predicted.Transform.Orientation.w;
		}
		float Tracker::GetCalibrationRate()const
		{
			if (m_pSensorfusion)
				return m_pSensorfusion->GetCalibrationRate();
			return 0;
		}
		void Tracker::SetCalibrationRate(float fRate)const
		{
			if (m_pSensorfusion)
				m_pSensorfusion->SetCalibrationRate(fRate);
		}
// 		int Tracker::GetCalibrationResetCount()const
// 		{
// 			if (m_pSensorfusion)
// 				return m_pSensorfusion->GetCalibrationResetCount();
// 			return 0;
// 		}
// 		void Tracker::ResetCalibrationResetCount()
// 		{
// 			if (m_pSensorfusion)
// 				m_pSensorfusion->ResetCalibrationResetCount();
// 		}

		void Tracker::SendSensorData(float* pArray, double dLastSampleTime)
		{
			if (GetDataFromExternal())
			{
				//MOJING_TRACE(g_Sensorlogger, "Tracker::SendSensorData LastSampleTime : " << dLastSampleTime);
				if (m_pSensorfusion == NULL || m_bSensorfusionInit == false)
					return;
			
				if(m_nSkipSamples > 0)
				{
					m_nSkipSamples--;
					m_sensorFrame.LastSampleTime = m_sensorFrame.lastTempTime = NanoSecondToSecond(Timer::GetTicksNanos());
					return;
				}

				m_sensorFrame.TimeDelta = dLastSampleTime - m_sensorFrame.LastSampleTime;
				m_sensorFrame.LastSampleTime = dLastSampleTime;
				m_sensorFrame.AbsoluteTimeSeconds = Timer::GetSeconds();

				m_sensorFrame.Acceleration.x = -pArray[1];
				m_sensorFrame.Acceleration.y = pArray[0];
				m_sensorFrame.Acceleration.z = pArray[2];

				m_sensorFrame.RotationRate.x = -pArray[4];
				m_sensorFrame.RotationRate.y = pArray[3];
				m_sensorFrame.RotationRate.z = pArray[5];

				m_sensorFrame.MagneticField.x = -pArray[7];
				m_sensorFrame.MagneticField.y = pArray[6];
				m_sensorFrame.MagneticField.z = pArray[8];

				m_sensorFrame.MagneticBias.x = -pArray[10];
				m_sensorFrame.MagneticBias.y = pArray[9];
				m_sensorFrame.MagneticBias.z = pArray[11];

#ifdef ENABLE_SENSOR_LOGGER
				MOJING_TRACE(g_Sensorlogger, "Tracker::SendSensorData Acceleration.x:" << m_sensorFrame.Acceleration.x << " y:" << m_sensorFrame.Acceleration.y << " z:" << m_sensorFrame.Acceleration.z
					<< " RotationRate.x:" << m_sensorFrame.RotationRate.x << " y:" << m_sensorFrame.RotationRate.y << " z:" << m_sensorFrame.RotationRate.z
					<< " MagneticField.x:" << m_sensorFrame.MagneticField.x << " y:" << m_sensorFrame.MagneticField.y << " z:" << m_sensorFrame.MagneticField.z
					<< " MagneticBias.x:" << m_sensorFrame.MagneticBias.x << " y:" << m_sensorFrame.MagneticBias.y << " z:" << m_sensorFrame.MagneticBias.z);
#endif

				// 设置温度
				if ((m_sensorFrame.LastSampleTime - m_sensorFrame.lastTempTime) > 0.2f)
				{
					// 200ms获取温度一次
					float Temp = getTemperature();
					if (Temp > 0)
						m_sensorFrame.Temperature = Temp;
					else
						m_sensorFrame.Temperature = 2500;
					m_sensorFrame.lastTempTime = m_sensorFrame.LastSampleTime;
				}
				m_pSensorfusion->OnSensorData(m_sensorFrame);
				if (MojingSDKStatus::GetSDKStatus()->GetTrackerStatus() != TRACKER_START)
				{
					MojingSDKStatus::GetSDKStatus()->SetTrackerStatus(TRACKER_START);
					SetYaw();
				}
			}
		}
	}
}
