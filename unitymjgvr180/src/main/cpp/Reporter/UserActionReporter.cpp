#include "UserActionReporter.h"
#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"
#include "../Platform/MojingPlatformBase.h"
#include "../3rdPart/MD5/MD5.h"
#include "../3rdPart/AES/AESPro.h"
#include "../Profile/InternetProfile.h"
#include "../Profile/ProfileThreadMGR.h"
#include "../Base/MojingTypes.h"

#include "../MojingSDKStatus.h"
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#ifdef _DEBUG
#define CHECK_CHAR "123456"
#else
#define CHECK_CHAR "!Q2w#E4r%T"
#endif

namespace Baofeng
{
	namespace Mojing
	{
		// 注意：因为本对象可能被多次调用，所以建立一个全局对象，运行期间不释放
		UserActionReporter *g_pUserActionReporter = NULL;
		UserActionReporter::UserActionReporter()
		{
			SetClassName(__FUNCTION__);
		}

		UserActionReporter::~UserActionReporter()
		{

		}
		UserActionReporter* UserActionReporter::GetUserActionReporter()
		{
			if (g_pUserActionReporter == NULL)
				g_pUserActionReporter = new UserActionReporter;

			return g_pUserActionReporter;
		}

	
		void UserActionReporter::Post(const char* strActionType, const char* strItemID, const char* strJsonValue)
		{
			MojingPlatformBase* pPlatform = MojingPlatformBase::GetPlatform();
			if (pPlatform == NULL)
			{
				return;
			}

			MD5 md5;

			String strUserID = ConvertToUrlEncode(pPlatform->GetUserID());
			String data = "userid=";
			data += strUserID.ToCStr();

			String ItemIDStr = ConvertToUrlEncode(strItemID);
			data += "&itemid=";
			data += ItemIDStr.ToCStr();

			String ActionTypeStr = ConvertToUrlEncode(strActionType);
			data += "&action_type=";
			data += ActionTypeStr.ToCStr();

			String JsonValueStr = ConvertToUrlEncode(strJsonValue);
			data += "&jsonvalue=";
			data += JsonValueStr.ToCStr();

			char szTime[256];
			sprintf(szTime, "%d", GetCurrentTime());
			data += "&time=";
			data += szTime;

			data += "&sign=";
			// MAKE MD5
			md5.reset();
			size_t  buflen = strlen(szTime) + strlen(strUserID.ToCStr()) + ItemIDStr.GetSize() + ActionTypeStr.GetSize() + 128;
			char * pMD5SrcBuffer = new char[buflen];
			strcpy(pMD5SrcBuffer, strUserID.ToCStr());
			strcat(pMD5SrcBuffer, ItemIDStr.ToCStr());
			strcat(pMD5SrcBuffer, ActionTypeStr.ToCStr());
			strcat(pMD5SrcBuffer, szTime);

			strcat(pMD5SrcBuffer, CHECK_CHAR);
			md5.update(pMD5SrcBuffer, strlen(pMD5SrcBuffer));
			data += md5.toString();
			delete[] pMD5SrcBuffer;
			// post by thread 
			ProfileThreadMGR::UpdateInternetProfile(GetClassName(), data, Profile_LOAD, InternetProfileCallBack, this);
		}	

		void UserActionReporter::InternetProfileCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			int valueCode = -1;
			if (RespCode == 200)
			{
				UserActionReporter *pRepoter = (UserActionReporter *)pCallBackParam;

				char *pBuffer = new char[uiSize + 1];
				memcpy(pBuffer, lpszRespString, uiSize);
				pBuffer[uiSize] = 0;
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "UserActionReporter::InternetProfileCallBack res str: " << pBuffer);				  
#endif // _DEBUG
				JSON *pJson = JSON::Parse(pBuffer);
				delete[] pBuffer;

				if (pJson)
				{
					if (JSON *pValueObjectNode = pJson->GetItemByName("data"))
					{
						if (JSON *pValueNode = pValueObjectNode->GetItemByName("code"))
						{
							if (pValueNode->Type == JSON_Number)
							{
								valueCode = pValueNode->GetInt32Value();
							}
							else if (pValueNode->Type == JSON_String)
							{
								valueCode = atoi(pValueNode->GetStringValue().ToCStr());
							}
						}
					}
					pJson->Release();
				}
			}
#ifdef _DEBUG
			if (valueCode == 0)
			{
				MOJING_TRACE(g_APIlogger, "Report user action succeed.");
			}
			else
			{
				MOJING_TRACE(g_APIlogger, "Report user action failed. RespCode: " << RespCode << " valueCode: " << valueCode);
			}				  
#endif // _DEBUG
		}
	}
}