#include "MojingString.h"
#include "MojingLog.h"

namespace Baofeng
{
	namespace Mojing
	{

		void StringBuffer::AppendFormat(const char* format, ...)
		{
			va_list argList;

			va_start(argList, format);
			UPInt size = MJ_vscprintf(format, argList);
			va_end(argList);

			char* buffer = (char*)MJ_ALLOC(sizeof(char)* (size + 1));

			va_start(argList, format);
			UPInt result = MJ_vsprintf(buffer, size + 1, format, argList);
			MJ_UNUSED1(result);
			va_end(argList);
			MJ_ASSERT_LOG(result == size, ("Error in MJ_vsprintf"));

			AppendString(buffer);

			MJ_FREE(buffer);
		}
	}
} // OVR
