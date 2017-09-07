#pragma once
#include "../Parameters/MojingJsonElement.h"

#define CHECK_MOBILECONFIG_INTERVEL		86400.0
#define CHECK_GLASSCONFIG_INTERVEL		86400.0
#define CHECK_JOYSTICKPROFILE_INTERVEL  86400.0

#define	SENSOR_ORIGIN_EXTERNAL_SDK		0
#define	SENSOR_ORIGIN_LOCAL_NATIVE		1
#define	SENSOR_ORIGIN_LOCAL_JAVA		2
#define	SENSOR_ORIGIN_EXTERNAL_DEVICE	3

namespace Baofeng
{
	namespace Mojing
	{
		class UserSettingProfile :
			public MojingJsonElement
		{
		public:
			UserSettingProfile();
			virtual ~UserSettingProfile();
			virtual JSON* ToJson();
			virtual bool FromJson(JSON*);
			float GetUserScreenSizePPI();
			CLASS_MEMBER_J(bool, m_b, EnableScreenSize);
			CLASS_MEMBER_J(float, m_f, ScreenSize);
			CLASS_MEMBER_J(bool, m_b, SensorDataFromMJSDK);
			CLASS_MEMBER_J(bool, m_b, SensorDataFromJava);
			CLASS_MEMBER_J(double, m_d, CheckMobileConfig);
			CLASS_MEMBER_J(double, m_d, CheckGlassConfig);
			CLASS_MEMBER_J(double, m_d, CheckJoystickProfile);	
		};


	}
}
