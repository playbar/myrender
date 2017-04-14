#include "DayDreamParameters.h"
#include "../../Base/MojingStd.h"
#include "../../Base/Base32.h"

#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif
#define USE_PROTOBUF 0
#if USE_PROTOBUF
#include "../../3rdPart/Protobuf/CardboardDevice.pb.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif // ENABLE_LOGGER

CDayDreamParameters::CDayDreamParameters()
{
}


CDayDreamParameters::~CDayDreamParameters()
{
}

int CDayDreamParameters::UpdateDayDreamURL(const char* szDayDreamURL, char * szNewDayDreamURL, float fPPI_Scale)
{
	if (szNewDayDreamURL == NULL)
	{
		return (strlen(szDayDreamURL) + 3 )/ 4 * 4 + 1;
	}

	int iRet = 0;
	int iBufferSize = Base64ToBuffer(szDayDreamURL, NULL);
	if (iBufferSize)
	{
		unsigned char * pDDParametersBuffer = new unsigned char[iBufferSize + 16];
		memset(pDDParametersBuffer, 0, iBufferSize + 16);
		int iLength = Base64ToBuffer(szDayDreamURL, pDDParametersBuffer);
		// pDDParametersBuffer_New 是存放新的二进制格式参数的缓冲区
		unsigned char * pDDParametersBuffer_New = new unsigned char[iBufferSize + 16];
		memset(pDDParametersBuffer_New, 0, iBufferSize + 16);
		*szNewDayDreamURL = 0;

#if USE_PROTOBUF
		DeviceParams D;
		if (D.ParsePartialFromArray(pDDParametersBuffer , iLength))
		{
			if (D.has_screen_to_lens_distance() && D.has_inter_lens_distance())
			{
				float fFixA = (int)(D.screen_to_lens_distance() * fPPI_Scale * 10000 + 5);
				float fFixB = (int)(D.inter_lens_distance() * fPPI_Scale * 10000 + 5);
				fFixA /= 10000;
				fFixB /= 10000;
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "Replace DURL Parm :  x" << fPPI_Scale);
				MOJING_TRACE(g_APIlogger, "Replace DURL Parm : " << D.screen_to_lens_distance() << " --> " << fFixA);
				MOJING_TRACE(g_APIlogger, "Replace DURL Parm : " << D.inter_lens_distance() << " --> " << fFixB);
#endif
				D.set_screen_to_lens_distance(fFixA);
				D.set_inter_lens_distance(fFixB);
				
				if (D.SerializePartialToArray(pDDParametersBuffer_New, iBufferSize))
				{
					iRet = BufferToBase64(pDDParametersBuffer_New, iLength, szNewDayDreamURL);;
				}
			}
		}
		

#else
		/*
		0x7AC68880  0a 0d 62 61 6f 66 65 6e 67 6d 6f 6a 69 6e 67 12  ..baofengmojing.
		0x7AC68890  0e 6d 6f 6a 69 6e 67 20 50 6c 75 73 20 33 42 1d  .mojing Plus 3B.
		0x7AC688A0  a1 67 33 3d 25 b6 f3 7d 3d 2a 10 00 00 30 42 00  ?g3=%??}=*...0B.
		0x7AC688B0  00 30 42 00 00 30 42 00 00 30 42 58 01 35 29 5c  .0B..0B..0BX.5)\
		0x7AC688C0  0f 3d 3a 08 cd cc 4c 3d 3d 0a 57 3e 50 00 60 00  .=:.??L==.W>P.`.
		*/
		memcpy(pDDParametersBuffer_New, pDDParametersBuffer, iLength);
		unsigned char *pPos = pDDParametersBuffer_New;
		if (*pPos++ == 0x0A)
		{
			pPos += *pPos + 1;
			if (*pPos++ == 0x12)
			{
				pPos += *pPos + 1;
				if (*pPos++ == 0x1D)
				{
					float screen_to_lens_distance;
					unsigned char *pScreen_to_lens_distance = pPos;
					memcpy(&screen_to_lens_distance, pScreen_to_lens_distance, 4);
					pPos += 4;
					if (*pPos++ == 0x25)
					{
						float inter_lens_distance;
						unsigned char *pinter_lens_distance = pPos;
						memcpy(&inter_lens_distance, pinter_lens_distance, 4);
						float fFixA = (int)(screen_to_lens_distance * fPPI_Scale * 10000 + 5);
						float fFixB = (int)(inter_lens_distance * fPPI_Scale * 10000 + 5);
						fFixA /= 10000;
						fFixB /= 10000;
#ifdef _DEBUG
						MOJING_TRACE(g_APIlogger, "Replace DURL Parm :  x" << fPPI_Scale);
						MOJING_TRACE(g_APIlogger, "Replace DURL Parm : " << screen_to_lens_distance << " --> " << fFixA);
						MOJING_TRACE(g_APIlogger, "Replace DURL Parm : " << inter_lens_distance << " --> " << fFixB);
#endif
						memcpy(pScreen_to_lens_distance, &fFixA, 4);
						memcpy(pinter_lens_distance, &fFixB, 4);
					}
				}
			}
		}
#endif


		iRet = BufferToBase64(pDDParametersBuffer_New, iLength, szNewDayDreamURL);
		while (char * pPos = strchr(szNewDayDreamURL, '='))
		{
			*pPos = 0;
		}
		delete[] pDDParametersBuffer_New;
		delete[] pDDParametersBuffer;
	}
	return iRet;
}
