#pragma once
#include "InfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		class LogInfoReporter : public InfoReporter
		{
		public:
			LogInfoReporter();
			virtual ~LogInfoReporter(){};
			void PostMessage(int iLogType, const char* szTypeName, const char* szLogContent, int64_t date, bool pd);
			void SaveMessage(int iLogType, const char* szTypeName, const char* szLogContent, int64_t date, bool pd);

		private:
			void CreateReportMsg(int iLogType, const char* szTypeName, const char* szLogContent, int64_t date, bool pd);
		};
	}
}

