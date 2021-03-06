﻿#include "GlassInfo.h"

#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#include "../../MojingSDKStatus.h"

namespace Baofeng
{
	namespace Mojing
	{
		bool ParseLanguageDisplay(DisplayInfo*pDisplayMap, JSON *pDisplayNode)
		{
			if (pDisplayNode->Type == JSON_Object)
			{
				pDisplayMap->clear();
				JSON *pLanguage = pDisplayNode->GetFirstItem();
				while (pLanguage != NULL)
				{
					unsigned short wLanguageCode  = GetLanguageCode(pLanguage->Name.ToCStr());
					if (wLanguageCode != 0)
					{
						String ss = pLanguage->GetStringValue();
						(*pDisplayMap)[wLanguageCode] = ss;
					}

					pLanguage = pDisplayNode->GetNextItem(pLanguage);
				}
				return true;
			}
			return false;
		}
		bool ParseIDList(vector<unsigned int> *pIDList, JSON *pIDListNode)
		{
			if (pIDListNode->Type == JSON_Array)
			{
				int iCount = pIDListNode->GetArraySize();
				for (int i = 0; i < iCount; i++)
				{
					pIDList->push_back(pIDListNode->GetArrayNumber(i));
				}
				return true;
			}
			return false;
		}
		unsigned short GetLanguageCode(const char * szLanguageName)
		{
			unsigned short wLanguageCode = 0;
			if (szLanguageName)
			{
				String strLanguage = szLanguageName;
				strLanguage = strLanguage.ToUpper();
				const char *pStrLanguage = strLanguage.ToCStr();
				if (strlen(pStrLanguage) == 2)
				{
					memcpy(&wLanguageCode, pStrLanguage, 2);
				}
			}
			return wLanguageCode;
		}
		GlassInfo::GlassInfo():
			m_iSegments(0),
			m_fMetersPerTanAngleAtCenter(0),
			m_fFOV(0),
			m_uiID(0),
			m_iNoDispersion(0),
			m_bReCalculationKT(false)
		{
			SetClassName(__FUNCTION__);
			for (int i = 0 ; i < 20 ; i++)
            {
				m_fR[i] = m_fG[i] = m_fB[i] = m_fL[i] = m_fRT[i] = m_fGT[i] = m_fBT[i] = 1;
            }
		}


		GlassInfo::~GlassInfo()
		{
			m_Display.clear();
		}
		JSON* GlassInfo::ToJson()
		{
			JSON* pRet = JSON::CreateObject();
			URLToJson(pRet);
			IDToJson(pRet);
			if ((Mojing::MojingSDKStatus::GetSDKStatus()->GetEngineStatus() & ENGINE_UNITY) == 0)
			{// 注意：因为现在Unity还不支持这个节点，所以暂时不要给Unity返回
				if (m_szDURL.GetLength())
				{					
					DURLToJson(pRet);
				}
			}
			return pRet;
		}
		JSON* GlassInfo::ToJson(unsigned short wLanguageCode)
		{
			JSON* pRet = ToJson();
			if (m_Display.find(wLanguageCode) != m_Display.end())
			{
				pRet->AddStringItem("Display", m_Display[wLanguageCode].ToCStr());
			}
			return pRet;
		}
		bool GlassInfo::FromJson(JSON* pJson)
		{
			bool bRet = false;
			if (GlassesNameFromJson(pJson) &&
				IDFromJson(pJson) &&
				URLFromJson(pJson))
			{

				if (DURLFromJson(pJson))
				{
					MOJING_TRACE(g_APIlogger, "Find DURL @ GID = "<< GetID() <<"....");
				}
				/*Display被要求修改为可选节点*/
				JSON *pDisplay = pJson->GetItemByName("Display");
				if (pDisplay && pDisplay->Type == JSON_Object)
				{
					if (!ParseLanguageDisplay(&m_Display, pDisplay))
					{
						return false;
					}
				}// end of display
				JSON *pDistortion = pJson->GetItemByName("Distortion");
				if (pDistortion && pDistortion->Type == JSON_Object &&
					FOVFromJson(pDistortion) &&
					SegmentsFromJson(pDistortion))
				{
					if (pDistortion->GetItemByName("NoDispersion"))
					{
						NoDispersionFromJson(pDistortion);
					}

					if (m_iSegments > 0 &&
						MetersPerTanAngleAtCenterFromJson(pDistortion))
					{
						JSON *pKR = pDistortion->GetItemByName("KR");
						JSON *pKG = pDistortion->GetItemByName("KG");
						JSON *pKB = pDistortion->GetItemByName("KB");
						// For Unreal的转置表
						JSON *pKRT = pDistortion->GetItemByName("KRT");
						JSON *pKGT = pDistortion->GetItemByName("KGT");
						JSON *pKBT = pDistortion->GetItemByName("KBT");
						// 亮度，在K值有效时
						JSON *pKL = pDistortion->GetItemByName("KL");
						if (pKB && pKG && pKB &&
							pKR->Type == JSON_Array && pKR->GetArraySize() == m_iSegments &&
							pKG->Type == JSON_Array && pKG->GetArraySize() == m_iSegments &&
							pKB->Type == JSON_Array && pKB->GetArraySize() == m_iSegments)
						{
							
							for (int iIndex = 0; iIndex < m_iSegments; iIndex++)
							{
								m_fR[iIndex] = pKR->GetArrayNumber(iIndex);
								m_fG[iIndex] = pKG->GetArrayNumber(iIndex);
								m_fB[iIndex] = pKB->GetArrayNumber(iIndex);
								if (pKL)
								{
									m_fL[iIndex] = pKL->GetArrayNumber(iIndex);
								}
							}
							if (pKL)
							{
								for (int iIndex = m_iSegments; iIndex < 20; iIndex++)
									m_fL[iIndex] = 0;
							}

							bRet = true;
						}// end of Ks 

						if (pKBT && pKGT && pKBT &&
							pKRT->Type == JSON_Array && pKRT->GetArraySize() == m_iSegments &&
							pKGT->Type == JSON_Array && pKGT->GetArraySize() == m_iSegments &&
							pKBT->Type == JSON_Array && pKBT->GetArraySize() == m_iSegments)
						{
							for (int iIndex = 0; iIndex < m_iSegments; iIndex++)
							{
								m_fRT[iIndex] = pKRT->GetArrayNumber(iIndex);
								m_fGT[iIndex] = pKGT->GetArrayNumber(iIndex);
								m_fBT[iIndex] = pKBT->GetArrayNumber(iIndex);
							}
							m_bReCalculationKT = false;
						}// end of KTs
						else
						{
							m_bReCalculationKT = true;
						}
					}
					else// segment > 0 && Distortion
					{
						bRet = true;
					}
				}// end of Distortion

			}
			return bRet;
		}
	}
}
