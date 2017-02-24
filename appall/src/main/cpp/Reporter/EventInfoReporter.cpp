#include "EventInfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		EventInfoReporter::EventInfoReporter()
		{
		}

		void EventInfoReporter::CreateReportMsg(const char* szEventName, const char* szEventChannelID,
			const char* szEventInName, float eInData, const char* szEventOutName, float eOutData, int64_t date, bool pd)
		{
			JSON* joMsg = JSON::CreateObject();

			joMsg->AddNumberItem("date", date);
			joMsg->AddStringItem("eid", szEventName);
			joMsg->AddStringItem("ein", szEventInName);
			joMsg->AddNumberItem("indata", eInData);
			joMsg->AddStringItem("eon", szEventOutName);
			joMsg->AddNumberItem("outdata", eOutData);
			joMsg->AddStringItem("sid", szEventChannelID);
			joMsg->AddBoolItem("pd", pd);
			char *pJsonValue = joMsg->PrintValue(0, false);
			SetReportMsg(pJsonValue);
			MJ_FREE(pJsonValue);

			SetReportType(4);
		}

		void EventInfoReporter::PostMessage(const char* szEventName, const char* szEventChannelID,
			const char* szEventInName, float eInData, const char* szEventOutName, float eOutData, int64_t date, bool pd)
		{
			CreateReportMsg(szEventName, szEventChannelID, szEventInName, eInData, szEventOutName, eOutData, date, pd);

			Post();
		}

		void EventInfoReporter::SaveMessage(const char* szEventName, const char* szEventChannelID,
			const char* szEventInName, float eInData, const char* szEventOutName, float eOutData, int64_t date, bool pd)
		{
			//MOJING_FUNC_TRACE(g_APIlogger);
			CreateReportMsg(szEventName, szEventChannelID, szEventInName, eInData, szEventOutName, eOutData, date, pd);

			SaveToDB();
		}
	}
}
