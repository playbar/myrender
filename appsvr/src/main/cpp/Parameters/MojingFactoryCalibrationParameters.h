#pragma once
#include "../Base/MojingMath.h"
#include "MojingJsonElement.h"
namespace Baofeng
{
	namespace Mojing
	{
		struct FactoryCalibration
		{
			Vector3f AccelOffset;
			Vector3f GyroOffset;
			Matrix4f AccelMatrix;
			Matrix4f GyroMatrix;
			float    Temperature;
		};

		class MojingFactoryCalibrationParameters :
			public MojingJsonElement
		{
		public:
			MojingFactoryCalibrationParameters();
			virtual ~MojingFactoryCalibrationParameters();

			virtual JSON* ToJson();
			virtual bool FromJson(JSON*);

			FactoryCalibration ToStruct()const;
			bool FromStruct(FactoryCalibration V);



			virtual bool Load();
			virtual bool Save();
			CLASS_MEMBER_V3_J(Vector3f, m_, AccelOffset);
			CLASS_MEMBER_V3_J(Vector3f, m_, GyroOffset);
			
			CLASS_MEMBER_M4_J(Matrix4f, m_, AccelMatrix);
			CLASS_MEMBER_M4_J(Matrix4f, m_, GyroMatrix);

			CLASS_MEMBER_J(float, m_f, Temperature);
			CLASS_INTERFACE(bool , m_b , HaveLoaclProfile);
			CLASS_MEMBER(bool, m_b, Calibrated);
		};

	}
}