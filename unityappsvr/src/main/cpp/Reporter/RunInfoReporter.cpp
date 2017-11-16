#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDeviceParameters.h"
#include "../MojingSDKStatus.h"
#include "../MojingManager.h"
#include "GyroTempCalibrationReporter.h"
#include "RunInfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		RunInfoReporter::RunInfoReporter()
		{
		}

		void RunInfoReporter::CreateReportMsg()
		{
			MojingPlatformBase* platformBase = MojingPlatformBase::GetPlatform();
			DatabaseInfoReporter* pReporter = Manager::GetMojingManager()->GetReporter();
			if (pReporter == NULL || platformBase == NULL)
			{
				return;
			}
		
			JSON* joMsg = JSON::CreateObject();
			joMsg->AddStringItem("an", platformBase->GetPackageName());
			//joMsg->AddStringItem("ver", platformBase->GetAppName());
			//joMsg->AddStringItem("sid", platformBase->GetChannelID());
			joMsg->AddStringItem("sdkver", MojingSDKStatus::GetSDKStatus()->GetSDKVersion());
			joMsg->AddNumberItem("date", pReporter->GetCurrentRunTime());
			joMsg->AddNumberItem("ldate", pReporter->GetLastRunTime());
			joMsg->AddNumberItem("count", pReporter->GetRunCount());

			GyroTempCalibrationReporter* gyroReporter = GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter();
			if (gyroReporter != NULL)
			{
				joMsg->AddStringItem("brand", gyroReporter->GetBrand());
				joMsg->AddStringItem("mobile", gyroReporter->GetMobile());
				joMsg->AddStringItem("serial", gyroReporter->GetSerial());
				joMsg->AddStringItem("gles", gyroReporter->GetGLES());
				joMsg->AddStringItem("egl", gyroReporter->GetEGL());
			}

			//add display info
			JSON* joDisplay = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters()->ToJson();
			if (joDisplay)
			{
				/*
				char *pJsonValue = joDisplay->PrintValue(0, false);
				joMsg->AddStringItem("display", pJsonValue);
				MJ_FREE(pJsonValue);
				delete joDisplay;
				*/
				joMsg->AddItem("display", joDisplay);
			}
				
			joMsg->AddStringItem("cpu", platformBase->GetCPUName());
			joMsg->AddStringItem("iid", pReporter->GetInstallID());
			joMsg->AddStringItem("os", platformBase->GetOSString());
			//joMsg->Save("/sdcard/MojingSDK/RunInfo.txt", 0);

			char *pJsonValue = joMsg->PrintValue(0, false);
			SetReportMsg(pJsonValue);
			MJ_FREE(pJsonValue);

			SetReportType(1);
		}

		void RunInfoReporter::PostMessage()
		{
			//MOJING_FUNC_TRACE(g_APIlogger);
			CreateReportMsg();

			Post();
		}
	}
}
