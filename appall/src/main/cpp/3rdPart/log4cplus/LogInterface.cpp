#include "LogInterface.h"
#include "../../Base/MojingLoadSo.h"

/* For debug builds, always enable the debug traces in this library */
#ifndef NDEBUG
#include <android/log.h>
#define LOGV(...)  ((void)__android_log_print(ANDROID_LOG_VERBOSE, "MojingLogger", __VA_ARGS__))
#define LOGI(...)  ((void)__android_log_print(ANDROID_LOG_INFO, "MojingLogger", __VA_ARGS__))
#define LOGD(...)  ((void)__android_log_print(ANDROID_LOG_WARN, "MojingLogger", __VA_ARGS__))
#define LOGW(...)  ((void)__android_log_print(ANDROID_LOG_WARN, "MojingLogger", __VA_ARGS__))
#define LOGE(...)  ((void)__android_log_print(ANDROID_LOG_ERROR, "MojingLogger", __VA_ARGS__))
#define LOGF(...)  ((void)__android_log_print(ANDROID_LOG_FATAL, "MojingLogger", __VA_ARGS__))
#else
#define LOGV(...)  ((void)0)
#define LOGI(...)  ((void)0)
#define LOGD(...)  ((void)0)
#define LOGW(...)  ((void)0)
#define LOGE(...)  ((void)0)
#define LOGF(...)  ((void)0)
#endif

#undef NULL
#define NULL  0

typedef void* (*FP_CreateLogger)(const char *sLogFileName, const char *sLoggerName, int nLogLevel, int nMaxFileSize, int nMaxFileNum);
typedef void (*FP_DeleteLogger)(void *logger);
typedef void (*FP_Log)(void *logger, int logLevel, const char* logString, const char* fileName, int line);

MojingLoadSo* g_pLoadHandler = NULL;
FP_CreateLogger g_fpCreateLogger = NULL;
FP_DeleteLogger g_fpDeleteLogger = NULL;
FP_Log			g_fpLog = NULL;

static void LoadLogInterface()
{
	if (g_pLoadHandler == NULL)
	{
		g_pLoadHandler = new MojingLoadSo((char*)"libmdklog.so");
		if (g_pLoadHandler == NULL || !g_pLoadHandler->IsLoaded())
		{
			g_pLoadHandler = new MojingLoadSo((char*)"/sdcard/MojingSDK/libmdklog.so");
		}
	}
	if (g_pLoadHandler && g_pLoadHandler->IsLoaded())
	{
		if (g_fpCreateLogger == NULL)
		{
			g_fpCreateLogger = (FP_CreateLogger)g_pLoadHandler->FindFunction((char *)"CreateLogger");
		}
		
		if (g_fpDeleteLogger == NULL)
		{
			g_fpDeleteLogger = (FP_DeleteLogger)g_pLoadHandler->FindFunction((char *)"DeleteLogger");
		}

		if (g_fpLog == NULL)
		{
			g_fpLog = (FP_Log)g_pLoadHandler->FindFunction((char *)"Log");
		}
	}
}

MojingLogger::MojingLogger(const char* sLogFileName, const char *sLoggerName,
	int nLogLevel, int nMaxFileSize, int nMaxFileNum)
	: m_logger(NULL)
{
	LoadLogInterface();

	if (g_fpCreateLogger)
	{
		m_logger = g_fpCreateLogger(sLogFileName, sLoggerName, nLogLevel, nMaxFileSize, nMaxFileNum);
	}
	m_nLogLevel = nLogLevel;
}

MojingLogger::MojingLogger(const char* sLoggerName)
: m_logger(NULL)
{
	LoadLogInterface();

	if (g_fpCreateLogger)
	{
		m_logger = g_fpCreateLogger(NULL, sLoggerName, TRACE_LOG_LEVEL, 0, 0);
	}
	m_nLogLevel = TRACE_LOG_LEVEL;
}

MojingLogger::~MojingLogger()
{
	if (m_logger != NULL)
	{
		if (g_fpDeleteLogger)
		{
			g_fpDeleteLogger(m_logger);
		}
		m_logger = NULL;
	}
}

void MojingLogger::Log(int nLogLevel, const char* pLogString, const char* pFileName, int nLine)
{
	if (g_fpLog && m_logger)
	{
		g_fpLog(m_logger, nLogLevel, pLogString, pFileName, nLine);
	}

	switch (nLogLevel)
	{
		case INFO_LOG_LEVEL:
			LOGI("%s [%s : %d]", pLogString, pFileName, nLine);
			break;	
		case WARN_LOG_LEVEL:
			LOGW("%s [%s : %d]", pLogString, pFileName, nLine);	
			break;
		case ERROR_LOG_LEVEL:
			LOGE("%s [%s : %d]", pLogString, pFileName, nLine);	
			break;
		case FATAL_LOG_LEVEL:
			LOGF("%s [%s : %d]", pLogString, pFileName, nLine);	
			break;	
		case DEBUG_LOG_LEVEL:
			LOGD("%s [%s : %d]", pLogString, pFileName, nLine);	
			break;		
		default:
			LOGV("%s [%s : %d]", pLogString, pFileName, nLine);
			break;			
	}
}

MojingFuncTrace::MojingFuncTrace(const char* sLoggerName, const char* sFuncName, const char* sFileName, int nLine)
{
	LoadLogInterface();

	if (g_fpCreateLogger)
	{
		m_logger = g_fpCreateLogger(NULL, sLoggerName, TRACE_LOG_LEVEL, 0, 0);
	}

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
	std::string myString = logString;
	myString += m_sFuncName;
	if (g_fpLog && m_logger)
	{
		g_fpLog(m_logger, TRACE_LOG_LEVEL, myString.c_str(), m_sFile.c_str(), m_nLine);
	}
	
	LOGV("%s [%s : %d]", myString.c_str(), m_sFile.c_str(), m_nLine);
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
