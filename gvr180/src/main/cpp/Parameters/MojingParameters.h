#pragma once

#ifdef MJ_OS_ANDROID
#include <jni.h>
#endif
#include "../Base/MojingMath.h"
#include "MojingDisplayParameters.h"
#include "MojingDeviceParameters.h"
#include "MojingFactoryCalibrationParameters.h"
#include "MojingGyroOffsetReportSet.h"
#include "MojingSensorParameters.h"
#include "../Profile/GlassesConfigProfileV2.h"
#include "../Profile/UserSettingProfile.h"
#include "../Profile/DebugSettingsProfile.h"
#include "../Profile/JoystickProfile.h"

namespace Baofeng
{
	namespace Mojing
	{
// 		// Gyro autocalibration result.
// 		struct GyroOffsetReport
// 		{
// 			GyroOffsetReport()
// 				: Offset(0)
// 				, Temperature(0)
// 			{}
// 
// 			GyroOffsetReport(Vector3d offset, double temperature)
// 				: Offset(offset)
// 				, Temperature(temperature)
// 			{}
// 
// 			Vector3d    Offset;
// 			double      Temperature;
// 		};

		class Parameters
		{
		public:
			Parameters();
			virtual ~Parameters();
			bool Init();
			CLASS_MEMBER_STR(String, m_str, OSVersion);
			CLASS_MEMBER(bool, m_b, HDMWorking);
			CLASS_INTERFACE(MojingDisplayParameters*, m_p, DisplayParameters);
			CLASS_INTERFACE(MojingDeviceParameters*, m_p, DeviceParameters);
			CLASS_INTERFACE(MojingFactoryCalibrationParameters*, m_p, FactoryCalibrationParameters);
			CLASS_INTERFACE(MojingGyroOffsetReportSet*, m_p, GyroOffsetReportSet);
			CLASS_INTERFACE(MojingSensorParameters*, m_p, SensorParameters);
			CLASS_INTERFACE(GlassesConfigProfileV2*, m_p, GlassesConfigProfile);
			CLASS_INTERFACE(UserSettingProfile*, m_p, UserSettingProfile);
			CLASS_INTERFACE(DebugSettingsProfile*, m_p, DebugSettingsProfile);
			CLASS_INTERFACE(JoystickProfile*, m_p, JoystickProfile);
		};
	}
}
