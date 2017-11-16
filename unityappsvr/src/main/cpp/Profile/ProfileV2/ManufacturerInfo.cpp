#include "ManufacturerInfo.h"

namespace Baofeng
{
	namespace Mojing
	{
		ManufacturerInfo::ManufacturerInfo():
			m_uiID(0)
		{
			SetClassName(__FUNCTION__);
		}


		ManufacturerInfo::~ManufacturerInfo()
		{
			m_Display.clear();
		}

		JSON* ManufacturerInfo::ToJson()
		{
			JSON* pRet = JSON::CreateObject();
			URLToJson(pRet);
			IDToJson(pRet);
			return pRet;
		}
		JSON* ManufacturerInfo::ToJson(unsigned short wLanguageCode)
		{
			JSON* pRet = ToJson();
			if (m_Display.find(wLanguageCode) != m_Display.end())
			{
				pRet->AddStringItem("Display", m_Display[wLanguageCode].ToCStr());
			}
			return pRet;
		}
		bool ManufacturerInfo::FromJson(JSON* pJson)
		{
			bool bRet = false;
			if (NameFromJson(pJson) &&
				URLFromJson(pJson) &&
				IDFromJson(pJson))
			{
				JSON *pDisplay = pJson->GetItemByName("Display");
				JSON *pProductID = pJson->GetItemByName("ProductID");
				if (pDisplay && ParseLanguageDisplay(&m_Display, pDisplay) &&
					pProductID && ParseIDList(&m_AllProductID, pProductID))
				{
#ifdef _DEBUG
					JSON *pDebugProductID = pJson->GetItemByName("DebugProductID");
					if (pDebugProductID)
					{
						ParseIDList(&m_AllProductID, pDebugProductID);
					}
#endif
					bRet = true;
				}
			}
			return bRet;
		}
	}
}