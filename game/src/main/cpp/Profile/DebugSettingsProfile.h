#pragma once
#include "../Parameters/MojingJsonElement.h"
namespace Baofeng
{
	namespace Mojing
	{
		class DebugSettingsProfile :public MojingJsonElement
		{
		public:
			DebugSettingsProfile();
			virtual ~DebugSettingsProfile();
			virtual JSON* ToJson(){ return NULL; };
			virtual bool FromJson(JSON*);
			
			CLASS_INTERFACE_J(bool, m_b, ShowTimeWapRange);
		};

	}
}

