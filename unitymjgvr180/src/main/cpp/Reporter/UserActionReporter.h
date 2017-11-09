#pragma once
#include "../Base/MojingTypes.h"
#include "../Base/MojingString.h"
#include "ReporterTools.h"
namespace Baofeng
{
	namespace Mojing
	{
		class UserActionReporter:public ReporterTools
		{
			UserActionReporter();
			static void InternetProfileCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam);
		public:
			virtual ~UserActionReporter();
			static UserActionReporter* GetUserActionReporter();
			
			CLASS_MEMBER_STR(String, m_str, ClassName);

			void Post(const char* strActionType, const char* strItemID, const char* strJsonValue);
		};
	}
}
