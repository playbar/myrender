#pragma once
#include "InfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		class RunInfoReporter : public InfoReporter
		{
		public:
			RunInfoReporter();
			~RunInfoReporter();

			void PostMessage();
		private:
			void CreateReportMsg();
		};
	}
}

