#include "MojingMerchantVerify.h"
#include "../3rdPart/MD5/MD5.h"
#include "../Reporter/DatabaseInfoReporter.h"
#include "../Base/MojingTypes.h"
#include "../Profile/ProfileThreadMGR.h"
#include "../MojingSDKStatus.h"
#include "../MojingManager.h"

#ifdef MJ_OS_WIN32
#include "../3rdPart/sqlite3/sqlite3.h"
#else
#include <sqlite3.h>
#endif

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

//to be modify...
#define  SHOW_INVALID_INFO

#define  API_CHAR "0p9o8i7u"

namespace Baofeng
{
	namespace Mojing
	{
		extern Mutex *g_pSqlite3ExecLocker;
		// 注意：因为本对象可能被多次调用，所以建立一个全局对象，运行期间不释放
		MojingMerchantVerify *g_pMojingMerchantVerify = NULL;
		MojingMerchantVerify::MojingMerchantVerify()
		{
			SetAppID("UNKNOWN");
			SetAppKey("UNKNOWN");
			SetPackageName("UNKNOWN");
			SetMerchantID("UNKNOWN");
			SetClassName(__FUNCTION__);
		}

		MojingMerchantVerify::~MojingMerchantVerify()
		{

		}

		MojingMerchantVerify* MojingMerchantVerify::GetMojingMerchantVerify()
		{
			if (g_pMojingMerchantVerify == NULL)
				g_pMojingMerchantVerify = new MojingMerchantVerify;

			return g_pMojingMerchantVerify;
		}

		bool MojingMerchantVerify::IsNeedVerifyFromServer()
		{
			DatabaseInfoReporter *pInfoReporter = Manager::GetMojingManager()->GetReporter();
			if (pInfoReporter == NULL)
			{
				return true;
			}
			sqlite3* db = pInfoReporter->GetInfoDataBase();
			if (db == NULL)
			{
				return true;
			}
			char szSQL[1024];
			sprintf(szSQL, "SELECT result FROM tb_Verify WHERE pid = '%s' AND appkey = '%s'", GetAppID(), GetAppKey());
			char* errmsg = NULL;
			int nrow = 0;
			int ncolumn = 0;
			char ** azResult;
			if (g_pSqlite3ExecLocker == NULL)
			{
				g_pSqlite3ExecLocker = new Mutex;
			}
			if (g_pSqlite3ExecLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "MojingMerchantVerify::IsNeedVerifyFromServer g_pSqlite3ExecLocker invalid.");
				return true;
			}
			g_pSqlite3ExecLocker->DoLock();
			int iRet = sqlite3_get_table(db, szSQL, &azResult, &nrow, &ncolumn, &errmsg);
			sqlite3_free(errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK || nrow == 0)
			{
				sqlite3_free_table(azResult);
				return true;
			}
			
			iRet = atoi(azResult[1]);
			sqlite3_free_table(azResult);
			if (iRet)
			{
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "MojingMerchantVerify::IsNeedVerifyFromServer alrealy verify ok.");
#endif
				return false;
			}
			else
			{
#ifdef SHOW_INVALID_INFO
				MojingSDKStatus::GetSDKStatus()->SetVerfiyStatus(VERIFY_INVALID);
#endif
				return true;
			}
		}

		void MojingMerchantVerify::SaveVerifyResult(bool bVerifyOK)
		{
			DatabaseInfoReporter *pInfoReporter = Manager::GetMojingManager()->GetReporter();
			if (pInfoReporter == NULL)
			{
				return;
			}
			sqlite3* db = pInfoReporter->GetInfoDataBase();
			if (db == NULL)
			{
				return;
			}

			char szSQL[1024];
			sprintf(szSQL, "SELECT * FROM tb_Verify WHERE pid = '%s' AND appkey = '%s'", GetAppID(), GetAppKey());
			char* errmsg = NULL;
			int nrow = 0;
			int ncolumn = 0;
			char ** azResult;
			if (g_pSqlite3ExecLocker == NULL)
			{
				g_pSqlite3ExecLocker = new Mutex;
			}
			if (g_pSqlite3ExecLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "MojingMerchantVerify::SaveVerifyResult g_pSqlite3ExecLocker invalid.");
				return;
			}
			g_pSqlite3ExecLocker->DoLock();
			int iRet = sqlite3_get_table(db, szSQL, &azResult, &nrow, &ncolumn, &errmsg); 
			sqlite3_free_table(azResult);
			sqlite3_free(errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK || nrow == 0)
			{
				sprintf(szSQL, "INSERT INTO tb_Verify VALUES(?, '%s', '%s', '%s', '%s', %d)", GetAppID(), GetMerchantID(), GetAppKey(), GetPackageName(), bVerifyOK ? 1 : 0);
			}
			else
			{
				sprintf(szSQL, "UPDATE tb_Verify SET result = %d WHERE pid = '%s' AND appkey = '%s'", bVerifyOK ? 1 : 0, GetAppID(), GetAppKey());
			}

			g_pSqlite3ExecLocker->DoLock();
			iRet = sqlite3_exec(db, szSQL, NULL, NULL, &errmsg);
			if (iRet != SQLITE_OK)
			{
				MOJING_ERROR(g_APIlogger, "update table(tb_Verify) failed." << errmsg);
			}
			sqlite3_free(errmsg);
			g_pSqlite3ExecLocker->Unlock();
		}

		void MojingMerchantVerify::VerifyFromServer()
		{
			MD5 md5;
			String data = GetMerchantID();
			data += "&";
			data += GetPackageName();
			data += "&";
			data += GetAppID();
			data += "&";
			data += GetAppKey();
			data += API_CHAR;
			data += API_CHAR;

			size_t  buflen = data.GetLength() + 64;
			char * pMD5SrcBuffer = new char[buflen];
			strcpy(pMD5SrcBuffer, data.ToCStr());
			md5.update(pMD5SrcBuffer, strlen(pMD5SrcBuffer));
			data = md5.toString();

			JSON* joMsg = JSON::CreateObject();
			joMsg->AddStringItem("merchant_no", GetMerchantID());
			joMsg->AddStringItem("package_name", GetPackageName());
			joMsg->AddStringItem("app_appid", GetAppID());
			joMsg->AddStringItem("app_appkey", GetAppKey());
			joMsg->AddStringItem("open_verify", data.ToCStr());

			data = "open_id=";
			data += joMsg->PrintValue(0, false);
			delete[] pMD5SrcBuffer;

			ProfileThreadMGR::UpdateInternetProfile(GetClassName(), data, Profile_LOAD, InternetProfileCallBack, this);
		}

		void MojingMerchantVerify::InternetProfileCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			if (RespCode != 200)
            {
#ifdef _DEBUG
				MOJING_WARN(g_APIlogger, "MojingMerchantVerify, network is abnormal.");
#endif
				return;
            }
			char *pBuffer = new char[uiSize + 1];
			memcpy(pBuffer, lpszRespString, uiSize);
			pBuffer[uiSize] = 0;


			MojingMerchantVerify *pVerify = (MojingMerchantVerify *)pCallBackParam;
			JSON *pJson = JSON::Parse(pBuffer);

			delete[] pBuffer;

			if (pJson)
			{
				const char *pValueObjectName = "status";
				if (JSON *pValueObjectNode = pJson->GetItemByName(pValueObjectName))
				{
					if (pValueObjectNode->Type == JSON_Bool)
					{
						if (pValueObjectNode->GetBoolValue() == false)
						{
							MOJING_WARN(g_APIlogger, "MojingMerchantVerify, failed.");
#ifdef SHOW_INVALID_INFO
							MojingSDKStatus::GetSDKStatus()->SetVerfiyStatus(VERIFY_INVALID);
#endif
						}
						else
						{
							MOJING_TRACE(g_APIlogger, "MojingMerchantVerify, succeed");
#ifdef SHOW_INVALID_INFO
                            if (MojingSDKStatus::GetSDKStatus()->GetVerfiyStatus() == VERIFY_INVALID)
                                MojingSDKStatus::GetSDKStatus()->SetVerfiyStatus(VERIFY_OK);
#endif
						}
						pVerify->SaveVerifyResult(pValueObjectNode->GetBoolValue());
					}
				}
				delete pJson;
			}
		}

		void MojingMerchantVerify::AppVerify(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szPackageName)
		{
			SetAppID(szAppID);
			SetAppKey(szAppKey);
			SetPackageName(szPackageName);
			SetMerchantID(szMerchantID);

			if (IsNeedVerifyFromServer())
			{
				VerifyFromServer();
			}
		}
	}
}