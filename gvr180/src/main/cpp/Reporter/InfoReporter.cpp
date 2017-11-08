#include "InfoReporter.h"
#include "../Profile/ProfileThreadMGR.h"

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

namespace Baofeng
{
	namespace Mojing
	{
		extern Mutex *g_pSqlite3ExecLocker;
		InfoReporter::InfoReporter()
		{
			SetReportMsg("");
			SetMessageType(MSGTYPE_ACTIVE);
			SetClassName(__FUNCTION__);
		}

		//run in other thread ?
		void InfoReporter::SaveToDB()
		{
			//MOJING_FUNC_TRACE(g_APIlogger);
			DatabaseInfoReporter* pReporter = Manager::GetMojingManager()->GetReporter();
			MojingPlatformBase* pPlatform = MojingPlatformBase::GetPlatform();
			if (pReporter == NULL || pPlatform == NULL)
				return;
			sqlite3* db = pReporter->GetInfoDataBase();
			if (db == NULL)
			{
				MOJING_ERROR(g_APIlogger, "Can not find database.");
				return;
			}
			String sql = "INSERT INTO tb_Info VALUES(?, ";
#if defined(MJ_OS_ANDROID)
			if (GetMessageType() == MSGTYPE_ACTIVE)
				sql += "'mjsdk_active_android'";
			else
				sql += "'mjsdk_error_android'";
#elif defined(MJ_OS_MAC)
			if (GetMessageType() == MSGTYPE_ACTIVE)
				sql += "'mjsdk_active_ios'";
			else
				sql += "'mjsdk_error_ios'";
#endif
			sql += ", '";
			sql += pPlatform->GetAppID();
			sql += "', '";
			sql += pPlatform->GetUserID();
			sql += "', '";
			sql += pPlatform->GetRunID();
			sql += "', '";
			sql += ConvertToUrlEncode(pPlatform->GetAppName());
			sql += "', '";
			sql += ConvertToUrlEncode(pPlatform->GetChannelID());
			sql += "',";
			char buf[32];
			sprintf(buf, "%d", GetReportType());
			sql += buf;
			sql += ", '";
			sql += ConvertToUrlEncode(GetReportMsg());
			sql += "')";
			

			if (g_pSqlite3ExecLocker == NULL)
			{
				g_pSqlite3ExecLocker = new Mutex();
			}
			if (g_pSqlite3ExecLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "InfoReporter::SaveToDB g_pSqlite3ExecLocker invalid.");
				return;
			}
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "InfoReporter::SaveToDB sql =" << sql);
#endif
			char* errmsg = NULL;
			g_pSqlite3ExecLocker->DoLock();
			int iRet = sqlite3_exec(db, sql.ToCStr(), NULL, NULL, &errmsg);
			if (iRet != SQLITE_OK)
			{
				MOJING_ERROR(g_APIlogger, "Insert to table(tb_Info) failed:" << errmsg);
			}
			sqlite3_free(errmsg);
			g_pSqlite3ExecLocker->Unlock();
		}

		void InfoReporter::Post()
		{
			MojingPlatformBase* pPlatform = MojingPlatformBase::GetPlatform();
			if (pPlatform == NULL)
				return;
			String data = "ltype=";
#ifdef MJ_OS_ANDROID
			if (GetMessageType() == MSGTYPE_ACTIVE)
				data += "mjsdk_active_android";
			else
				data += "mjsdk_error_android";
#else
			if (GetMessageType() == MSGTYPE_ACTIVE)
				data += "mjsdk_active_ios";
			else
				data += "mjsdk_error_ios";
#endif
			char buf[32];
			data += "&pid=";
#ifdef REPORTER_URL_ENCRYPT 
			data += BaofengEnc(pPlatform->GetAppID());
			data += "&uid=";
			data += BaofengEnc(pPlatform->GetUserID());
			data += "&rid=";
			data += BaofengEnc(pPlatform->GetRunID());
			data += "&ver=";
			data += BaofengEnc(pPlatform->GetAppName());
			data += "&sid=";
			data += BaofengEnc(pPlatform->GetChannelID());
			data += "&type=";
			sprintf(buf, "%d", GetReportType());
			data += BaofengEnc(buf);
			data += "&msg=";
			data += BaofengEnc(GetReportMsg());
			data += "&enc=1";
#else
			data += pPlatform->GetAppID(); 
			data += "&uid=";
			data += pPlatform->GetUserID();
			data += "&rid=";
			data += pPlatform->GetRunID();
			data += "&ver=";
			data += ConvertToUrlEncode(pPlatform->GetAppName());
			data += "&sid=";
			data += ConvertToUrlEncode(pPlatform->GetChannelID());
			data += "&type=";
			sprintf(buf, "%d", GetReportType());
			data += buf;
			data += "&msg=";
			data += ConvertToUrlEncode(GetReportMsg());
#endif

			// post by thread 
			ProfileThreadMGR::UpdateInternetProfile(GetClassName(), data, Profile_SAVE, InternetInfoPostCallBack, this);
		}

		void InfoReporter::InternetInfoPostCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			if (RespCode != 200)
			{
				MOJING_TRACE(g_APIlogger, "InfoReporter post msg failed. errcode:" << RespCode);
				InfoReporter* pReporter = (InfoReporter *)pCallBackParam;
				if (pReporter)
				{
					pReporter->SaveToDB();
				}
			}
		}
	}
}
