#include "GlassInfo.h"
#include "../../MojingManager.h"
#include "../../Parameters/MojingParameters.h"
#include "../../Parameters/MojingDisplayParameters.h"
#include "DayDreamParameters.h"

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
			m_szDURL_Original(""),
			m_szDURL(""),
			m_szURL(""),
			m_fDDScale(0),
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
#ifdef _DEBUG
			MOJING_FUNC_TRACE(g_APIlogger);
#endif
			JSON* pRet = JSON::CreateObject();
			URLToJson(pRet);
			IDToJson(pRet);
			//if ((Mojing::MojingSDKStatus::GetSDKStatus()->GetEngineStatus() & ENGINE_UNITY) == 0)
			{// 注意：因为现在Unity还不支持这个节点，所以暂时不要给Unity返回
				if (m_szDURL.GetLength())
				{	
					UpdateDURL();

					if (m_szDURL == m_szDURL_Original && (m_fDDScale <= 0.98f || m_fDDScale >= 1.02f))
					{
						DDScaleToJson(pRet);
					}
					DURLToJson(pRet);
				}
			}
			return pRet;
		}
		JSON* GlassInfo::ToJson(unsigned short wLanguageCode)
		{
#ifdef _DEBUG
			MOJING_FUNC_TRACE(g_APIlogger);
#endif
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

		void GlassInfo::UpdateDURL()
		{
#ifdef _DEBUG
			MOJING_FUNC_TRACE(g_APIlogger);
#endif
			if (m_szDURL_Original.GetSize() == 0 || m_szDURL_Original.IsEmpty())
				m_szDURL_Original = m_szDURL;
		//	MojingDeviceParameters *pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
			
			MojingDisplayParameters *pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
			if (pDisplayParameters != NULL)
			{
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "Check - 1");
#endif
				float fFixPPI = pDisplayParameters->GetUserPPI();
				if (fFixPPI < 1)
					fFixPPI = pDisplayParameters->GetPPI();
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "Check - 2 , fFixPPI = " << fFixPPI);
#endif
				// for test
			//	fFixPPI *= 2.0f;
				// test end				
				if (fFixPPI > 1)
				{
					m_fDDScale = fFixPPI / pDisplayParameters->GetYdpi();
#ifdef _DEBUG
					MOJING_TRACE(g_APIlogger, "Check - 3 , fPPI_Scale = " << m_fDDScale);
#endif

					if (m_fDDScale > 1.02 || m_fDDScale < 0.98)
					{
#ifdef _DEBUG
						MOJING_TRACE(g_APIlogger, "Replace DURL From: \"" << m_szDURL_Original.ToCStr() << "\"");
#endif
						int iBufferSize = CDayDreamParameters::UpdateDayDreamURL(m_szDURL_Original.ToCStr(), NULL, m_fDDScale);
						char *pszBuffer = new char[iBufferSize * 2];
						memset(pszBuffer, 0, iBufferSize * 2);
						int iRet = CDayDreamParameters::UpdateDayDreamURL(m_szDURL_Original.ToCStr(), pszBuffer, m_fDDScale);
						if (iRet > 1)
						{
#ifdef _DEBUG
							MOJING_TRACE(g_APIlogger, "Replace DURL  To : \"" << pszBuffer << "\"");
#endif
							m_szDURL = pszBuffer;
						}
						else
						{
							MOJING_ERROR(g_APIlogger, "UpdateDayDreamURL return : " << iRet);
						}
					}
					else
					{
#ifdef _DEBUG
						MOJING_TRACE(g_APIlogger, "Check - 4 , fPPI_Scale = " << m_fDDScale);
#endif
						m_fDDScale = 1.0f;
					}
				}// if (fFixPPI > 1)
				else
				{
#ifdef _DEBUG
					MOJING_TRACE(g_APIlogger, "Check - 5  , fFixPPI = " << fFixPPI);
#endif
				}

				// Modify no distortion 
				CDayDreamParameters TempDURL = CDayDreamParameters::FromDayDreamURL(m_szDURL);

				if (TempDURL.IsNoDistortion())
				{
					MojingDisplayParameters *pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
					float Heigth = pDisplayParameters->GetScreenHeightMeter();
					float Width = pDisplayParameters->GetScreenWidthMeter() / 2.0f;
					float Radius = fmin(Heigth, Width);				
					float FixedScreenToLens = Radius / tan(TempDURL.GetOuterFOV()*PI/180.0);
					TempDURL.SetScreenToLens(FixedScreenToLens);
					m_szDURL = TempDURL.GetDayDreamURL();
				}
			} 
			else // if (pDisplayParameters != NULL)
			{
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "Check - 6  , Can not get display parmeat");
				
#endif
			}
		}

	}
}
