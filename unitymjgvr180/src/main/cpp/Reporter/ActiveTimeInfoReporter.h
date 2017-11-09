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
			virtual ~ActiveTimeInfoReporter(){};
			void PostMessage();
			void SaveMessage();
		private:
			void CreateReportMsg();
		};
	}
}

