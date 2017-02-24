#pragma once
namespace Baofeng
{
	namespace Mojing
	{
		class MojingCompress
		{
		public:
			MojingCompress();
			virtual ~MojingCompress();
			static bool CompressFile(const char * szSrcFile, const char* szDestFile  , const char* szAppendInfo = (char*)0);
			static char* GetFileAppendInfo(const char * szSrcFile);
		};

	}
}