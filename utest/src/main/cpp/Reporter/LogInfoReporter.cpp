#include "LogInfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		LogInfoReporter::LogInfoReporter()
		{
			SetMessageType(MSGTYPE_ERROR);
		}

		void LogInfoReporter::CreateReportMsg(int iLogType, const char* szTypeName, const char* szLogContent, int64_t date, bool pd)
		{
			JSON* joMsg = JSON::CreateObject();

			joMsg->AddNumberItem("date", date);
			joMsg->AddNumberItem("eid", iLogType);
			joMsg->AddStringItem("tname", szTypeName);
			joMsg->AddStringItem("content", szLogContent);
			joMsg->AddBoolItem("pd", pd);
			char *pJsonValue = joMsg->PrintValue(0, false);
			SetReportMsg(pJsonValue);
			MJ_FREE(pJsonValue);

			SetReportType(5);
		}

		void LogInfoReporter::PostMessage(int iLogType, const char* szTypeName, const char* szLogContent, int64_t date, bool pd)
		{
			CreateReportMsg(iLogType, szTypeName, szLogContent, date, pd);

			Post();
		}

		void LogInfoReporter::SaveMessage(int iLogType, const char* szTypeName, const char* szLogContent, int64_t date, bool pd)
		{
			//MOJING_FUNC_TRACE(g_APIlogger);
			CreateReportMsg(iLogType, szTypeName, szLogContent, date, pd);

			SaveToDB();
		}
	}
}
