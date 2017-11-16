#include "ProductInfo.h"
#include "../../MojingManager.h"
#include "../../Parameters/MojingParameters.h"
#include "../../Parameters/MojingDeviceParameters.h"

#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

namespace Baofeng
{
	namespace Mojing
	{
		ProductInfo::ProductInfo() :
			m_uiID(0),
			m_dLensSeparation(0),
			m_dLensSeparationMin(0),
			m_dLensSeparationMax(0),
			m_iMachineID(0)
		{
			SetClassName(__FUNCTION__);
		}


		ProductInfo::~ProductInfo()
		{
			m_Display.clear();
			for (unsigned int i = 0; i < m_MobilePositon.size(); i++)
			{
				delete m_MobilePositon[i];
			}
			m_MobilePositon.clear();
		}

		JSON* ProductInfo::ToJson()
		{
			JSON* pRet = JSON::CreateObject();
			URLToJson(pRet);
			IDToJson(pRet);
			return pRet;
		}

		JSON* ProductInfo::ToJson(unsigned short wLanguageCode) 
		{
			JSON* pRet = ToJson();
			if (m_Display.find(wLanguageCode) != m_Display.end())
			{
				pRet->AddStringItem("Display", m_Display[wLanguageCode].ToCStr());
			}
			return pRet;
		}
		bool ProductInfo::CheckEnable()const
		{
			MojingDeviceParameters *pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
#if defined(MJ_OS_ANDROID)
			const char* szOS = "Android";
#elif defined(MJ_OS_MAC)
			const char* szOS = "iOS";
#elif defined(MJ_OS_WIN32)
			const char* szOS = "Windows";
#endif
			MachineListNode CurrentMachineType = pDeviceParameters->GetCurrentMachine();
			bool bCheckOS = szOS == NULL || *szOS == 0 || m_szOS.GetLength() < 1 || 0 == MJ_stricmp(szOS , m_szOS.ToCStr());

			bool bCheckMachine = GetMachineID() == CurrentMachineType.m_iID;
			bool bRet = bCheckOS && bCheckMachine;
			return bRet;
		}
		bool ProductInfo::FromJson(JSON* pJsonNode)
		{
			bool bRet = false;
			if (NameFromJson(pJsonNode) &&
				URLFromJson(pJsonNode) &&
				IDFromJson(pJsonNode))
			{
				JSON *pDisplay = pJsonNode->GetItemByName("Display");
				JSON *pLensSeparation = pJsonNode->GetItemByName("LensSeparation");
				JSON *pMobilePositon = pJsonNode->GetItemByName("MobilePositon");
				JSON *pGlassID = pJsonNode->GetItemByName("GlassID");
				
				JSON *pOS	 = pJsonNode->GetItemByName("OS");
				
				JSON *pMachineID = pJsonNode->GetItemByName("MachineID");

				if (pDisplay && ParseLanguageDisplay(&m_Display, pDisplay) &&
					pGlassID && ParseIDList(&m_AllGlassID, pGlassID))
				{
					if (pLensSeparation && pLensSeparation->Type == JSON_Array && pLensSeparation->GetArraySize() >= 1 && pLensSeparation->GetArraySize() <= 3)
					{
						if (pLensSeparation->GetArraySize() == 1)
						{
							SetLensSeparation(pLensSeparation->GetArrayNumber(0));
							SetLensSeparationMin(pLensSeparation->GetArrayNumber(0));
							SetLensSeparationMax(pLensSeparation->GetArrayNumber(0));
						}
						else if (pLensSeparation->GetArraySize() == 2)
						{
							SetLensSeparation((pLensSeparation->GetArrayNumber(0) + pLensSeparation->GetArrayNumber(1)) / 2);
							SetLensSeparationMin(pLensSeparation->GetArrayNumber(0));
							SetLensSeparationMax(pLensSeparation->GetArrayNumber(1));
						}
						else if (pLensSeparation->GetArraySize() == 3)
						{
							SetLensSeparation(pLensSeparation->GetArrayNumber(2));
							SetLensSeparationMin(pLensSeparation->GetArrayNumber(0));
							SetLensSeparationMax(pLensSeparation->GetArrayNumber(1));
						}
						/*以下是可选参数*/
						if (pMobilePositon && pMobilePositon->Type == JSON_Array)
						{// 手机摆放位置
							for (int i = 0; i < pMobilePositon->GetArraySize(); i++)
							{
								MobilePositionInfo *pNewItem = new MobilePositionInfo;
								if (pNewItem->FromJson(pMobilePositon->GetItemByIndex(i)))
								{
									m_MobilePositon.push_back(pNewItem);
								}
								else
								{
									delete pNewItem;
								}
							}
						}
						
						if (pOS && pOS->Type == JSON_String)
						{
							SetOS(pOS->GetStringValue());
						}
// 						if (pBrand && pBrand->Type == JSON_String)
// 						{
// 							SetBrand(pBrand->GetStringValue());
// 						}
// 						if (pModel && pModel->Type == JSON_String)
// 						{
// 							SetModel(pModel->GetStringValue());
// 						}
						if (pMachineID && pMachineID->Type == JSON_Number)
						{
							SetMachineID(pMachineID->GetInt32Value());
						}

						
						bRet = true;
					}// LensSeparation
				}// Display,GlassID
			}// Name , URL , ID
			return bRet;
		}
		const MobilePositionInfo ProductInfo::GetMobilePositoin(const float fMobileSize)const
		{
			MobilePositionInfo Ret;
			float fLast = -1;
			for (int i = 0; i < (int)m_MobilePositon.size(); i++)
			{
				MobilePositionInfo* pMPI = m_MobilePositon[i];
				float fSize = pMPI->GetMobileSize();
				

				if (fLast < fSize && fMobileSize >= fSize)
				{
					Ret = *m_MobilePositon[i];
					fLast = Ret.GetMobileSize();
				}
			}
			return Ret;
		}
	}
}