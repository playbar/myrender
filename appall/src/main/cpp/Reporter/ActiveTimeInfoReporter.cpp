#include "ActiveTimeInfoReporter.h"

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
			
			char *pJsonValue = joMsg->PrintValue(0, false);
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
