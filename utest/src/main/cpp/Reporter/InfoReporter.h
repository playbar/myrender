#pragma once
#include "../Base/MojingTypes.h"
#include "../Base/MojingString.h"
#include "../Base/MojingJSON.h"
#include "../Platform/MojingPlatformBase.h"
#include "../MojingManager.h"
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#include "ReporterTools.h"
#include "DatabaseInfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		enum MessageType{
			MSGTYPE_ACTIVE,
			MSGTYPE_ERROR
		};
		class InfoReporter : public ReporterTools
		{
			static void InternetInfoPostCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam);
		public:
			InfoReporter();
			virtual ~InfoReporter(){};
			void Post();
			void SaveToDB();

			CLASS_MEMBER_STR(String, m_str, ClassName);	
			CLASS_INTERFACE_STR(String, m_, ReportMsg);
			CLASS_INTERFACE(int, m_, ReportType);
			CLASS_INTERFACE(MessageType, m_, MessageType);
		private:
			sqlite3 * m_sqlite3;
		};
	}
}

