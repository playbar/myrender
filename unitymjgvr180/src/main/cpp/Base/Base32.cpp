#include "Base32.h"
#include <string.h>
#include <ctype.h>
//                            0         1         2         3 
//                            01234567890123456789012345678901
const char g_Base32Table[] = "A4EDS2XC9WQYZHF8BGT3NVR5U7LKMP6J";
int BufferToBase32(const unsigned char* pInBuffer , int iLength  , char* pOutBuffer )
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

int Base32ToBuffer(const char* pInBuffer, unsigned char* pOutBuffer)
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

//                            0         1         2         3         4         5         6
//                            0123456789012345678901234567890123456789012345678901234567890123
const char g_Base64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '_') || (c == '-'));
}
int BufferToBase64(const unsigned char* pInBuffer, int iLength, char* pOutBuffer)
{
	if (pOutBuffer == 0)
	{
		return (iLength + 2) / 3 * 4 + 1;// Add 1 for Ascii String Zero
	}
	const unsigned char * pPos = pInBuffer;
	char * pRet = pOutBuffer;
	int  in_len = iLength;
	int i = 0, j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];
	while (in_len--) {
		char_array_3[i++] = *(pPos++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				*pRet++ = g_Base64Table[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			*pRet++ = g_Base64Table[char_array_4[j]];

		while ((i++ < 3))
			*pRet++ = '=';
	}
	*pRet = 0;
	return pRet - pOutBuffer + 1;
}

int Base64ToBuffer(const char* pInBuffer, unsigned char* pOutBuffer)
{
	int in_len = strlen(pInBuffer);
	if (pOutBuffer == 0)
		return in_len * 3 / 4;

	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	unsigned char* pRet = pOutBuffer;

	while (in_len-- && (pInBuffer[in_] != '=') && is_base64(pInBuffer[in_])) {
		char_array_4[i++] = pInBuffer[in_]; in_++;
		if (i == 4) {
			for (i = 0; i < 4; i++)
				char_array_4[i] = strchr(g_Base64Table, char_array_4[i]) - g_Base64Table;

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				*pRet++ = char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++)
			char_array_4[j] = 0;

		for (j = 0; j < 4; j++)
			char_array_4[j] = strchr(g_Base64Table, char_array_4[j]) - g_Base64Table;

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) *pRet++ = char_array_3[j];
	}

	return pRet - pOutBuffer;
}