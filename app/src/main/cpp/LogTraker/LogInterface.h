#pragma once
#include "../Base/MojingTypes.h"
#include "log4cplus/loggingmacros.h"
#include "log4cplus/logger.h"

using namespace log4cplus;



#define LOG_FILE_MAX_SIZE		(1024 * 1024 * 2)
#define LOG_FILE_MAX_NUM		10

class MojingLogger
{
public:
	MojingLogger(const char* sLogFileName, const char *sLoggerName,
		int nLogLevel = TRACE_LOG_LEVEL, int nMaxFileSize = LOG_FILE_MAX_SIZE, int nMaxFileNum = LOG_FILE_MAX_NUM);
	MojingLogger(const char* sLoggerName);
	~MojingLogger();

public:
	Logger m_logger;
};

class MojingFuncTrace
{
public:
	MojingFuncTrace(const char* sLoggerName, const char* sFuncName, const char* sFileName, int nLine);
	MojingFuncTrace(MojingLogger& mojinglogger, const char* sFuncName, const char* sFileName, int nLine);
	~MojingFuncTrace(void);
	
private:
	Logger m_logger;
	tstring m_sFuncName;
	tstring m_sFile;
	int m_nLine;

private:
	void log(const char* logString);
};

#ifdef ENABLE_LOGGER
#define MOJING_FUNC_TRACE(mojingLoggerOrloggerName)			MojingFuncTrace funcTrace(mojingLoggerOrloggerName, __FUNCTION__, __FILE__, __LINE__)
#define MOJING_TRACE(mojinglogger, logEvent)	LOG4CPLUS_TRACE(mojinglogger.m_logger, logEvent)
#define MOJING_DEBUG(mojinglogger, logEvent)	LOG4CPLUS_DEBUG(mojinglogger.m_logger, logEvent)
#define MOJING_WARN(mojinglogger, logEvent)		LOG4CPLUS_WARN(mojinglogger.m_logger, logEvent)
#define MOJING_ERROR(mojinglogger, logEvent)	LOG4CPLUS_ERROR(mojinglogger.m_logger, logEvent)
#define MOJING_FATAL(mojinglogger, logEvent)	LOG4CPLUS_FATAL(mojinglogger.m_logger, logEvent)
#else
#define MOJING_FUNC_TRACE(mojingLoggerOrloggerName)	    
#define MOJING_TRACE(mojinglogger, logEvent)   
#define MOJING_WARN(mojinglogger, logEvent)
#define MOJING_ERROR(mojinglogger, logEvent)
#define MOJING_FATAL(mojinglogger, logEvent)
#endif