#include "MojingLog.h"
#include "MojingStd.h"
#include <stdarg.h>
#include <stdio.h>

#if defined(MJ_OS_WIN32)
#include <windows.h>
#elif defined(MJ_OS_ANDROID)
#include <android/log.h>
#endif

namespace Baofeng
{
	namespace Mojing
	{

		// Global Log pointer.
		Log* volatile MJ_GlobalLog = 0;

		//-----------------------------------------------------------------------------------
		// ***** Log Implementation

		Log::~Log()
		{
			// Clear out global log
			if (this == MJ_GlobalLog)
			{
				// TBD: perhaps we should ASSERT if this happens before system shutdown?
				MJ_GlobalLog = 0;
			}
		}

		void Log::LogMessageVarg(LogMessageType messageType, const char* fmt, va_list argList)
		{
			if ((messageType & LoggingMask) == 0)
				return;
#ifndef MJ_BUILD_DEBUG
			if (IsDebugMessage(messageType))
				return;
#endif

			char buffer[MaxLogBufferMessageSize];
			FormatLog(buffer, MaxLogBufferMessageSize, messageType, fmt, argList);
			DefaultLogOutput(messageType, buffer);
		}

		void Log::LogMessage(LogMessageType messageType, const char* pfmt, ...)
		{
			va_list argList;
			va_start(argList, pfmt);
			LogMessageVarg(messageType, pfmt, argList);
			va_end(argList);
		}


		void Log::FormatLog(char* buffer, unsigned bufferSize, LogMessageType messageType,
			const char* fmt, va_list argList)
		{
			bool addNewline = true;

			switch (messageType)
			{
			case Log_Error:         MJ_strcpy(buffer, bufferSize, "Error: ");     break;
			case Log_Debug:         MJ_strcpy(buffer, bufferSize, "Debug: ");     break;
			case Log_Assert:        MJ_strcpy(buffer, bufferSize, "Assert: ");    break;
			case Log_Text:       buffer[0] = 0; addNewline = false; break;
			case Log_DebugText:  buffer[0] = 0; addNewline = false; break;
			default:
				buffer[0] = 0;
				addNewline = false;
				break;
			}

			UPInt prefixLength = MJ_strlen(buffer);
			char *buffer2 = buffer + prefixLength;
			MJ_vsprintf(buffer2, bufferSize - prefixLength, fmt, argList);

			if (addNewline)
				MJ_strcat(buffer, bufferSize, "\n");
		}


		void Log::DefaultLogOutput(LogMessageType messageType, const char* formattedText)
		{

#if defined(MJ_OS_WIN32)
			// Under Win32, output regular messages to console if it exists; debug window otherwise.
			static DWORD dummyMode;
			static bool  hasConsole = (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE) &&
				(GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dummyMode));

			if (!hasConsole || IsDebugMessage(messageType))
			{
				::OutputDebugStringA(formattedText);
			}
			else
			{
				fputs(formattedText, stdout);
			}

#elif defined(MJ_OS_ANDROID)

			int logPriority = ANDROID_LOG_INFO;
			switch(messageType)
			{
			case Log_DebugText:    
			case Log_Debug:     logPriority = ANDROID_LOG_DEBUG; break;
			case Log_Assert:
			case Log_Error:     logPriority = ANDROID_LOG_ERROR; break;
			default:
				logPriority = ANDROID_LOG_INFO;       
			}

			__android_log_write(logPriority, "OVR", formattedText);

#else
			fputs(formattedText, stdout);

#endif

			// Just in case.
			MJ_UNUSED2(formattedText, messageType);
		}


		//static
		void Log::SetGlobalLog(Log *log)
		{
			MJ_GlobalLog = log;
		}
		//static
		Log* Log::GetGlobalLog()
		{
			// No global log by default?
			//    if (!MJ_GlobalLog)
			//        MJ_GlobalLog = GetDefaultLog();
			return MJ_GlobalLog;
		}

		//static
		Log* Log::GetDefaultLog()
		{
			// Create default log pointer statically so that it can be used
			// even during startup.
			static Log defaultLog;
			return &defaultLog;
		}


		//-----------------------------------------------------------------------------------
		// ***** Global Logging functions

#define MJ_LOG_FUNCTION_IMPL(Name)  \
	void Log##Name(const char* fmt, ...) \
		{                                                                    \
		if (MJ_GlobalLog)                                               \
		{                                                                \
		va_list argList; va_start(argList, fmt);                     \
		MJ_GlobalLog->LogMessageVarg(Log_##Name, fmt, argList);  \
		va_end(argList);                                             \
		}                                                                \
		}

		MJ_LOG_FUNCTION_IMPL(Text)
			MJ_LOG_FUNCTION_IMPL(Error)

#ifdef MJ_BUILD_DEBUG
			MJ_LOG_FUNCTION_IMPL(DebugText)
			MJ_LOG_FUNCTION_IMPL(Debug)
			MJ_LOG_FUNCTION_IMPL(Assert)
#endif

	} // OVR
}