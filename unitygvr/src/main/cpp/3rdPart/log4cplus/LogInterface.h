#pragma once
#include <iostream>
#include <sstream>
#include <string.h>

#define LOG_FILE_MAX_SIZE		(1024 * 1024 * 2)
#define LOG_FILE_MAX_NUM		10

#define FATAL_LOG_LEVEL			50000
#define ERROR_LOG_LEVEL			40000
#define WARN_LOG_LEVEL			30000
#define INFO_LOG_LEVEL			20000
#define DEBUG_LOG_LEVEL			10000
#define TRACE_LOG_LEVEL			0

class MojingLogger
{
public:
	MojingLogger(const char* sLogFileName, const char *sLoggerName,
		int nLogLevel = TRACE_LOG_LEVEL, int nMaxFileSize = LOG_FILE_MAX_SIZE, int nMaxFileNum = LOG_FILE_MAX_NUM);
	MojingLogger(const char* sLoggerName);
	~MojingLogger();

	void Log(int nLogLevel, const char* pLogString, const char* pFileName, int nLine);

public:
	void* m_logger;
	int m_nLogLevel;
};

class MojingFuncTrace
{
public:
	MojingFuncTrace(const char* sLoggerName, const char* sFuncName, const char* sFileName, int nLine);
	MojingFuncTrace(MojingLogger& mojinglogger, const char* sFuncName, const char* sFileName, int nLine);
	~MojingFuncTrace(void);
	
private:
	void* m_logger;
	std::string m_sFuncName;
	std::string m_sFile;
	int m_nLine;

private:
	void log(const char* logString);
};

#define LOG_BODY(logger, logEvent, logLevel)                \
	do {													\
		if (logLevel >= logger.m_nLogLevel ) {				\
			std::ostringstream _buf;						\
			_buf << logEvent;                               \
			logger.Log(logLevel, _buf.str().c_str(),		\
				__FILE__, __LINE__);						\
		}                                                   \
	} while (0)

#ifdef ENABLE_LOGGER
#define MOJING_FUNC_TRACE(mojingLoggerOrloggerName)			MojingFuncTrace funcTrace(mojingLoggerOrloggerName, __FUNCTION__, __FILE__, __LINE__)
#define MOJING_TRACE(mojinglogger, logEvent)	LOG_BODY(mojinglogger, logEvent, TRACE_LOG_LEVEL)
#define MOJING_DEBUG(mojinglogger, logEvent)	LOG_BODY(mojinglogger, logEvent, DEBUG_LOG_LEVEL)
#define MOJING_WARN(mojinglogger, logEvent)		LOG_BODY(mojinglogger, logEvent, WARN_LOG_LEVEL)
#define MOJING_ERROR(mojinglogger, logEvent)	LOG_BODY(mojinglogger, logEvent, ERROR_LOG_LEVEL)
#define MOJING_FATAL(mojinglogger, logEvent)	LOG_BODY(mojinglogger, logEvent, FATAL_LOG_LEVEL)
#else
#define MOJING_FUNC_TRACE(mojingLoggerOrloggerName)	    
#define MOJING_TRACE(mojinglogger, logEvent)   
#define MOJING_WARN(mojinglogger, logEvent)
#define MOJING_ERROR(mojinglogger, logEvent)
#define MOJING_FATAL(mojinglogger, logEvent)
#endif