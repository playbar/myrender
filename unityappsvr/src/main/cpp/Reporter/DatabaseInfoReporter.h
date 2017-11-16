#pragma once
#include "../Base/MojingTypes.h"
#include "../Base/MojingString.h"
#include "../Base/MojingJSON.h"
#include "../Base/MojingThreads.h"
#include "../Platform/MojingPlatformBase.h"
#include "ReporterTools.h"
#include <map>
//#ifdef MJ_OS_WIN32
#include "../3rdPart/sqlite3/sqlite3.h"
//#else
//#include <sqlite3.h>
//#endif

using namespace std;

namespace Baofeng
{
	namespace Mojing
	{
		typedef struct _PageInfo
		{
			bool bStartInvoked;
			int count;
			int time;
			int64_t date;
			int64_t exitdate;
		}PageInfo;

		class RunInfoReporter;
		class EventInfoReporter;
		class ActiveTimeInfoReporter;
		class PageInfoReporter;
		class LogInfoReporter;

		class DatabaseInfoReporter : public ReporterTools, public Thread
		{
			friend class RunInfoReporter;
			friend class EventInfoReporter;
			friend class ActiveTimeInfoReporter;
			friend class PageInfoReporter;
		public:
			DatabaseInfoReporter();
			virtual ~DatabaseInfoReporter();

			CLASS_MEMBER_STR(String, m_str, ClassName);
			CLASS_MEMBER_STR(String, m_, InstallID);
			CLASS_MEMBER(sqlite3*, m_p, InfoDataBase);
			CLASS_MEMBER(MojingPlatformBase*, m_p, PlatformBase);
			CLASS_MEMBER(int, m_, ContinueInterval);
			CLASS_MEMBER(int, m_, ReportInterval);
			CLASS_MEMBER(bool, m_, ReportImmediate);
			CLASS_INTERFACE(int64_t, m_, CurrentRunTime);
			CLASS_INTERFACE(int64_t, m_, LastRunTime);
			CLASS_INTERFACE(int64_t, m_, CurrentExitTime);
			CLASS_INTERFACE(int, m_, CurrentActiveTime);
			CLASS_INTERFACE(int, m_, RunCount);			
			CLASS_INTERFACE(bool, m_, PostWait);

		public:
			int AppInit();
            void AppExit();
			void AppReportAll();
			bool AppResume(const char* szUniqueID);
			void AppPause();
			void AppPageStart(const char* szPageName);
			void AppPageEnd(const char* szPageName);
			void AppSetEvent(const char* szEventName, const char* szEventChannelID,	const char* szEventInName, float eInData, const char* szEventOutName, float eOutData, bool pd);
			void ReportLog(int iLogType, const char* szTypeName, const char* szLogContent, bool pd);
			void AppSetReportImmediate(bool bReportImmediate);
			bool ReportReInit();

		private:
			RunInfoReporter* m_RunReporter;
			ActiveTimeInfoReporter *m_ActiveReporter;
			PageInfoReporter *m_PageReporter;
			EventInfoReporter *m_EventReporter;
			LogInfoReporter *m_LogReporter;
			bool m_bResumeInvoked;
			bool m_bNewRun;
			int64_t m_lResumDate;
			map<String, PageInfo> m_PageInfo;			
			static int Get_InfoResult_callback(void* data, int n_columns, char** column_values, char** column_names);
			static int Get_SaveResult_callback(void* data, int n_columns, char** column_values, char** column_names);
			static void InternetInfoPostCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam);

			virtual int   Run();

			sqlite3* OpenInfoDatabase();
			void CloseInfoDatabase();
			void ReportMsgFromDB();
		};
	}
}
