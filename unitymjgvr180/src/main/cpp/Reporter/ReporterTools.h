#pragma once
#include "../Base/BuildType.h"
#include "../Base/MojingString.h"
namespace Baofeng
{
	namespace Mojing
	{
		class ReporterTools
		{
		public:
			ReporterTools();
			virtual ~ReporterTools();

			static int GetCurrentTime();
			static int64_t GetCurrentTimeMS();
			static String AES_Value_S(String S);
			static String BaofengEnc(String S);
			static String BaofengEncEx(String S);
			static String ConvertToUrlEncode(String S);
			static char * AES_Value(const char * Str, int iLength);
			static char * B2A(const char * Str, int iLength);
		};


	}
}
