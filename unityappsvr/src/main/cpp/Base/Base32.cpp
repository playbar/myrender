#include "Base32.h"
#include <string.h>
//                            0         1         2         3 
//                            01234567890123456789012345678901
const char g_Base32Table[] = "A4EDS2XC9WQYZHF8BGT3NVR5U7LKMP6J";
int Buffer2Base32(const unsigned char* pInBuffer , int iLength  , char* pOutBuffer )
{
	if (pOutBuffer != 0)
	{
		for (int i = 0; i < iLength; i++)
		{
			int iIndex1 = (pInBuffer[i] & 0xF0) >> 4;
			int iIndex2 = (pInBuffer[i] & 0x0F) + iIndex1;
			*pOutBuffer++ = g_Base32Table[iIndex1];
			*pOutBuffer++ = g_Base32Table[iIndex2];
			*pOutBuffer = 0;
		}
	}
	return iLength * 2 + 1;
}

int Base322Buffer(const char* pInBuffer, unsigned char* pOutBuffer)
{
	int iRet = 0;
	int iLength = (int)strlen(pInBuffer);
	if ((iLength & 1) != 0)
		return -1;
	for (int i = 0; i < iLength; i += 2)
	{
		unsigned int iIndex1 = (unsigned int) (strchr(g_Base32Table, pInBuffer[i]) - g_Base32Table);
		unsigned int iIndex2 = (unsigned int) (strchr(g_Base32Table, pInBuffer[i + 1]) - g_Base32Table - iIndex1);
		if (iIndex1 < 16 && iIndex2 < 16)
		{
			if (pOutBuffer != 0)
			{
				*pOutBuffer++ = ((iIndex1 << 4) | iIndex2);
				*pOutBuffer = 0;
			}
			iRet++;
		}
		else
		{
			return -1;
		}
	}

	return iRet;
}
