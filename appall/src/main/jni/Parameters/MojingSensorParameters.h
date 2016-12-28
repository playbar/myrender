#pragma once
#include "MojingJsonElement.h"
namespace Baofeng
{
	namespace Mojing
	{
		class MojingSensorParameters :
			public MojingJsonElement
		{
		public:
			MojingSensorParameters();
			virtual ~MojingSensorParameters();
			virtual JSON* ToJson();
			virtual bool FromJson(JSON*);

			CLASS_MEMBER_STR_J(String, m_s, GlassName);

			CLASS_MEMBER_J(int, m_i, MagSensorUncalibrated);
			CLASS_MEMBER_J(int, m_i, MagneticFieldSensor);
			CLASS_MEMBER_J(int, m_i, GyroScopeSensor);
			CLASS_MEMBER_J(int, m_i, AccelerometerSensor);

			CLASS_MEMBER_J(int, m_i, Socket);
			CLASS_MEMBER_J(int, m_i, Port);

			CLASS_MEMBER_J(bool, m_b, IsLowPower);

			CLASS_MEMBER_J(float, m_f, MaxSampleRate);
			CLASS_MEMBER_J(float, m_f, MinSampleRate);
			CLASS_MEMBER_J(float, m_f, AvgSampleRate);
			CLASS_MEMBER_J(float, m_f, Last50AvgSampleRate);
		};

	}
}
