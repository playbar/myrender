#include "PageInfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		PageInfoReporter::PageInfoReporter()
		{
		}

		void PageInfoReporter::CreateReportMsg(String pageName, PageInfo info)
		{
			JSON* joMsg = JSON::CreateObject();
			joMsg->AddNumberItem("date", info.date); 
			joMsg->AddNumberItem("count", info.count);
			int nActiveTime = 0;
			if (info.exitdate != 0)
			{
				nActiveTime = int(info.exitdate - info.date);
				joMsg->AddNumberItem("time", nActiveTime);
			}
			joMsg->AddNumberItem("exitdate", info.exitdate);
			joMsg->AddStringItem("eid", pageName);

			char *pJsonValue = joMsg->PrintValue(0, false);
			SetReportMsg(pJsonValue);
			MJ_FREE(pJsonValue);

			SetReportType(3);
		}

		void PageInfoReporter::PostMessage(String pageName, PageInfo info)
		{
			CreateReportMsg(pageName, info);

			Post();
		}

		void PageInfoReporter::SaveMessage(String pageName, PageInfo info)
		{
			//MOJING_FUNC_TRACE(g_APIlogger);
			CreateReportMsg(pageName, info);

			SaveToDB();
		}
	}
}
