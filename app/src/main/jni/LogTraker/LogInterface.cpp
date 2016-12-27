#include <fcntl.h>
#include "../Platform/MojingPlatformBase.h"
#include "log4cplus/appender.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/ndc.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/spi/filter.h"
#include "log4cplus/clogger.h"
#include "log4cplus/helpers/property.h"

#include "LogInterface.h"

using namespace log4cplus::helpers;
using namespace log4cplus::spi;

// tstring g_sLogPath = LOG4CPLUS_TEXT("/sdcard/MojingSDK/log/");

MojingLogger::MojingLogger(const char* sLogFileName, const char *sLoggerName,
	int nLogLevel, int nMaxFileSize, int nMaxFileNum)
{
	m_logger = Logger::getInstance(sLoggerName);

	char *pDefatultPath = Baofeng::Mojing::MojingPlatformBase::GetDefaultLocalLogPath();
	tstring path = pDefatultPath;
	delete pDefatultPath;
	if (*path.end() != '/' && *path.end() != '\\')
		path += "/";
	path += "log/";
	path += sLogFileName;

	RollingFileAppender *pAppender = new RollingFileAppender(path, nMaxFileSize, nMaxFileNum, false, true);
	SharedAppenderPtr _append(pAppender);
	_append->setName(sLoggerName);

	//TTCCLayout *pLayout = new TTCCLayout();

	std::string pattern = "%D{%m/%d/%y %H:%M:%S:%Q}  - [%t] %p - %m [%l]%n";
	PatternLayout *pLayout = new PatternLayout(pattern);
	_append->setLayout(std::auto_ptr<Layout>(pLayout));

	//	Logger::getRoot().addAppender(_append);
	m_logger.addAppender(_append);
	m_logger.setLogLevel(nLogLevel);
	//delete pLayout;
	//delete pAppender;
}

MojingLogger::MojingLogger(const char* sLoggerName)
{
	m_logger = Logger::getInstance(sLoggerName);
	SharedAppenderPtrList appenders = m_logger.getAllAppenders();
	if (appenders.begin() == appenders.end())
	{
		// No appender yet.
		tstring logFileName = sLoggerName;
		logFileName += ".log";
		MojingLogger(logFileName.c_str(), sLoggerName);
	}
}

MojingLogger::~MojingLogger()
{
}

MojingFuncTrace::MojingFuncTrace(const char* sLoggerName, const char* sFuncName, const char* sFileName, int nLine)
{
	m_logger = Logger::getInstance(sLoggerName);
	m_sFuncName = sFuncName;
	m_sFile = sFileName;
	m_nLine = nLine;
	log("Enter ");
}

MojingFuncTrace::MojingFuncTrace(MojingLogger& mojinglogger, const char* sFuncName, const char* sFileName, int nLine)
{
	m_logger = mojinglogger.m_logger;
	m_sFuncName = sFuncName;
	m_sFile = sFileName;
	m_nLine = nLine;
	log("Enter ");
}

MojingFuncTrace::~MojingFuncTrace()
{
	log("Leave ");
}

void MojingFuncTrace::log(const char* logString)
{
	log4cplus::Logger const & _l = log4cplus::detail::macros_get_logger(m_logger);
	if (LOG4CPLUS_MACRO_LOGLEVEL_PRED(_l.isEnabledFor(log4cplus::TRACE_LOG_LEVEL), TRACE_LOG_LEVEL))
	{
		LOG4CPLUS_MACRO_INSTANTIATE_OSTRINGSTREAM(_log4cplus_buf);
		_log4cplus_buf << logString << m_sFuncName;
		log4cplus::detail::macro_forced_log(_l, log4cplus::TRACE_LOG_LEVEL, _log4cplus_buf.str(), m_sFile.c_str(), m_nLine, LOG4CPLUS_MACRO_FUNCTION());       \
	}
}

//void LoggerTest()
//{
//#if 0
//	/* step 1: Instantiate an appender object */
//	SharedObjectPtr<Appender> _append(new ConsoleAppender());
//	_append->setName(LOG4CPLUS_TEXT("append for test"));
//	/* step 2: Instantiate a layout object */
//	//std::string pattern = ;   
//	std::auto_ptr<Layout> _layout(new PatternLayout(LOG4CPLUS_TEXT("%d{%m/%d/%y %H:%M:%S}  - %m [%l]%n")));
//	/* step 3: Attach the layout object to the appender */
//	_append->setLayout(_layout);
//	/* step 4: Instantiate a logger object */
//	Logger _logger = Logger::getInstance(LOG4CPLUS_TEXT("test"));
//	/* step 5: Attach the appender object to the logger  */
//	_logger.addAppender(_append);
//	/* step 6: Set a priority for the logger  */
//	_logger.setLogLevel(ALL_LOG_LEVEL);
//	/* log activity */
//	LOG4CPLUS_DEBUG(_logger, "This is the FIRST log message");
//	sleep(1);
//	LOG4CPLUS_WARN(_logger, "This is the SECOND log message");
//#endif
//
//#define LOOP_COUNT 200000        
//#if 1	
//
////	log4cplus::initialize();
//
//	bool bRet = LogFileInit("/sdcard/DebugAll.log", "Debug_Info", "StringToMatch=DEBUG AcceptOnMatch=true", LOG_FILE_MAX_SIZE, LOG_FILE_MAX_NUM);
//
//// 	Logger root = Logger::getRoot();
//// 	Logger test = Logger::getInstance("test");
////	Logger subTest = Logger::getInstance("test.subtest");
//	for (int i = 0; i < 100; i++)
//	{
////		NDCContextCreator _context("loop");
////		LOG4CPLUS_TRACE(subTest, "Entering Trace loop #" << i);
//		Mojing_Debug("Entering DEBUG loop #");
//		Mojing_Debug("Entering DEBUG loop #" << i);
//		Mojing_Debug("Entering DEBUG loop " << "again #" << i);
//// 		LOG4CPLUS_ERROR(subTest, "Entering ERROR loop #" << i);
//// 		LOG4CPLUS_WARN(subTest, "Entering WARN loop #" << i);
//// 		LOG4CPLUS_FATAL(subTest, "Entering FATAL loop #" << i);
//	}
//	Mojing_Debug("Leave LoggerTest");
//#endif
//
//#if 0
//	//PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("mojingcfg.properties"));
//	log4cplus_file_configure(LOG4CPLUS_TEXT("/data/mojingcfg.properties"));
//
//	//入root的logger写入记录
//	Logger root = Logger::getRoot();
//	LOG4CPLUS_FATAL(root, "向all_msgs.log文件中写入信息，因为向root，自动往所有的字文件写入!");
//
//	//只向log4cplus.logger.trace_msgs      = TRACE,TRACE_MSGS对应的
//	//log4cplus.appender.TRACE_MSGS文件写入日志
//	Logger logger1 = Logger::getInstance(LOG4CPLUS_TEXT("TRACE_MSGS"));
//	LOG4CPLUS_TRACE(logger1, "只向all_msgs.log或trace_msgs.log文件写入日志!");
//#endif
//	// 	int fd = open("/storage/mojingcfg.properties", O_RDWR);
//	// 	char buffer[256];
//	// 	read(fd, buffer, 255);
//}
