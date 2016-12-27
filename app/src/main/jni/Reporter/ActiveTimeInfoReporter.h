#pragma once
#include "InfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		class ActiveTimeInfoReporter : public InfoReporter
		{
		public:
			ActiveTimeInfoReporter();
			~ActiveTimeInfoReporter();
			void PostMessage();
			void SaveMessage();
		private:
			void CreateReportMsg();
		};
	}
}

