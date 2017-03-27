#pragma once
#include "../Base/MojingMath.h"
#include "MojingJsonElement.h"
namespace Baofeng
{
	namespace Mojing
	{
		struct GyroOffsetReport;
		// Gyro autocalibration result.
		class MojingGyroOffsetReport :
			public MojingJsonElement
		{

		public:
			MojingGyroOffsetReport();
			virtual ~MojingGyroOffsetReport();

			virtual JSON* ToJson();
			virtual bool FromJson(JSON*);

			virtual GyroOffsetReport ToStruct()const;

			void        operator =  (const struct GyroOffsetReport& src);

			CLASS_MEMBER_V3_J(Vector3d, m_, Offset);
			CLASS_MEMBER_J(double,m_d,Temperature);
			CLASS_MEMBER_J(UInt32,m_u, Time);
		};
		struct GyroOffsetReport
		{

			GyroOffsetReport()
			: Offset(0), Temperature(0)
			{}

			GyroOffsetReport(
				Vector3d	offset,
				double		temperature)
				: Offset(offset), Temperature(temperature)
			{}
			void        operator =  (const MojingGyroOffsetReport& src){
				*this = src.ToStruct();
			};
			Vector3d    Offset;
			double      Temperature;
		};


	}
}