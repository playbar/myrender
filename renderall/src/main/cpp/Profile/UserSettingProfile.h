#pragma once
#include "../Parameters/MojingJsonElement.h"
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
			CLASS_MEMBER_J(bool, m_b, SensorDataFromJava);
			CLASS_MEMBER_J(double, m_d, CheckMobileConfig);
			CLASS_MEMBER_J(double, m_d, CheckGlassConfig);
		};


	}
}
