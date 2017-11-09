#include "MojingFactoryCalibrationParameters.h"
#include "../Base/MojingTypes.h"
namespace Baofeng
{
	namespace Mojing
	{
		MojingFactoryCalibrationParameters::MojingFactoryCalibrationParameters()
			:m_bHaveLoaclProfile(0), m_bCalibrated(0)
		{
			m_AccelOffset = Vector3f::ZERO;
			m_GyroOffset = Vector3f::ZERO;
			m_AccelMatrix = Matrix4f::Identity();
			m_GyroMatrix = Matrix4f::Identity();
			m_fTemperature = 0;
			SetClassName(__FUNCTION__);
		}


		MojingFactoryCalibrationParameters::~MojingFactoryCalibrationParameters()
		{
		}

		JSON* MojingFactoryCalibrationParameters::ToJson()
		{
			JSON* pJson = JSON::CreateObject();
			pJson->Name = "FactoryCalibrationParameters";

			ClassNameToJson(pJson);
			ClassVersionToJson(pJson);
			AccelOffsetToJson(pJson);
			GyroOffsetToJson(pJson);
			AccelMatrixToJson(pJson);
			GyroMatrixToJson(pJson);
			TemperatureToJson(pJson);
			return pJson;

		}
		bool MojingFactoryCalibrationParameters::FromJson(JSON*pJsonBase)
		{
			JSON* pJson = pJsonBase;
			if (pJson)
			{
				return AccelOffsetFromJson(pJson)&&
					GyroOffsetFromJson(pJson) &&
					AccelMatrixFromJson(pJson) &&
					GyroMatrixFromJson(pJson) &&
				TemperatureFromJson(pJson);
			}
			return false;
		}

		FactoryCalibration MojingFactoryCalibrationParameters::ToStruct()const
		{
			FactoryCalibration Ret;
			Ret.AccelOffset = GetAccelOffset();
			Ret.GyroOffset  =  GetGyroOffset();
			Ret.AccelMatrix = GetAccelMatrix();
			Ret.GyroMatrix  =  GetGyroMatrix();
			Ret.Temperature = GetTemperature();
			return Ret;
		}
		
		bool MojingFactoryCalibrationParameters::FromStruct(FactoryCalibration V)
		{
			SetAccelOffset(V.AccelOffset);
			SetGyroOffset(V.GyroOffset);
			SetAccelMatrix(V.AccelMatrix);
			SetGyroMatrix(V.GyroMatrix);
			SetTemperature(V.Temperature);

			return true;
		}
		bool MojingFactoryCalibrationParameters::Load()
		{
			bool bRet = MojingJsonElement::Load();
			if (bRet && !GetHaveLoaclProfile())
				SetHaveLoaclProfile(true);
			return bRet;
		}
		bool MojingFactoryCalibrationParameters::Save()
		{
			bool bRet = MojingJsonElement::Save();
			if (bRet)
			{
				if (!GetHaveLoaclProfile())
					SetHaveLoaclProfile(true);
				if (!GetCalibrated())
					SetCalibrated(true);
			}
			
			return bRet;
		}
	}
}