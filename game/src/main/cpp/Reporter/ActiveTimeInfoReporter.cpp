#include "ActiveTimeInfoReporter.h"
#include "GyroTempCalibrationReporter.h"

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

namespace Baofeng
{
	namespace Mojing
	{
		ActiveTimeInfoReporter::ActiveTimeInfoReporter()
		{
		}

		void ActiveTimeInfoReporter::CreateReportMsg()
		{
			MojingPlatformBase* pPlatform = MojingPlatformBase::GetPlatform();
			DatabaseInfoReporter* pReporter = Manager::GetMojingManager()->GetReporter();
			if (pReporter == NULL || pPlatform == NULL)
			{
				return;
			}

			JSON* joMsg = JSON::CreateObject();
			joMsg->AddStringItem("an", pPlatform->GetPackageName());
			joMsg->AddNumberItem("date", pReporter->GetCurrentRunTime());
			joMsg->AddNumberItem("time", pReporter->GetCurrentActiveTime());
			joMsg->AddNumberItem("exitdate", pReporter->GetCurrentExitTime());
			const char* szGlasseeName = GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->GetGlassesReportName();
			if (strlen(szGlasseeName) == 0 || strcmp(szGlasseeName, "UNKNOWN") == 0)
			{
				joMsg->AddStringItem("myglasses", "noglass");
			}
			else
			{
				const char* p = strstr(szGlasseeName, "M=");
				if (p == NULL)
				{
					joMsg->AddStringItem("myglasses", szGlasseeName);
				}
				else
				{
					joMsg->AddStringItem("myglasses", p);
				}
			}
			const char* szMojingSN = GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()->GetMojingSN();
			if (szMojingSN == NULL || strlen(szMojingSN) == 0)
			{
				//joMsg->AddStringItem("glassessn", "");
			}
			else
			{
				joMsg->AddStringItem("glassessn", szMojingSN);
			}
			
			char *pJsonValue = joMsg->PrintValue(0, false);
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "ActiveTimeInfoReporter::SaveToDB json = " << pJsonValue);
#endif
			SetReportMsg(pJsonValue);
			MJ_FREE(pJsonValue);

			SetReportType(2);
		}

		void ActiveTimeInfoReporter::PostMessage()
		{
			CreateReportMsg();

			Post();
		}

		void ActiveTimeInfoReporter::SaveMessage()
		{
			//MOJING_FUNC_TRACE(g_APIlogger);
			CreateReportMsg();

			SaveToDB();
		}
	}
}
