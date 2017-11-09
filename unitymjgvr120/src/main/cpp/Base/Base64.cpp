#include "Base64.h"


namespace Baofeng
{
	namespace Mojing
	{
		int Base64::Decode(const char *pInBuffer, char *pOutBuffer)
		{
			int length = strlen(pInBuffer);
			if (pOutBuffer == 0)
				return length * 3 / 4;
			//解码表
			const char DecodeTable[] =
			{
				(char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-1, (char)-1, (char)-2, (char)-2, (char)-1, (char)-2, (char)-2, (char)
				(char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-1, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)62, (char)-2, (char)-2, (char)-2, (char)63, (char)
				(char)52, (char)53, (char)54, (char)55, (char)56, (char)57, (char)58, (char)59, (char)60, (char)61, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-2, (char)0, (char)1, (char)2, (char)3, (char)4, (char)5, (char)6, (char)7, (char)8, (char)9, (char)10, (char)11, (char)12, (char)13, (char)14, (char)
				(char)15, (char)16, (char)17, (char)18, (char)19, (char)20, (char)21, (char)22, (char)23, (char)24, (char)25, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-2, (char)26, (char)27, (char)28, (char)29, (char)30, (char)31, (char)32, (char)33, (char)34, (char)35, (char)36, (char)37, (char)38, (char)39, (char)40, (char)
				(char)41, (char)42, (char)43, (char)44, (char)45, (char)46, (char)47, (char)48, (char)49, (char)50, (char)51, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)
				(char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2, (char)-2
			};
			int bin = 0, i = 0, pos = 0;
			char* pRet = pOutBuffer;
			const char *current = pInBuffer;
			char ch;
			while ((ch = *current++) != '\0' && length-- > 0)
			{
				if (ch == base64_pad) { // 当前一个字符是“=”号
					/*
					先说明一个概念：在解码时，4个字符为一组进行一轮字符匹配。
					两个条件：
					1、如果某一轮匹配的第二个是“=”且第三个字符不是“=”，说明这个带解析字符串不合法，直接返回空
					2、如果当前“=”不是第二个字符，且后面的字符只包含空白符，则说明这个这个条件合法，可以继续。
					*/
					if (*current != '=' && (i % 4) == 1) {
						return NULL;
					}
					continue;
				}
				ch = DecodeTable[ch];
				//这个很重要，用来过滤所有不合法的字符
				if (ch < 0) { /* a space or some other separator character, we simply skip over */
					continue;
				}
				switch (i % 4)
				{
				case 0:
					bin = ch << 2;
					break;
				case 1:
					bin |= ch >> 4;
					*(pRet++) = bin;
					bin = (ch & 0x0f) << 4;
					break;
				case 2:
					bin |= ch >> 2;
					*(pRet++) = bin;
					bin = (ch & 0x03) << 6;
					break;
				case 3:
					bin |= ch;
					*(pRet++) = bin;
					break;
				}
				i++;
			}
			return pRet - pOutBuffer;
		}
	}
}