#include "../Profile/ProfileThreadMGR.h"
#include "ActiveTimeInfoReporter.h"
#include "EventInfoReporter.h"
#include "PageInfoReporter.h"
#include "RunInfoReporter.h"
#include "LogInfoReporter.h"
#include "DatabaseInfoReporter.h"

#include <unistd.h>
#ifdef MJ_OS_MAC
#include <sys/time.h>
#elif defined(MJ_OS_ANDROID)
#include <android/log.h>
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#define INTERNAL_DB_VERSION		1	
#define REPORT_NEXTRUN			0

namespace Baofeng
{
	namespace Mojing
	{
		Mutex *g_pSqlite3ExecLocker = NULL;

		DatabaseInfoReporter::DatabaseInfoReporter()
			:m_bResumeInvoked(false)
			, m_bNewRun(true)
		{
			SetClassName(__FUNCTION__);
			SetInstallID("UNKNOWN");
			SetInfoDataBase(NULL);
			SetRunCount(0);
			SetCurrentRunTime(0);
			SetCurrentExitTime(0);
			SetLastRunTime(0);
			SetContinueInterval(30);
			SetReportInterval(120);
			SetPostWait(false);
			SetReportImmediate(false);
			SetPlatformBase(MojingPlatformBase::GetPlatform());
			m_PageInfo.clear();
			m_RunReporter = new RunInfoReporter();
			m_ActiveReporter = new ActiveTimeInfoReporter();
			m_PageReporter = new PageInfoReporter();
			m_EventReporter = new EventInfoReporter();
			m_LogReporter = new LogInfoReporter();
		}

		DatabaseInfoReporter::~DatabaseInfoReporter()
		{
			if (m_RunReporter)
			{
				delete m_RunReporter;
				m_RunReporter = NULL;
			}
			if (m_EventReporter)
			{
				delete m_EventReporter;
				m_EventReporter = NULL;
			}
			if (m_ActiveReporter)
			{
				delete m_ActiveReporter;
				m_ActiveReporter = NULL;
			}
			if (m_PageReporter)
			{
				delete m_PageReporter;
				m_PageReporter = NULL;
			}
			AppExit();
		}

		void DatabaseInfoReporter::InternetInfoPostCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			if (RespCode == 200)
			{
				DatabaseInfoReporter* pReporter = (DatabaseInfoReporter *)pCallBackParam;
				if (pReporter == NULL || lpszRespString == NULL)
				{
					return;
				}
				sqlite3* db = pReporter->GetInfoDataBase();
				uiSize = strlen((const char *)lpszRespString);
				char *pBuffer = new char[uiSize + 1];
				memcpy(pBuffer, lpszRespString, uiSize);
				pBuffer[uiSize] = 0;

				int iRecordID = atoi(pBuffer);
				char szSQL[256];
				char* errmsg = NULL;
				if (g_pSqlite3ExecLocker == NULL)
				{
					g_pSqlite3ExecLocker = new Mutex();
				}
				if (g_pSqlite3ExecLocker == NULL)
				{
					MOJING_ERROR(g_APIlogger, "DatabaseInfoReporter::InternetInfoPostCallBack g_pSqlite3ExecLocker invalid.");
					return;
				}
				g_pSqlite3ExecLocker->DoLock();
				sprintf(szSQL, "DELETE FROM tb_Info WHERE id = %d", iRecordID);
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "InternetInfoPostCallBack delete before. sql = " << szSQL);
#endif
				int iRet = sqlite3_exec(db, szSQL, NULL, NULL, &errmsg);
				if (iRet != SQLITE_OK)
				{
					MOJING_ERROR(g_APIlogger, "Delete from table(tb_Info) failed:" << errmsg << " sql = " << szSQL);
				}
				//MOJING_TRACE(g_APIlogger, "InternetInfoPostCallBack delete ok. sql = " << szSQL);
				if (errmsg)
					sqlite3_free(errmsg);

				g_pSqlite3ExecLocker->Unlock();
				delete[] pBuffer;
			}
			else
			{
				MOJING_ERROR(g_APIlogger, "DatabaseInfoReporter post msg failed. errcode:" << RespCode);
			}
		}

		int DatabaseInfoReporter::Get_InfoResult_callback(void* data, int n_columns, char** column_values, char** column_names)
		{
			//MOJING_FUNC_TRACE(g_APIlogger);
			DatabaseInfoReporter *pReporter = (DatabaseInfoReporter *)data;
			if (pReporter == NULL)
			{
				return -1;
			}

			if (n_columns != 9)
			{
				MOJING_ERROR(g_APIlogger, "Query result is not invalid, column = " << n_columns);
				return -2;
			}

			String paramData;
			for (int i = 1; i < n_columns; i++)
			{
				if (i != 1)
				{
					paramData += "&";
				}
				paramData += column_names[i];
				paramData += "=";
				if (i > 1)
				{
					paramData += BaofengEncEx(column_values[i]);
				}
				else
				{
					paramData += column_values[i];
				}
			}
#ifdef REPORTER_URL_ENCRYPT
			paramData += "&enc=1";
#endif

			// post by thread 
			ProfileThreadMGR::UpdateInternetProfile(pReporter->GetClassName(), paramData, Profile_SAVE, InternetInfoPostCallBack, pReporter, column_values[0], pReporter->GetPostWait());

			return 0;
		}

		void DatabaseInfoReporter::ReportMsgFromDB()
		{
			//MOJING_FUNC_TRACE(g_APIlogger);
			//m_vRecordID.clear();
			sqlite3* db = GetInfoDataBase();
			if (db == NULL)
			{
				MOJING_ERROR(g_APIlogger, "Can not find database.");
				return;
			}

			char* errmsg = NULL;
			if (g_pSqlite3ExecLocker == NULL)
			{
				g_pSqlite3ExecLocker = new Mutex();
			}
			if (g_pSqlite3ExecLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "DatabaseInfoReporter::ReportMsgFromDB g_pSqlite3ExecLocker invalid.");
				return;
			}
			g_pSqlite3ExecLocker->DoLock();
			String sql = "SELECT * FROM tb_Info";
			int iRet = sqlite3_exec(db, sql.ToCStr(), Get_InfoResult_callback, this, &errmsg);
			if (iRet != SQLITE_OK)
			{
				MOJING_ERROR(g_APIlogger, "Query from table(tb_Info) failed:" << errmsg);
			}
			sqlite3_free(errmsg);
			g_pSqlite3ExecLocker->Unlock();
		}

		static void CheckInfoDatabase(String dbPath, sqlite3** db)
		{
			char szSQL[256];
			char* errmsg = NULL;
			bool bDBExist = false;
			bool bNeedInsertVersion = true;
			String tempPath = dbPath + "-journal";
			if (access(tempPath.ToCStr(), F_OK) == 0)
			{
				MOJING_WARN(g_APIlogger, "CheckInfoDatabase need delete info database.");
				remove(tempPath.ToCStr());
				remove(dbPath.ToCStr());
			}

			if (access(dbPath.ToCStr(), F_OK) == 0)
			{
				bDBExist = true;
			}

			if (g_pSqlite3ExecLocker == NULL)
			{
				g_pSqlite3ExecLocker = new Mutex;
			}
			if (g_pSqlite3ExecLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "DatabaseInfoReporter::CheckInfoDatabase g_pSqlite3ExecLocker invalid.");
				return;
			}
			//if not exists, create database
			int iRet = sqlite3_open(dbPath.ToCStr(), db);
			if (iRet != SQLITE_OK || (*db) == NULL)
			{
				MOJING_ERROR(g_APIlogger, "Open database failed." << sqlite3_errmsg(*db));
				return;
			}

			//check db version
			if (bDBExist)
			{
				int nDBVersion = 0;
				char* errmsg = NULL;
				int nrow = 0;
				int ncolumn = 0;
				char ** azResult;
				sprintf(szSQL, "SELECT * FROM tb_Version");
				g_pSqlite3ExecLocker->DoLock();
				int iRet = sqlite3_get_table(*db, szSQL, &azResult, &nrow, &ncolumn, &errmsg);
				sqlite3_free(errmsg);
				g_pSqlite3ExecLocker->Unlock();
				if (iRet == SQLITE_OK && nrow > 0)
				{
					if (azResult[3] != NULL)
					{
						nDBVersion = atoi(azResult[3]);
						bNeedInsertVersion = false;
					}

				}
				sqlite3_free_table(azResult);
				if (nDBVersion != INTERNAL_DB_VERSION)
				{
					//create a new db
					sqlite3_close(*db);
					remove(dbPath.ToCStr());
					int iRet = sqlite3_open(dbPath.ToCStr(), db);
					if (iRet != SQLITE_OK || db == NULL)
					{
						MOJING_ERROR(g_APIlogger, "Create database failed." << sqlite3_errmsg(*db));
						return;
					}
					bNeedInsertVersion = true;
				}
			}

			if (bNeedInsertVersion)
			{
				g_pSqlite3ExecLocker->DoLock();
				String sql = "CREATE TABLE IF NOT EXISTS tb_Version (id INTEGER PRIMARY KEY, dbver INTEGER)";
				iRet = sqlite3_exec(*db, sql.ToCStr(), NULL, NULL, &errmsg);
				g_pSqlite3ExecLocker->Unlock();
				if (iRet != SQLITE_OK)
				{
					MOJING_ERROR(g_APIlogger, "Create table(tb_Version) failed: " << errmsg);
					sqlite3_free(errmsg);
					return;
				}
				sqlite3_free(errmsg);

				sprintf(szSQL, "INSERT INTO tb_Version VALUES(0, %d)", INTERNAL_DB_VERSION);
				g_pSqlite3ExecLocker->DoLock();
				iRet = sqlite3_exec(*db, szSQL, NULL, NULL, &errmsg);
				if (iRet != SQLITE_OK)
				{
					MOJING_ERROR(g_APIlogger, "Insert into table(tb_Version) failed: " << errmsg);
				}
				g_pSqlite3ExecLocker->Unlock();
				sqlite3_free(errmsg);
			}
		}

		sqlite3* DatabaseInfoReporter::OpenInfoDatabase()
		{
			sqlite3* db = NULL;
#ifdef MJ_OS_ANDROID
			__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "DatabaseInfoReporter::OpenInfoDatabase\n");
			String dbPath = GetPlatformBase()->GetPacketProfilePath();
#else
			String dbPath = GetPlatformBase()->GetDefaultLocalProfilePath();
#endif // MJ_OS_ANDROID
			dbPath += "/info.db";

			CheckInfoDatabase(dbPath, &db);

			char* errmsg = NULL;  //VARCHAR(100)
			if (g_pSqlite3ExecLocker == NULL)
			{
				g_pSqlite3ExecLocker = new Mutex();
			}
			if (g_pSqlite3ExecLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "DatabaseInfoReporter::OpenInfoDatabase g_pSqlite3ExecLocker invalid.");
#ifdef MJ_OS_ANDROID
				__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "DatabaseInfoReporter::OpenInfoDatabase g_pSqlite3ExecLocker invalid.");
#endif
				return NULL;
			}
			g_pSqlite3ExecLocker->DoLock();
			String sql = "CREATE TABLE IF NOT EXISTS tb_Info (id INTEGER PRIMARY KEY, ltype VARCHAR(20), pid VARCHAR(100), uid VARCHAR(100), rid VARCHAR(100), ver VARCHAR(256), sid VARCHAR(256), type INTEGER, msg VARCHAR(4000))";
			int iRet = sqlite3_exec(db, sql.ToCStr(), NULL, NULL, &errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK)
			{
				sqlite3_close(db);
				MOJING_ERROR(g_APIlogger, "Create table(tb_Info) failed: " << errmsg);
#ifdef MJ_OS_ANDROID
				__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "Create table(tb_Info) failed: %s", errmsg);
#endif
				sqlite3_free(errmsg);
				return NULL;
			}
			sqlite3_free(errmsg);

			g_pSqlite3ExecLocker->DoLock();
			sql = "CREATE TABLE IF NOT EXISTS tb_Save (id INTEGER PRIMARY KEY, pid VARCHAR(100), name VARCHAR(100), count INTEGER, date INTEGER, ldate INTEGER, exitdate INTEGER, iid VARCHAR(100))";
			iRet = sqlite3_exec(db, sql.ToCStr(), NULL, NULL, &errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK)
			{
				sqlite3_close(db);
				MOJING_ERROR(g_APIlogger, "Create table(tb_Save) failed: " << errmsg);
#ifdef MJ_OS_ANDROID
				__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "Create table(tb_Save) failed: %s", errmsg);
#endif
				sqlite3_free(errmsg);
				return NULL;
			}
			sqlite3_free(errmsg);

			g_pSqlite3ExecLocker->DoLock();
			sql = "CREATE TABLE IF NOT EXISTS tb_Verify (id INTEGER PRIMARY KEY, pid VARCHAR(32), mid VARCHAR(64), appkey VARCHAR(64), pname VARCHAR(100), result INTEGER)";
			iRet = sqlite3_exec(db, sql.ToCStr(), NULL, NULL, &errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK)
			{
				sqlite3_close(db);
				MOJING_ERROR(g_APIlogger, "Create table(tb_Verify) failed: " << errmsg);
#ifdef MJ_OS_ANDROID
				__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "Create table(tb_Verify) failed: %s", errmsg);
#endif
				sqlite3_free(errmsg);
				return NULL;
			}
			sqlite3_free(errmsg);

			SetInfoDataBase(db);

			return db;
		}

		void DatabaseInfoReporter::CloseInfoDatabase()
		{
#ifdef MJ_OS_ANDROID
			__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "DatabaseInfoReporter::OpenInfoDatabase\n");
#endif
			sqlite3* db = GetInfoDataBase();
			if (db != NULL)
			{
				sqlite3_close(db);
				SetInfoDataBase(NULL);
			}
		}

		int DatabaseInfoReporter::Run()
		{
			SetThreadName("DBInfoReporter");
			while (!GetExitFlag())
			{
				if (!GetReportImmediate())
				{
#ifdef _DEBUG
#ifndef MJ_OS_WIN32
					MOJING_TRACE(g_APIlogger, "Report message from database. Interval = " << GetReportInterval());
#endif
#endif
					ReportMsgFromDB();
				}
				Sleep(GetReportInterval());
			}

			MOJING_TRACE(g_APIlogger, "DatabaseInfoReporter thread exit.");
			return 0;
		}

		int DatabaseInfoReporter::AppInit()
		{
			MojingPlatformBase* pPlatformBase = GetPlatformBase();
			if (pPlatformBase == NULL)
			{
				return false;
			}
			sqlite3* db = OpenInfoDatabase();
			if (db == NULL)
			{
				return false;
			}

			return this->Start(); //Thread start
		}

		bool DatabaseInfoReporter::ReportReInit()
		{
			CloseInfoDatabase();

			sqlite3* db = OpenInfoDatabase();
			if (db == NULL)
			{
				return false;
			}

			return true;
		}

		void DatabaseInfoReporter::AppExit()
		{
			this->Exit();  //Thread Exit

			CloseInfoDatabase();
		}

		int DatabaseInfoReporter::Get_SaveResult_callback(void* data, int n_columns, char** column_values, char** column_names)
		{
			DatabaseInfoReporter *pReporter = (DatabaseInfoReporter *)data;
			if (pReporter == NULL)
			{
				return -1;
			}

			if (n_columns != 8)
			{
				return -2;
			}

			MojingPlatformBase* pPlatformBase = pReporter->GetPlatformBase();
			if (pPlatformBase == NULL)
			{
				return -3;
			}

			if (strcmp(column_values[2],pPlatformBase->GetPackageName()) == 0)
			{
				pReporter->SetRunCount(atoi(column_values[3]));
				pReporter->SetCurrentRunTime(atoll(column_values[4]));
				pReporter->SetLastRunTime(atoll(column_values[5]));
				pReporter->SetCurrentExitTime(atoll(column_values[6]));
				pReporter->SetInstallID(column_values[7]);
			}
			else
			{
				if (pReporter->m_PageInfo.find(column_values[2]) != pReporter->m_PageInfo.end())
				{
					pReporter->m_PageInfo[column_values[2]].count = atoi(column_values[3]);
				}
			}

			return 0;
		}

		static String CreateInstallID(const char* userID, const char* uniqeID)
		{
			if (strlen(userID) != strlen(uniqeID))
			{
				return String(userID) + String(uniqeID);
			}
			int len = (int)strlen(userID);
			char* newID = new char[len + 1];

			for (int i = 0; i < len; i++)
			{
				if (userID[i] == '-')
				{
					newID[i] = '-';
				}
				else
				{
#ifdef MJ_OS_MAC
					sprintf(newID + i, "%X", (userID[i] + uniqeID[i]) % 16);
#else
					sprintf(newID + i, "%x", (userID[i] + uniqeID[i]) % 16);
#endif
				}
			}

			newID[len] = '\0';
			String iid = String(newID);
			delete[] newID;

			return iid;
		}

		bool DatabaseInfoReporter::AppResume(const char* szUniqueID)
		{
#ifdef _DEBUG
            MOJING_TRACE(g_APIlogger, "++++AppResume++++");
#endif
			if (m_bResumeInvoked)
			{
				MOJING_WARN(g_APIlogger, "Invoke AppResume more than one time");
				return false;
			}
			m_bResumeInvoked = true;

			m_lResumDate = GetCurrentTimeMS();

			char* errmsg = NULL;
			char szSQL[1024];
			if (g_pSqlite3ExecLocker == NULL)
			{
				g_pSqlite3ExecLocker = new Mutex();
			}
			if (g_pSqlite3ExecLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "DatabaseInfoReporter::AppResume g_pSqlite3ExecLocker invalid.");
				return false;
			}
			g_pSqlite3ExecLocker->DoLock();
			snprintf(szSQL, 1023, "SELECT * FROM tb_Save WHERE pid = '%s' AND name = '%s'", GetPlatformBase()->GetAppID(), GetPlatformBase()->GetPackageName());
			szSQL[1023] = 0;
			int iRet = sqlite3_exec(GetInfoDataBase(), szSQL, Get_SaveResult_callback, this, &errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK )
			{
				MOJING_ERROR(g_APIlogger, "Select from table(tb_Save) failed: " << errmsg);
				sqlite3_free(errmsg);
				return false;
			}
			sqlite3_free(errmsg);
			if (0 == strcmp(GetInstallID() , "UNKNOWN"))
			{
				SetInstallID(CreateInstallID(GetPlatformBase()->GetUserID(), szUniqueID));
			}

			if (m_bNewRun || GetCurrentRunTime() == 0 || m_lResumDate - GetCurrentExitTime() > GetContinueInterval()*1000L)
			{
				//Set new run time;
				m_bNewRun = true;
				GetPlatformBase()->SetRunID(szUniqueID);
				SetLastRunTime(GetCurrentRunTime());
				SetCurrentRunTime(m_lResumDate);
				SetCurrentActiveTime(0);
				SetRunCount(GetRunCount() + 1);
				if (GetReportImmediate())
				{
					SetPostWait(false);
					ReportMsgFromDB();
				}
			}
			else
			{
#if REPORT_NEXTRUN
				//read the last record and delete it
				int nrow = 0;
				int ncolumn = 0;
				char ** azResult;
				g_pSqlite3ExecLocker->DoLock();
				sprintf(szSQL, "SELECT id FROM tb_Info WHERE pid = '%s' AND type = 2 ORDER BY id DESC LIMIT 1", GetPlatformBase()->GetAppID());
				int iRet = sqlite3_get_table(GetInfoDataBase(), szSQL, &azResult, &nrow, &ncolumn, &errmsg); 
				g_pSqlite3ExecLocker->Unlock();
				if (iRet != SQLITE_OK)
				{
					MOJING_ERROR(g_APIlogger, "Select from table(tb_Info) failed: " << errmsg);
                    sqlite3_free_table(azResult);
					sqlite3_free(errmsg);
					return false;
				}
				sqlite3_free(errmsg);
				if (nrow > 0)
				{
					g_pSqlite3ExecLocker->DoLock();
					sprintf(szSQL, "DELETE FROM tb_Info WHERE id = %d", atoi(azResult[1]));
                    sqlite3_free_table(azResult);			
					iRet = sqlite3_exec(GetInfoDataBase(), szSQL, NULL, NULL, &errmsg);
					g_pSqlite3ExecLocker->Unlock();
					if (iRet != SQLITE_OK)
					{
						MOJING_ERROR(g_APIlogger, "Delete from table(tb_Info) failed: " << errmsg);
						sqlite3_free(errmsg);
						return false;
					}
					sqlite3_free(errmsg);
				}
#else
				SetCurrentRunTime(m_lResumDate);
#endif
			}

//			if (!GetReportImmediate())
//			{
//				this->Resume(); //Thread resume
//			}
			return true;
		}

		void DatabaseInfoReporter::AppPause()
		{
#ifdef _DEBUG
            MOJING_TRACE(g_APIlogger, "----AppPause----");
#endif
			if (!m_bResumeInvoked)
			{
				MOJING_WARN(g_APIlogger, "Invoke AppPause more than one time");
				return;
			}
			m_bResumeInvoked = false;
//			if (!GetReportImmediate())
//			{
//				this->Suspend(); // thread suspend.
//			}

			//post this run
			if (m_bNewRun)
			{
				m_bNewRun = false;
				if (m_RunReporter)
				{
					m_RunReporter->PostMessage();
				}
			}
            
			char* errmsg = NULL;
			if (g_pSqlite3ExecLocker == NULL)
			{
				g_pSqlite3ExecLocker = new Mutex();
			}
			if (g_pSqlite3ExecLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "DatabaseInfoReporter::AppPause g_pSqlite3ExecLocker invalid.");
				return;
			}
			g_pSqlite3ExecLocker->DoLock();
            int iRet = sqlite3_exec(GetInfoDataBase(), "BEGIN TRANSACTION;", NULL, NULL, &errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK)
            {
                MOJING_ERROR(g_APIlogger, "AppPause BEGIN TRANSACTION failed: " << errmsg);
                sqlite3_free(errmsg);
                return;
            }
            sqlite3_free(errmsg);
            
			SetCurrentExitTime(GetCurrentTimeMS());
#if REPORT_NEXTRUN
			int nActiveTime = GetCurrentActiveTime();
			nActiveTime += int(GetCurrentExitTime() - m_lResumDate);
#else
			int nActiveTime = int(GetCurrentExitTime() - GetCurrentRunTime());
#endif
			SetCurrentActiveTime(nActiveTime);

            int nrow = 0;
            int ncolumn = 0;
            char ** azResult;
			char szSQL[1024];
			g_pSqlite3ExecLocker->DoLock();
			sprintf(szSQL, "SELECT * FROM tb_Save WHERE pid = '%s' AND name = '%s'", GetPlatformBase()->GetAppID(), GetPlatformBase()->GetPackageName());
			iRet = sqlite3_get_table(GetInfoDataBase(), szSQL, &azResult, &nrow, &ncolumn, &errmsg); //sqlite3_exec(db, szSQL, NULL, NULL, &errmsg);
			sqlite3_free_table(azResult);
			sqlite3_free(errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK || nrow == 0)
			{
				sprintf(szSQL, "INSERT INTO tb_Save VALUES(?, '%s', '%s', %d, %lld, %lld, %lld, '%s')", GetPlatformBase()->GetAppID(), GetPlatformBase()->GetPackageName(),
					GetRunCount(), GetCurrentRunTime(), GetLastRunTime(), GetCurrentExitTime(), GetInstallID()/*.ToCStr()*/);
			}
			else
			{
				sprintf(szSQL, "UPDATE tb_Save SET count = %d, date = %lld, ldate = %lld, exitdate = %lld WHERE pid = '%s' AND name = '%s'",
					GetRunCount(), GetCurrentRunTime(), GetLastRunTime(), GetCurrentExitTime(), GetPlatformBase()->GetAppID(), GetPlatformBase()->GetPackageName());
			}
			g_pSqlite3ExecLocker->DoLock();
			iRet = sqlite3_exec(GetInfoDataBase(), szSQL, NULL, NULL, &errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK)
			{
				MOJING_ERROR(g_APIlogger, "update table(tb_Save) failed: " << errmsg);
				sqlite3_free(errmsg);
				return;
			}
			sqlite3_free(errmsg);

			if (m_ActiveReporter)
			{
#if REPORT_NEXTRUN
				m_ActiveReporter->SaveMessage();
#else
				m_ActiveReporter->PostMessage();
#endif
			}
			g_pSqlite3ExecLocker->DoLock();
            iRet = sqlite3_exec(GetInfoDataBase(), "COMMIT TRANSACTION;", NULL, NULL, &errmsg);
            if (iRet != SQLITE_OK)
            {
                MOJING_ERROR(g_APIlogger, "AppPause COMMIT TRANSACTION failed: " << errmsg);
            }
            sqlite3_free(errmsg);
			g_pSqlite3ExecLocker->Unlock();
		}

		void DatabaseInfoReporter::AppPageStart(const char* szPageName)
		{
#ifdef _DEBUG
            MOJING_TRACE(g_APIlogger, "++++AppPageStart++++ Page Name : " << szPageName);
#endif
			if (m_PageInfo.find(szPageName) != m_PageInfo.end())
			{
				if (m_PageInfo[szPageName].bStartInvoked)
				{
					MOJING_WARN(g_APIlogger, "Invoke AppPageStart more than one time. Page Name : " << szPageName);
				}
				else
				{
					m_PageInfo[szPageName].date = GetCurrentTimeMS();
					m_PageInfo[szPageName].count++;
                    m_PageInfo[szPageName].bStartInvoked = true;
				}
			}
			else
			{
				PageInfo info;
				memset(&info, 0, sizeof(PageInfo));
				info.bStartInvoked = true;
				info.date = GetCurrentTimeMS();
				info.count = 0;
				m_PageInfo[szPageName] = info;

				char* errmsg = NULL;
				char szSQL[1024]; 
				if (g_pSqlite3ExecLocker == NULL)
				{
					g_pSqlite3ExecLocker = new Mutex();
				}
				if (g_pSqlite3ExecLocker == NULL)
				{
					MOJING_ERROR(g_APIlogger, "DatabaseInfoReporter::AppPageStart g_pSqlite3ExecLocker invalid.");
					return;
				}
				g_pSqlite3ExecLocker->DoLock();
				sprintf(szSQL, "SELECT * FROM tb_Save WHERE pid = '%s' AND name = '%s'", GetPlatformBase()->GetAppID(), szPageName);
				int iRet = sqlite3_exec(GetInfoDataBase(), szSQL, Get_SaveResult_callback, this, &errmsg);
				if (iRet != SQLITE_OK)
				{
				}
				sqlite3_free(errmsg);
				g_pSqlite3ExecLocker->Unlock();
				m_PageInfo[szPageName].count++;	
			}
		}

		void DatabaseInfoReporter::AppPageEnd(const char* szPageName)
		{
#ifdef _DEBUG
            MOJING_TRACE(g_APIlogger, "----AppPageEnd---- Page Name : " << szPageName);
#endif
			if (m_PageInfo.find(szPageName) == m_PageInfo.end())
			{
                MOJING_WARN(g_APIlogger, "Please invoke AppPageStart first. Page name : " << szPageName);
				return;
			}
				
			PageInfo info = m_PageInfo[szPageName];
			if (!info.bStartInvoked)
			{
				MOJING_WARN(g_APIlogger, "Maybe not invoke AppPageStart, or already invoke AppPageEnd. Page name : " << szPageName);
				return;
			}
           
			char* errmsg = NULL;
			if (g_pSqlite3ExecLocker == NULL)
			{
				g_pSqlite3ExecLocker = new Mutex();
			}
			if (g_pSqlite3ExecLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "DatabaseInfoReporter::AppPageEnd g_pSqlite3ExecLocker invalid.");
				return;
			}
			g_pSqlite3ExecLocker->DoLock();
            int iRet = sqlite3_exec(GetInfoDataBase(), "BEGIN TRANSACTION;", NULL, NULL, &errmsg);
			g_pSqlite3ExecLocker->Unlock();
            if (iRet != SQLITE_OK)
            {
                MOJING_ERROR(g_APIlogger, "AppPageEnd BEGIN TRANSACTION failed: " << errmsg);
                sqlite3_free(errmsg);
                return;
            }
            sqlite3_free(errmsg);
			info.bStartInvoked = false;
			info.exitdate = GetCurrentTimeMS();
			info.time = int(info.exitdate - info.date);
            m_PageInfo[szPageName] = info;
			if (m_PageReporter)
			{
				if (GetReportImmediate())
					m_PageReporter->PostMessage(szPageName, info);
				else
					m_PageReporter->SaveMessage(szPageName, info);
			}

            int nrow = 0;
            int ncolumn = 0;
            char ** azResult;
			char szSQL[1024];
			g_pSqlite3ExecLocker->DoLock();
            sprintf(szSQL, "SELECT * FROM tb_Save WHERE pid = '%s' AND name = '%s'", GetPlatformBase()->GetAppID(), szPageName);
			iRet = sqlite3_get_table(GetInfoDataBase(), szSQL, &azResult, &nrow, &ncolumn, &errmsg); //sqlite3_exec(db, szSQL, NULL, NULL, &errmsg);
			sqlite3_free_table(azResult);
			sqlite3_free(errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK || nrow == 0)
			{
				sprintf(szSQL, "INSERT INTO tb_Save VALUES(?, '%s', '%s', %d, %lld, %d, %lld, '%s')", GetPlatformBase()->GetAppID(), szPageName,
					info.count, info.date, info.time, info.exitdate, GetInstallID()/*.ToCStr()*/);
			}
			else
			{
				sprintf(szSQL, "UPDATE tb_Save SET count = %d, date = %lld, ldate = %d, exitdate = %lld WHERE pid = '%s' AND name = '%s'",
					info.count, info.date, info.time, info.exitdate, GetPlatformBase()->GetAppID(), szPageName);
			}
			g_pSqlite3ExecLocker->DoLock();
			iRet = sqlite3_exec(GetInfoDataBase(), szSQL, NULL, NULL, &errmsg);
			g_pSqlite3ExecLocker->Unlock();
			if (iRet != SQLITE_OK)
			{
				MOJING_ERROR(g_APIlogger, "update table(tb_Save) failed: " << errmsg);
				sqlite3_free(errmsg);
				return;
			}
			sqlite3_free(errmsg);
            
			g_pSqlite3ExecLocker->DoLock();
            iRet = sqlite3_exec(GetInfoDataBase(), "COMMIT TRANSACTION;", NULL, NULL, &errmsg);
            if (iRet != SQLITE_OK)
            {
                MOJING_ERROR(g_APIlogger, "AppPageEnd COMMIT TRANSACTION failed: " << errmsg);
            }
            sqlite3_free(errmsg);
			g_pSqlite3ExecLocker->Unlock();
		}

		void DatabaseInfoReporter::AppSetEvent(const char* szEventName, const char* szEventChannelID,
						const char* eEventInName, float eInData, const char* szEventOutName, float eOutData, bool pd)
		{
#ifdef _DEBUG
            MOJING_TRACE(g_APIlogger, "++++AppSetEvent++++");
#endif
			int64_t date = GetCurrentTimeMS();
			if (m_EventReporter)
			{
				if (GetReportImmediate())
					m_EventReporter->PostMessage(szEventName, szEventChannelID, eEventInName, eInData, szEventOutName, eOutData, date, pd);
				else
					m_EventReporter->SaveMessage(szEventName, szEventChannelID, eEventInName, eInData, szEventOutName, eOutData, date, pd);		
			}
		}

		void DatabaseInfoReporter::ReportLog(int iLogType, const char* szTypeName, const char* szLogContent, bool pd)
		{
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "++++ReportLog++++");
#endif
			int64_t date = GetCurrentTimeMS();
			if (m_LogReporter)
			{
				if (GetReportImmediate())
					m_LogReporter->PostMessage(iLogType, szTypeName, szLogContent, date, pd);
				else
					m_LogReporter->SaveMessage(iLogType, szTypeName, szLogContent, date, pd);
			}
		}

		void DatabaseInfoReporter::AppReportAll()
		{
#ifdef _DEBUG
			MOJING_FUNC_TRACE(g_APIlogger);
#endif
			SetPostWait(true);
			ReportMsgFromDB();
		}
	}
}
