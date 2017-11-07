#pragma once
#include "InfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		class EventInfoReporter : public InfoReporter
		{
		public:
			EventInfoReporter();
			virtual ~EventInfoReporter(){};
			void PostMessage(const char* szEventName, const char* szEventChannelID, const char* szEventInName,
				float eInData, const char* szEventOutName, float eOutData, int64_t date, bool pd);
			void SaveMessage(const char* szEventName, const char* szEventChannelID, const char* szEventInName,
				float eInData, const char* szEventOutName, float eOutData, int64_t date, bool pd);

		private:
			void CreateReportMsg(const char* szEventName, const char* szEventChannelID, const char* szEventInName,
				float eInData, const char* szEventOutName, float eOutData, int64_t date, bool pd);
		};
	}
}

