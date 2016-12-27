#pragma once
#include "InfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		class PageInfoReporter : public InfoReporter
		{
		public:
			PageInfoReporter();
			virtual ~PageInfoReporter(){};

			void PostMessage(String pageName, PageInfo info);
			void SaveMessage(String pageName, PageInfo info);
		private:
			void CreateReportMsg(String pageName, PageInfo info);
		};
	}
}
