#pragma once
#include "../Base/MojingMath.h"
#include "MojingAndroidSensor.h"
namespace Baofeng
{
	namespace Mojing
	{
#ifdef MJ_OS_ANDROID
		struct __tagSampleChecker
		{
			Vector3f m_Sample;	//	采样V3
			float m_fTemperature;// 温度
			float m_fSampleTime;// 采样时间，以秒为单位
		};
		struct __tagSampleCheckeResult
		{
			// SAMPLEs
			int		m_iSampleCount;				// 本次统计的样本数，最大值500
			float	m_fSampleOffset[500];		// 所有采样节点的偏心度，没有采样到的为0值
			// SAMPLE OFFSET
			float	m_fMaxContiguousOffset;		//	相邻采样最大差值
			float	m_fMaxSampleOffset;			//	采样最大差值
			float	m_fAvgSampleOffset;			//	平均采样偏心度
			
			float	m_fMaxContiguousAngle;		//	相邻采样最大轴偏角,单位是弧度
			float	m_fMaxSampleAngle;			//	采样最大轴偏角,单位是弧度
			
			// SAMPLE TIME ， In Sec.
			float	m_fMaxContiguousTimeSpace;	// 最大相邻采样时间间隔,以毫秒为单位

			// Temperature
			float	m_fAvfTemperature;			// 平均温度
			float	m_fTemperatureOffset;		// 温度变化
		};
		class AndroidInternalSensorChecker :public AndroidInternalSensor
		{
		public:
			AndroidInternalSensorChecker();
			virtual ~AndroidInternalSensorChecker();
			

			virtual int Run();
			static __tagSampleCheckeResult * GetSampleCheckeResult();
		private:
			virtual void OnSensorData(MessageBodyFrame& sensorFrame);
			// 统计所有的误差
			__tagSampleChecker m_Sample[500];	// 500个采样数据
			void CalculateSamples(__tagSampleCheckeResult * pSampleCheckeResult);// 返回本轮统计计算的采样数

			int m_iSampleCount;		// 已经得到的采样数
			float m_SampleOffset[499];	// 从下标1开始的相邻两个元素的差，即So[n] = S[n+1] - S[n] 
			float m_AvgSampleOffset;// 平均偏差 = m_SampleOffset的平均
			float m_MaxSampleOffset;// 最大偏差 = m_SampleOffset的最大值
			float m_MaxSampleOffsetIn500Samples;// 最大偏差，在m_Sample中任意最大两个节点之间最大的数值。
		};
#endif
	}
}
