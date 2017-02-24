#include "MojingTypes.h"
#include "MojingCompress.h"
#ifdef WIN32
#include "../3rdPart/zlib/zlib.h"
#else
#include <zlib.h>
#endif
#include <stdio.h>
#include <string.h>
#ifdef MJ_OS_ANDROID
extern char * g_pPacketName;
extern char * g_pUserID;
#endif
namespace Baofeng
{
	namespace Mojing
	{
		MojingCompress::MojingCompress()
		{
		}


		MojingCompress::~MojingCompress()
		{
		}

		bool MojingCompress::CompressFile(const char * szSrcFile, const char* szDestFile, const char* szAppendInfo)
		{
			FILE * fp_in = NULL; 
			size_t len = 0;
			char buf[4096];

			bool bRet = true;

			if (NULL == (fp_in = fopen(szSrcFile, "rb")))
			{
				return false;
			}

			/////////////////////////////////////////////
			gzFile out = gzopen(szDestFile, "wb6f");

			if (out == NULL)
			{
				return false;
			}

			for (;;)
			{
				len = fread(buf, 1, sizeof(buf), fp_in);

				if (ferror(fp_in))
				{
					bRet = false;
					break;
				}

				if (len == 0) break;

				if (gzwrite(out, buf, (unsigned)len) != (int)len)
				{
					bRet = false;
				}
			}

			
			gzclose(out);

			fclose(fp_in);

			if (!bRet)
			{
				remove(szDestFile);
			}
			else if (szAppendInfo && *szAppendInfo)
			{
				FILE *pOutFile = fopen(szDestFile , "ab+");
				fseek(pOutFile, 0, SEEK_END);
				int iFileSize = (int) ftell(pOutFile);
#ifdef MJ_OS_ANDROID
				sprintf(buf, "<APPEND><Context>%s</Context><ContextSize>%d</ContextSize><Packet>%s</Packet><UserID>%s</UserID></APPEND>", 
					szAppendInfo, (int)strlen(szAppendInfo), g_pPacketName == NULL ? "Unknown" : g_pPacketName, g_pUserID == NULL ? "Unknown" : g_pUserID);
#else
				sprintf(buf, "<APPEND><Context>%s</Context><ContextSize>%d</ContextSize></APPEND>", szAppendInfo, (int)strlen(szAppendInfo));
#endif

				int iTotleContextSize = (int)strlen(buf);
				sprintf(buf + strlen(buf), "%08X%08X", iFileSize, iTotleContextSize);
				len = strlen(buf);
				if (fwrite(buf, 1, len, pOutFile) != len)
				{
					bRet = false;
				}
				fflush(pOutFile);
				fclose(pOutFile);
			}

			return bRet;
		}

		char* MojingCompress::GetFileAppendInfo(const char * szSrcFile)
		{
			FILE *pFile = fopen(szSrcFile, "rb");
			if (!pFile)
				return NULL;
			fseek(pFile, 0, SEEK_END);
			int iFileSize = (int)ftell(pFile);
			if (iFileSize < 100)
			{
				fclose(pFile);
				return NULL;
			}
			fseek(pFile, -16, SEEK_END);
			char SizeInfo[24];
			memset(SizeInfo, 0, 24);
			fread(SizeInfo,1,16,pFile);
			int iZipFileSize, iContextSize;
			sscanf(SizeInfo ,"%08X%08X" , &iZipFileSize, &iContextSize);
			
			if (iFileSize != iZipFileSize + iContextSize + 16)
			{
				fclose(pFile);
				return NULL;
			}

			fseek(pFile, iZipFileSize, SEEK_SET);
			char *pBuffer = new char[iContextSize + 1];
			char *pRet = new char[iContextSize];
			memset(pBuffer, 0, iContextSize + 1);
			memset(pRet, 0, iContextSize );

			fread(pBuffer, 1 , iContextSize , pFile);
			if (strstr(pBuffer, "<APPEND><Context>") == pBuffer)
			{
				char *pContextEnd = strstr(pBuffer, "</Context><ContextSize>");
				if (pContextEnd != NULL && pContextEnd > pBuffer)
				{
					*pContextEnd = 0;
					strcpy(pRet, pBuffer + strlen("<APPEND><Context>"));
				}
			}
			delete[] pBuffer;
			fclose(pFile);
			return pRet;
		}
	}
}