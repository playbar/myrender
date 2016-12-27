#include "MojingGyroTempCalibration.h"

#include "../Base/MojingJSON.h"
#include "../Base/MojingLog.h"
#include "../Base/MojingString_Utils.h"
#include "../Base/MojingTimer.h"
#include "../Reporter/GyroTempCalibrationReporter.h"

#ifdef MJ_OS_WIN32
#include <Shlobj.h>
#else
#include <dirent.h>
#include <sys/stat.h>

#ifdef MJ_OS_LINUX
#include <unistd.h>
#include <pwd.h>
#endif
#endif

#define TEMP_CALIBRATION_FILE_VERSION_1		1	// Initial version.
#define TEMP_CALIBRATION_FILE_VERSION_2		2	// Introduced 2/9/15. Bug in MJ_SensorCalibration.cpp whereby the 
												// zero motion threshold is set badly means that we should overwrite
												// all version 1 files.
namespace Baofeng
{
	namespace Mojing
	{

		GyroTempCalibration::GyroTempCalibration()
			: m_dLastPostTime(0)
		{
		}

		void GyroTempCalibration::Initialize(Parameters* pParameters)
		{
			m_pParameters = pParameters;
		}

		void GyroTempCalibration::GetAllTemperatureReports(Array<Array<TemperatureReport> >* tempReports)
		{
			TemperatureReport t;

			MojingGyroOffsetReportSet* pSet = m_pParameters->GetGyroOffsetReportSet();

			int iNumBins = pSet->GetNumBins();
			int iNumSamples = pSet->GetNumSamples();

			tempReports->Clear();
			tempReports->Resize(iNumBins);
			for (int i = 0; i < iNumBins; i++)
			{
				(*tempReports)[i].Resize(iNumSamples);
			}

			for (int i = 0; i < iNumBins; i++)
			{
				MojingGyroOffsetReport* entry = (*pSet)[i];
				for (int j = 0; j < iNumSamples; j++)
				{
					t.Bin = i;
					t.NumBins = iNumBins;
					t.Sample = j;
					t.NumSamples = iNumSamples;
					t.TargetTemperature = i * 5.0 + 15.0;

					t.ActualTemperature = entry[j].GetTemperature();
					t.Time = entry[j].GetTime();
					t.Offset = entry[j].GetOffset();
					(*tempReports)[t.Bin][t.Sample] = t;
				}
			}
		}

		void GyroTempCalibration::SetTemperatureReport(const TemperatureReport& tempReport)
		{
			MojingGyroOffsetReportSet* pSet = m_pParameters->GetGyroOffsetReportSet();
			//int iNumBins = pSet->GetNumBins();
			//int iNumSamples = pSet->GetNumSamples();

			MojingGyroOffsetReport* entry = (*pSet)[tempReport.Bin];
			entry[tempReport.Sample].SetTemperature(tempReport.ActualTemperature);
			entry[tempReport.Sample].SetOffset(tempReport.Offset);
			entry[tempReport.Sample].SetTime(tempReport.Time);
			pSet->Save();

			if (m_dLastPostTime <= 1 || (Timer::GetSeconds() - m_dLastPostTime) > (60 * 60))
			{
				m_dLastPostTime = Timer::GetSeconds();
				JSON * pJson = pSet->ToJson();
				char * pJsonValue = pJson->PrintValue(0, false);
				GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->SetGyroOffsetReportSet(pJsonValue);
				MJ_FREE(pJsonValue);
				pJson->Release();

				GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->Post();
			}
		}
	}
}
