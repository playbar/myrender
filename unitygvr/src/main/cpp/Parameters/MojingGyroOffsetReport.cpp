#include "MojingGyroOffsetReport.h"
#include "../Base/MojingTypes.h"
namespace Baofeng
{
	namespace Mojing
	{
		MojingGyroOffsetReport::MojingGyroOffsetReport()
			: m_dTemperature(0.0)
			, m_uTime(0)
		{
			SetClassName(__FUNCTION__);
		}


		MojingGyroOffsetReport::~MojingGyroOffsetReport()
		{
		}

		JSON* MojingGyroOffsetReport::ToJson()
		{
			JSON* pRet = JSON::CreateObject();
			ClassNameToJson(pRet);
			ClassVersionToJson(pRet);
			OffsetToJson(pRet);
			TemperatureToJson(pRet);
			TimeToJson(pRet);
			return pRet;
		}
		bool MojingGyroOffsetReport::FromJson(JSON*pJson)
		{
			OffsetFromJson(pJson);
			TemperatureFromJson(pJson);
			TimeFromJson(pJson);
            return true;
		}
		GyroOffsetReport MojingGyroOffsetReport::ToStruct()const
		{
			GyroOffsetReport Ret;
			Ret.Offset = GetOffset();
			Ret.Temperature = GetTemperature();
			return Ret;
		}

		void   MojingGyroOffsetReport::operator =  (const struct GyroOffsetReport& src)
		{
			SetOffset(src.Offset);
			SetTemperature(src.Temperature);
		}
	}
}