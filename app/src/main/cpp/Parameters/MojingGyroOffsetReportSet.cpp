#include "../Base/MojingTypes.h"
#include "MojingGyroOffsetReportSet.h"
#include "MojingGyroOffsetReport.h"

namespace Baofeng
{
	namespace Mojing
	{
		MojingGyroOffsetReportSet::MojingGyroOffsetReportSet(int iBins, int iSamples)
		{
			SetClassName(__FUNCTION__);
			SetNumBins(iBins);
			SetNumSamples(iSamples);
			AllocMemory();
		}


		MojingGyroOffsetReportSet::~MojingGyroOffsetReportSet()
		{
			FreeMemory();
		}

		void MojingGyroOffsetReportSet::AllocMemory()
		{
//			FreeMemory();

			m_MojingGyroOffsetReport = new MojingGyroOffsetReport*[m_iNumBins];
			for (int iX = 0; iX < m_iNumBins; iX++)
				m_MojingGyroOffsetReport[iX] = new MojingGyroOffsetReport[m_iNumSamples]; 
			
		}
		void MojingGyroOffsetReportSet::FreeMemory()
		{
			for (int iX = 0; iX < m_iNumBins; iX++)
				delete[] m_MojingGyroOffsetReport[iX];
			delete[] m_MojingGyroOffsetReport;
			m_MojingGyroOffsetReport = NULL;
		}

		JSON* MojingGyroOffsetReportSet::ToJson()
		{
			JSON* pRet = JSON::CreateObject();
			ClassNameToJson(pRet);
			ClassVersionToJson(pRet);
			NumBinsToJson(pRet);
			NumSamplesToJson(pRet);
			JSON* pArrayX = JSON::CreateArray();
			for (int iX = 0; iX < m_iNumBins; iX++)
			{
				JSON* pArrayY = JSON::CreateArray();
				for (int iY = 0; iY < m_iNumSamples; iY++)
				{
					pArrayY->AddArrayElement(m_MojingGyroOffsetReport[iX][iY].ToJson());
				}
				pArrayX->AddArrayElement(pArrayY);
			}
			pRet->AddItem("MojingGyroOffsetReport" , pArrayX);
			return pRet;
		}

		bool MojingGyroOffsetReportSet::FromJson(JSON*pJson)
		{
			bool bRet = true;
			int iOldNumBins = GetNumBins();
			int iOldNumSamples = GetNumSamples();
			NumBinsFromJson(pJson);
			NumSamplesFromJson(pJson);
			int iJsonNumBins = GetNumBins();
			int iJsonNumSamples = GetNumSamples();

			int iNewBins = fmax(iOldNumBins, iJsonNumBins );
			int iNewSamples = fmax(iOldNumSamples, iJsonNumSamples);
            
			if (iNewSamples > iOldNumBins || iNewSamples > iOldNumSamples)
			{
				SetNumBins(iNewBins);
				SetNumSamples(iNewSamples);
				AllocMemory();
			}
			JSON* pArrayX = pJson->GetItemByName("MojingGyroOffsetReport");
			if (pArrayX && pArrayX->GetArraySize() == iJsonNumBins)
			{
				for (int iX = 0; iX < iJsonNumBins && bRet; iX++)
				{
					JSON* pArrayY = pArrayX->GetItemByIndex(0);
					if (pArrayY && pArrayY->GetArraySize() == iJsonNumSamples)
					{
						for (int iY = 0; iY < iJsonNumSamples && bRet; iY++)
							bRet = m_MojingGyroOffsetReport[iX][iY].FromJson(pArrayY->GetItemByIndex(iY));
					}
					else
					{
						bRet = false;
					}
				}
			}
			else
			{
				bRet = false;
			}
			return bRet;
		}

	}
}