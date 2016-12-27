#include "AndroidInternalSensorChecker.h"
#include "MojingTemperature.h"
namespace Baofeng
{
	namespace Mojing
	{
#ifdef MJ_OS_ANDROID
		__tagSampleCheckeResult g_SampleCheckeResult = {0};

		AndroidInternalSensorChecker::AndroidInternalSensorChecker():
			m_iSampleCount(0)
		{
		}


		AndroidInternalSensorChecker::~AndroidInternalSensorChecker()
		{
		}

		int AndroidInternalSensorChecker::Run()
		{
			SetThreadName("SensorChecker");
			m_iSampleCount = 0;
			memset(&g_SampleCheckeResult, 0, sizeof(__tagSampleCheckeResult));
			int ident = 0;
			int fd = 0;
			int events = 0;
			CreateSensor();
			int skipSamples = 10;
			while (skipSamples > 0 && !GetExitFlag())
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
							}
						}
					}
				}
			}// while (skipSamples > 0 && !GetExitFlag())

			MessageBodyFrame theSample;
			memset(&theSample, 0, sizeof(MessageBodyFrame));
			theSample.Temperature = getTemperature();
			if (theSample.Temperature < 0) theSample.Temperature = 2500;	// 如果不能拿到温度值，则设置成25度

			while (m_iSampleCount < 500 && !GetExitFlag())
			{
				while ((ident = ALooper_pollAll(3, &fd, &events, NULL) >= 0) && !GetExitFlag())
				{
					if (ident == LOOPER_ID)
					{
						ASensorEvent event;
						while (ASensorEventQueue_getEvents(m_pQueue, &event, 1) > 0 && !GetExitFlag())
						{
							if (event.type == ASENSOR_TYPE_GYROSCOPE)
							{
								theSample.RotationRate.x = -event.vector.y;
								theSample.RotationRate.y = event.vector.x;
								theSample.RotationRate.z = event.vector.z;
								theSample.LastSampleTime = NanoSecondToSecond(event.timestamp);
								OnSensorData(theSample);// m_iSampleCount 会在这个函数里面修改
							}
						}
					}//if (ident == LOOPER_ID)
				}//while ((ident = ALooper_pollAll(3, &fd, &events, NULL) >= 0))
			}// while (m_iSampleCount < 500)

			DeleteSensor();
			return 0;
		}
	
		void AndroidInternalSensorChecker::OnSensorData(MessageBodyFrame& sensorFrame)
		{
			// 根据需要更新温度
			if ((sensorFrame.LastSampleTime - sensorFrame.lastTempTime) > 0.2f)
			{
				// 200ms获取温度一次
				float Temp = getTemperature();
				if (Temp > 0)
					sensorFrame.Temperature = Temp;
				sensorFrame.lastTempTime = sensorFrame.LastSampleTime;
			}

			__tagSampleChecker NewSample;
			NewSample.m_Sample = sensorFrame.RotationRate;
			NewSample.m_fTemperature = sensorFrame.Temperature;
			NewSample.m_fSampleTime = sensorFrame.LastSampleTime;
			m_Sample[m_iSampleCount++] = NewSample;

			CalculateSamples(&g_SampleCheckeResult);
		}

		void AndroidInternalSensorChecker::CalculateSamples(__tagSampleCheckeResult * pSampleCheckeResult)// 返回本轮统计计算的采样数
		{
			// 防止多线程的资源竞争，先把数据记录下来。
			int iSampleCount = m_iSampleCount;
			if (iSampleCount <= 1)
				return ;// 防止后面的除法出错，直接返回

			double dAvgSampleOffset = 0;
			double dTemperature = 0;
			float fTemperatureMin = 500;
			float fTemperatureMax = 0;
			for (int iSampleIndex = pSampleCheckeResult->m_iSampleCount; iSampleIndex < iSampleCount; iSampleIndex++)
			{
				// 截距
				float fLengthSq = m_Sample[iSampleIndex].m_Sample.LengthSq();
				pSampleCheckeResult->m_fSampleOffset[iSampleIndex] = fLengthSq;
				
				dTemperature += m_Sample[iSampleIndex].m_fTemperature;
				fTemperatureMin = fmin(m_Sample[iSampleIndex].m_fTemperature, fTemperatureMin);
				fTemperatureMax = fmax(m_Sample[iSampleIndex].m_fTemperature, fTemperatureMax);
				dAvgSampleOffset += fLengthSq;
				if (iSampleIndex)
				{// 相邻节点间计算
					float ContiguousOffset = m_Sample[iSampleIndex].m_Sample.DistanceSq( m_Sample[iSampleIndex - 1].m_Sample);
					pSampleCheckeResult->m_fMaxContiguousOffset = fmax(pSampleCheckeResult->m_fMaxContiguousOffset, ContiguousOffset);

					float ContiguousAngle = fabs(m_Sample[iSampleIndex].m_Sample.Angle(m_Sample[iSampleIndex-1].m_Sample));
					pSampleCheckeResult->m_fMaxContiguousAngle = fmax(pSampleCheckeResult->m_fMaxContiguousAngle, ContiguousAngle);

					float ContiguousTimeSpace = m_Sample[iSampleIndex].m_fSampleTime - m_Sample[iSampleIndex - 1].m_fSampleTime;
					pSampleCheckeResult->m_fMaxContiguousTimeSpace = fmax(pSampleCheckeResult->m_fMaxContiguousTimeSpace, ContiguousTimeSpace);
				}

				// 最大轴偏角和最大截距
				for (int iSampleIndex2 = 0; iSampleIndex2 < iSampleIndex; iSampleIndex2++)
				{
					float Offset = m_Sample[iSampleIndex].m_Sample.DistanceSq(m_Sample[iSampleIndex2].m_Sample);
					pSampleCheckeResult->m_fMaxSampleOffset = fmax(pSampleCheckeResult->m_fMaxSampleOffset , Offset);

					float Angle = fabs(m_Sample[iSampleIndex].m_Sample.Angle(m_Sample[iSampleIndex2].m_Sample));
					pSampleCheckeResult->m_fMaxSampleAngle = fmax(pSampleCheckeResult->m_fMaxSampleAngle , Angle);
				}
			}

			pSampleCheckeResult->m_fAvgSampleOffset = dAvgSampleOffset / iSampleCount;
			pSampleCheckeResult->m_fAvfTemperature = dTemperature / iSampleCount;
			pSampleCheckeResult->m_fTemperatureOffset = fTemperatureMax - fTemperatureMin;
			
			// 最后赋值
			pSampleCheckeResult->m_iSampleCount = iSampleCount;
		}

		__tagSampleCheckeResult * AndroidInternalSensorChecker::GetSampleCheckeResult()
		{
			return &g_SampleCheckeResult;
		}
#endif
	}
}
