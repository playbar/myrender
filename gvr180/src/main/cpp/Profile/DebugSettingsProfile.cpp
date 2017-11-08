#include "DebugSettingsProfile.h"

namespace Baofeng
{
	namespace Mojing
	{
		DebugSettingsProfile::DebugSettingsProfile()
		{
			SetClassName(__FUNCTION__);
			m_bShowTimeWapRange = false;
		}


		DebugSettingsProfile::~DebugSettingsProfile()
		{
			
		}
		bool DebugSettingsProfile::FromJson(JSON*pJson)
		{
			bool bRet = false;
			if (JSON * pClassName = pJson->GetItemByName("Class"))
			{
				if (pClassName && (pClassName->GetStringValue() == GetClassName()))
				{
					ShowTimeWapRangeFromJson(pJson);
				}
			}

			return true;
		}
	}
}
