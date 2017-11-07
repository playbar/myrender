#include "ReporterTools.h"
#include "../3rdPart/AES/AESPro.h"
#ifdef ENABLE_LOGGER
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
extern MojingLogger g_APIlogger;
#endif

#if defined(MJ_OS_WIN32) || defined(MJ_OS_MAC)
#include <sys/time.h>
#endif

namespace Baofeng
{
	namespace Mojing
	{
		ReporterTools::ReporterTools()
		{
		}


		ReporterTools::~ReporterTools()
		{
		}

		int ReporterTools::GetCurrentTime()
		{
			struct timeval curtimeval;
#if defined(MJ_OS_WIN32)
			gettimeofday(&curtimeval, 0);
#else
			struct timezone curtimezone;
			gettimeofday(&curtimeval, &curtimezone);
#endif
			return (int)curtimeval.tv_sec;
		}

		//from 1970, ms
		int64_t ReporterTools::GetCurrentTimeMS()
		{
			struct timeval curtimeval;
#if defined(MJ_OS_WIN32)
			gettimeofday(&curtimeval, 0);
#else
			struct timezone curtimezone;
			gettimeofday(&curtimeval, &curtimezone);
#endif
			return int64_t((int64_t)curtimeval.tv_sec * 1000 + (int64_t)curtimeval.tv_usec / 1000);
		}

		char * ReporterTools::B2A(const char * Str, int iLength)
		{
			char *pRet = new char[iLength * 2 + 1];
			*(pRet + iLength * 2) = 0;
			for (int i = 0; i < iLength; i++)
			{
				sprintf(pRet + i * 2, "%02x", (unsigned char)Str[i]);
			}
			return pRet;
		}

		char * ReporterTools::AES_Value(const char * Str, int iLength)
		{
			int iBufferLength = (iLength + 15) / 16 * 16;
			char *pBuffer = new char[iBufferLength];
			//int iPadding = 16 - iLength % 16;
			//memset(pBuffer, iPadding, iBufferLength);
			memset(pBuffer, 0, iBufferLength);
			memcpy(pBuffer, Str, iLength);

			char *pEncBuffer = new char[iBufferLength + 16];
			memset(pEncBuffer, 0, iBufferLength + 16);
			CAESPro AP;
			AP.MakeKey("37f2da686534395e", "fedcba9876543210");
			AP.Encrypt(pBuffer, pEncBuffer, iBufferLength, CAESPro::CBC);

			char *pRet = B2A(pEncBuffer, iBufferLength);
			delete[] pBuffer;
			delete[] pEncBuffer;
			return pRet;
		}

		String ReporterTools::ConvertToUrlEncode(String S)
		{
            //MOJING_TRACE(g_APIlogger,"ReporterTools::ConvertToUrlEncode Begin.....");
            
            //MOJING_TRACE(g_APIlogger,"Convert From = " << S);
			String res = "";
			const char* p = S.ToCStr();
			// 首先判断是不是需要转换：判定是不是有字母和数字之外的字符
			const char *SaveChar = "-_.~"; // 未保留字
			char temp[8];
			for (unsigned int i = 0; i < S.GetSize(); i++)
			{
				unsigned char c = p[i];// 注意：这里如果用有符号数的话，负数在％02X会在iOS系统中打印出8个字节
				if (!isdigit(c) && !isalpha(c) && strchr(SaveChar, c) == NULL)
				{// 不是数字，不是字母，不是允许出现的未保留字
					
					sprintf(temp, "%%%02X", c);
					res += temp;
				}
				else
				{
					res += c;
				}
                //MOJING_TRACE(g_APIlogger,"Step " << i+1 << " = " << res);
			}
			//MOJING_TRACE(g_APIlogger,"ReporterTools::ConvertToUrlEncode End , Ret = " << res);
			return res;
		}

		String ReporterTools::AES_Value_S(String S)
		{
			String Ret;
			char *pEncValueString = AES_Value(S, (int)S.GetSize());
			Ret = pEncValueString;
			delete pEncValueString;
			return Ret;
		}

		String ReporterTools::BaofengEnc(String S)
		{
			String Ret;
			static char decrypt_str[] = " !_#$%&\'()*+,-.ABCDEFGHIJKLMNOP?@/0123456789:;<=>QRSTUVWXYZ[\\]^\"`nopqrstuvwxyzabcdefghijklm{|}~";
			String S1 = ConvertToUrlEncode(S);

			for (int iIndex = 0; iIndex < (int)S1.GetLength(); iIndex++)
			{
				char d = S1[iIndex];
				if (d >= 32 && d <= 126)
				{
					Ret += decrypt_str[d - 32];
				}
				else
				{
					Ret += d;
				}
			}
			
			return ConvertToUrlEncode(Ret);
		}

		String ReporterTools::BaofengEncEx(String S)
		{
			String Ret;
			static char decrypt_str[] = " !_#$%&\'()*+,-.ABCDEFGHIJKLMNOP?@/0123456789:;<=>QRSTUVWXYZ[\\]^\"`nopqrstuvwxyzabcdefghijklm{|}~";
			
			for (int iIndex = 0; iIndex < (int)S.GetLength(); iIndex++)
			{
				char d = S[iIndex];
				if (d >= 32 && d <= 126)
				{
					Ret += decrypt_str[d - 32];
				}
				else
				{
					Ret += d;
				}
			}

			return ConvertToUrlEncode(Ret);
		}

	}
}
