#pragma once
#include "ReporterTools.h"
namespace Baofeng
{
	namespace Mojing
	{
#ifdef  USING_MINIDUMP
		class CrashReporter :	public ReporterTools
		{
			static void InternetProfileCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam);
			CrashReporter();
			virtual ~CrashReporter();
			bool	DoReport_ZIP(const char* szMinidumpFile = NULL, bool bWait = false);
		public:			
			static CrashReporter* GetCrashReporter(const char* szMiniDumpFileName);
			CLASS_INTERFACE_STR(String, m_str, ClassName);
			CLASS_INTERFACE_STR(String, m_str, MiniDumpFileName);
			bool	DoReport(bool bWait = false);
		};
#endif

	}
}