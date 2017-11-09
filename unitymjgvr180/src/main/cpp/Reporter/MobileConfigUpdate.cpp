#include "MobileConfigUpdate.h"
#include "../Base/MojingString.h"
#include "../Base/MojingJSON.h"
#include "../Base/MojingFile.h"
#include "../Base/MojingTimer.h"
#include "../3rdPart/MD5/MD5.h"
#include "../Platform/MojingPlatformBase.h"
#include "../Profile/MobileConfigProfile.h"
#include "../Profile/ProfileThreadMGR.h"
#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDeviceParameters.h"


namespace Baofeng
{
	namespace Mojing
	{
		extern unsigned char g_EncKey[16];
		MobileConfigUpdate::MobileConfigUpdate()
		{
			SetClassName(__FUNCTION__);
		}


		MobileConfigUpdate::~MobileConfigUpdate()
		{
		}

		MobileConfigUpdate* MobileConfigUpdate::GetMobileConfigUpdate()
		{
			return new MobileConfigUpdate;
		}

		void MobileConfigUpdate::UpdateConfig()
		{
			double dLastCheckMobileConfigTime = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->GetCheckMobileConfig();
			//request once per day.
			if (fabs(GetCurrentTime() - dLastCheckMobileConfigTime) < CHECK_MOBILECONFIG_INTERVEL)
			{
				return;
			}
			String data = "";
			MD5 md5;
			char szTime[256];

			data += "mobile=";
			String strMobile = AES_Value_S(Manager::GetMojingManager()->GetParameters()->GetDeviceParameters()->GetModel());
			data += strMobile;

			data += "&releasedate=";
			String releaseData = MojingPlatformBase::GetPlatform()->GetMobileConfigVersion();
			if (releaseData == "")
			{
				releaseData = "20150101";
			}
			String strReleaseDate = AES_Value_S(releaseData);
			data += strReleaseDate;

			sprintf(szTime, "%d", GetCurrentTime());
			data += "&curr_time=";
			data += szTime;
			data += "&sign=";
			size_t  buflen = strMobile.GetSize() + strReleaseDate.GetSize() + strlen(szTime) + 64;
			char * pMD5SrcBuffer = new char[buflen];
			strcpy(pMD5SrcBuffer, strMobile.ToCStr());
			strcat(pMD5SrcBuffer, strReleaseDate.ToCStr());
			strcat(pMD5SrcBuffer, szTime);
			strcat(pMD5SrcBuffer, "41f836e3d488337eeb49b7f6e87175db");
			md5.update(pMD5SrcBuffer, strlen(pMD5SrcBuffer));
			data += md5.toString();
			delete[] pMD5SrcBuffer;

			ProfileThreadMGR::UpdateInternetProfile(GetClassName(), data, Profile_LOAD, InternetProfileCallBack, this);
		}
		void MobileConfigUpdate::InternetProfileCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			if (RespCode != 200)
				return;
			char *pBuffer = new char[uiSize + 1];
			memcpy(pBuffer, lpszRespString, uiSize);
			pBuffer[uiSize] = 0;

			Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->SetCheckMobileConfig(GetCurrentTime());
			Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->Save();

			MobileConfigUpdate *pUpdateObj = (MobileConfigUpdate *)pCallBackParam;
			JSON *pJson = JSON::Parse(pBuffer);

			delete[] pBuffer;
			//MOJING_TRACE(g_APIlogger, "MobileConfigUpdate::InternetProfileCallBack, lpszRespString:" << lpszRespString);

			if (pJson)
			{
				if (JSON *pResultNode = pJson->GetItemByName("Result"))
				{
					if (pResultNode->GetStringValue().CompareNoCase("SUCCEED") == 0)
					{
						MobileConfigProfile::UpdateFromProfile(MojingPlatformBase::GetPlatform()->GetPacketProfilePath(), pJson);
					}
				}
			}

			delete pUpdateObj;
		}
	}
}
