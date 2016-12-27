#include "MojingDeviceParameters.h"
#include "../Base/MojingTypes.h"
#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"

namespace Baofeng
{
	namespace Mojing
	{
		MojingDeviceParameters::MojingDeviceParameters()
		{
			SetClassName(__FUNCTION__);
			m_bIsMachine = false;
			m_bSensorDataFromJava = false;
		}


		MojingDeviceParameters::~MojingDeviceParameters()
		{
		}
		JSON* MojingDeviceParameters::ToJson()
		{
			JSON* pRet = JSON::CreateObject();
			ClassNameToJson(pRet);
			ClassVersionToJson(pRet);
			BrandToJson(pRet);
			DeviceToJson(pRet);
			DisplayToJson(pRet);
			FingerprintToJson(pRet);
			HardwareToJson(pRet);
			HostToJson(pRet);
			IDToJson(pRet);
			ModelToJson(pRet);
			ProductToJson(pRet);
			SerialToJson(pRet);
			TagsToJson(pRet);
			TypeToJson(pRet);

			return pRet;
		}

		bool MojingDeviceParameters::FromJson(JSON*pJsonItem)
		{
			bool bRet = false;
			JSON *pJson = pJsonItem->GetItemByName("DeviceParameterss");
			if (pJson)
			{
				BrandFromJson(pJson);
				DeviceFromJson(pJson);
				DisplayFromJson(pJson);
				FingerprintFromJson(pJson);
				HardwareFromJson(pJson);
				HostFromJson(pJson);
				IDFromJson(pJson);
				ModelFromJson(pJson);
				ProductFromJson(pJson);
				SerialFromJson(pJson);
				TagsFromJson(pJson);
				TypeFromJson(pJson);
			}
			return bRet;
		}

		void MojingDeviceParameters::CheckIsMachine()
		{
			if (FindIsMachine(m_sBrand, m_sModel) >= 0)
				m_bIsMachine = true;			
		}

		int MojingDeviceParameters::FindIsMachine(const  String szBrand, const String szModel) const
		{
			int iRet = -1;
			for (uint32_t i = 0; i < m_MachineList.size(); i++)
			{
				if (m_MachineList[i].m_szBrand == szBrand && m_MachineList[i].m_szModel == szModel)
				{
					iRet = i;
					break;
				}
			}
			return iRet;
		}
		void MojingDeviceParameters::AppendMachine(int iID, const  String szBrand, const String szModel, int iRender)
		{
			MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
			if (0 > pDeviceParameters->FindIsMachine(szBrand, szModel))
			{
				MachineListNode NewItem;
				NewItem.m_iID = iID;
				NewItem.m_szBrand = szBrand;
				NewItem.m_szModel = szModel;
				NewItem.m_iRender = iRender;
				pDeviceParameters->m_MachineList.push_back(NewItem);

				pDeviceParameters->CheckIsMachine();
			}
		}

		MachineListNode MojingDeviceParameters::GetCurrentMachine()const
		{
			MachineListNode Ret;
			Ret.m_iID = Ret.m_iRender = 0;
			int iIndex = FindIsMachine(m_sBrand, m_sModel);
			if (iIndex >= 0)
				Ret = m_MachineList[iIndex];
			return Ret;
		}
	}//namespace Mojing
}//namespace Baofeng