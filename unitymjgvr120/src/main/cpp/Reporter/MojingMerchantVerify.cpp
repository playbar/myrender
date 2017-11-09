#include "MojingMerchantVerify.h"
#include "../3rdPart/MD5/MD5.h"
#include "../Reporter/DatabaseInfoReporter.h"
#include "../Base/MojingTypes.h"
#include "../Base/Base64.h"
#include "../Profile/ProfileThreadMGR.h"
#include "../MojingSDKStatus.h"
#include "../MojingManager.h"
#include "../Base/MojingJSON.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

//to be modify...
#ifndef _DEBUG
#define  SHOW_INVALID_INFO
#endif

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
				pJson->Release();
			}// end of if (pJson)
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
		
		void MojingMerchantVerify::AppCheckPackage(const char* szAppName, const char* szCaseCode)
		{
			if (szAppName == NULL || szCaseCode == NULL)
			{
				return;
			}
			if (strcmp(szAppName, "UNKNOWN") == 0 || strcmp(szCaseCode, "UNKNOWN") == 0)
			{
				return;
			}

			String szPackageName = "";

			szPackageName = GetPackageNameFromCaseCode(szAppName, szCaseCode);

			if (szPackageName.GetLength() == 0)
			{
				MOJING_WARN(g_APIlogger, "AppCheckPackage, failed. Case code is wrong.");
				MojingSDKStatus::GetSDKStatus()->SetVerfiyStatus(VERIFY_PACKAGE_INVALID);
			}
			else
			{
				if (strcmp(szPackageName, GetPackageName()) != 0)
				{
					MOJING_WARN(g_APIlogger, "AppCheckPackage, failed. Package is invalid.");
					MojingSDKStatus::GetSDKStatus()->SetVerfiyStatus(VERIFY_PACKAGE_INVALID);
				}				
#ifdef _DEBUG
				else
				{
					MOJING_TRACE(g_APIlogger, "MojingMerchantVerify::AppCheckPackage alrealy verify ok.");
				}
#endif // DEBUG

			}
		}

		bool MojingMerchantVerify::AESKeyInit(const char* szAppName)
		{
			if (szAppName == NULL)
				return false;
			int appNameLength = strlen(szAppName);
			if (appNameLength > 0)
			{
				unsigned char* applicationData = new unsigned char[appNameLength + 1];
				applicationData[appNameLength] = '\0';
				memcpy(applicationData, szAppName, appNameLength);

				int keyDataLength = appNameLength < 8 ? 8 : appNameLength;
				char* keyData = new char[keyDataLength + 1];
				keyData[keyDataLength] = 0;
				char* chainData = new char[keyDataLength + 1];
				chainData[keyDataLength] = 0;

				int keyLength = 0;
				int chainLength = 0;
				for (int i = 0; i < appNameLength; i++) {
					if (i % 2 == 0)
					{
						keyData[keyLength] = applicationData[i];
						keyLength++;
					}
					if (i % 2 == 1)
					{
						chainData[chainLength] = applicationData[i];
						chainLength++;
					}
				}
				int append = 0;
				while (keyLength < 8)
				{
					keyData[keyLength++] = keyData[append++];
				}
				append = 0;
				while (chainLength < 8)
				{
					chainData[chainLength++] = chainData[append++];
				}


				int aesKeyLength = keyLength + strlen(MOJING_KEY);
				int aesChainLength = chainLength + strlen(MOJING_CHAIN);

				unsigned char* key = new unsigned char[aesKeyLength + 1];
				unsigned char* chain = new unsigned char[aesChainLength + 1];

				memset(key, 0, aesKeyLength + 1);
				memcpy(key, MOJING_KEY, strlen(MOJING_KEY));
				memcpy(key + strlen(MOJING_KEY), keyData, keyLength);

				memset(chain, 0, aesChainLength + 1);
				memcpy(chain, chainData, chainLength);
				memcpy(chain + chainLength, MOJING_CHAIN, strlen(MOJING_CHAIN));

				byte KeyMD5[16], ChainMD5[16];
				MD5 MD5Key, MD5Chain;
				MD5Key.update((const void*)key, aesKeyLength);
				MD5Chain.update((const void*)chain, aesChainLength);

				memcpy(KeyMD5, MD5Key.digest(), 16);
				memcpy(ChainMD5, MD5Chain.digest(), 16);
				

				m_cAesProHandle.MakeKey((const char *)KeyMD5, (const char *)ChainMD5);
				delete[] applicationData;
				delete[] key;
				delete[] chain;
				delete[] keyData;
				delete[] chainData;
				return true;
			}
			return false;
		}

		String MojingMerchantVerify::AESDecrypt(const char* szCaseCode)
		{
			String sRet = "";
			if (szCaseCode == NULL)
				return sRet;
			Base64 *base = new Base64();
			int length = strlen(szCaseCode) * 3 / 4 + 1;
			char* decode = new char[length + 1];
			memset(decode, 0, length + 1);
			int decodeLength = base->Decode(szCaseCode, decode);
			if (decodeLength <= length)
			{
				char *pDecBuffer = new char[length + 16];
				memset(pDecBuffer, 0, length + 16);
				m_cAesProHandle.Decrypt(decode, pDecBuffer, length, m_cAesProHandle.CBC);
				sRet = pDecBuffer;
				delete[] pDecBuffer;		
			}
			else
			{
				//解析Base64错误
				MOJING_ERROR(g_APIlogger, "MojingMerchantVerify::AESDecrypt decode base64 error");
			}
			delete[] decode;
			delete base;
			return sRet;
		}

		String MojingMerchantVerify::GetPackageNameFromCaseCode(const char* szAppName, const char* szCaseCode)
		{
			String sRet = "";
			if (AESKeyInit(szAppName))
			{
				String decode = AESDecrypt(szCaseCode);
				JSON *pJson = JSON::Parse(decode);
				if (pJson)
				{
					JSON *pPackageNameJson = pJson->GetItemByName(MOJING_PACKAGE_NAME_JSON_NODE);
					sRet = pPackageNameJson->GetStringValue();
				}
			}
			return sRet;
		}
	}
}