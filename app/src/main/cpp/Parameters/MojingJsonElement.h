#pragma once
#include "../Base/MojingTypes.h"
#include "../Base/MojingJSON.h"

namespace Baofeng
{
	namespace Mojing
	{
		class MojingJsonElement
		{
		public:
			MojingJsonElement();
			virtual ~MojingJsonElement();
			virtual JSON* ToJson() = 0;
			virtual bool FromJson(JSON*) = 0;
			virtual bool Save();
			virtual bool Load();
			CLASS_INTERFACE_STR_J(String, m_, ClassName);
			CLASS_INTERFACE_J(int, m_i, ClassVersion);
		};

	}//namespace Mojing
}//namespace Baofeng
