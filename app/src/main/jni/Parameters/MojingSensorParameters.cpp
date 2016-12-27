#include "MojingSensorParameters.h"
namespace Baofeng
{
	namespace Mojing
	{
		MojingSensorParameters::MojingSensorParameters():
			m_iMagSensorUncalibrated(0),
			m_iMagneticFieldSensor(0),
			m_iGyroScopeSensor(0),
			m_iAccelerometerSensor(0),
			m_fMaxSampleRate(0),
			m_fMinSampleRate(0),
			m_fAvgSampleRate(0),
			m_fLast50AvgSampleRate(0),
			m_iSocket(-1),
			m_iPort(0)
		{
			SetClassName(__FUNCTION__);
		}


		MojingSensorParameters::~MojingSensorParameters()
		{
		}
		JSON* MojingSensorParameters::ToJson()
		{
			JSON* pRet = JSON::CreateObject();
			ClassNameToJson(pRet);
			ClassVersionToJson(pRet);
			MagSensorUncalibratedToJson(pRet);
			MagneticFieldSensorToJson(pRet);
			GyroScopeSensorToJson(pRet);
			AccelerometerSensorToJson(pRet);
			MaxSampleRateToJson(pRet);
			MinSampleRateToJson(pRet);
			AvgSampleRateToJson(pRet);
			Last50AvgSampleRateToJson(pRet);
			return pRet;
		}
		bool MojingSensorParameters::FromJson(JSON*pJson)
		{
			MagSensorUncalibratedFromJson(pJson);
			MagneticFieldSensorFromJson(pJson);
			GyroScopeSensorFromJson(pJson);
			AccelerometerSensorFromJson(pJson);
			MaxSampleRateFromJson(pJson);
			MinSampleRateFromJson(pJson);
			AvgSampleRateFromJson(pJson);
			Last50AvgSampleRateFromJson(pJson);
			return true;
		}
	}
}


