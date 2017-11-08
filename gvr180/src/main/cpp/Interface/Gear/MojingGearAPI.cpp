#include "MojingGearAPI.h"
#include "../../MojingAPI.h"
#include "../../MojingManager.h"
#include "../../Distortion/MojingDistortion.h"
#include "../../Profile/MojingProfileKey.h"
#include "../../Profile/UserSettingProfile.h"

#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif
bool   Gear_Init(int nWidth, int nHeight, float xdpi, float ydpi, const char *  Brand, const char *  Model, const char *  Serial , const char* ProfilePath)
{
	return MojingSDK_Init(nWidth, nHeight, xdpi, ydpi, Brand, Model, Serial, "", "", "", "", "", "", "", ProfilePath);
}
bool   Gear_StartTracker(int nSampleFrequence, const char* szGlassName/* = NULL*/)
{
	int nSensorOriginStatus = MojingSDK_GetSensorOrigin();
	MojingSDK_SetSensorOrigin(SENSOR_ORIGIN_LOCAL_NATIVE);
	bool bRet = MojingSDK_StartTracker(nSampleFrequence, szGlassName);
	MojingSDK_SetSensorOrigin(nSensorOriginStatus);
	return bRet;
}

void   Gear_StopTracker(void)
{
	MojingSDK_StopTracker();
}


bool   Gear_SetEngineVersion(const char* lpszEngine)
{
	return MojingSDK_SetEngineVersion(lpszEngine);
}

// API for tracking head
int   Gear_CheckSensors()
{
	return MojingSDK_CheckSensors();
}
void   Gear_ResetSensorOrientation(void)
{
	return MojingSDK_ResetSensorOrientation();
}

void   Gear_ResetTracker(void)
{
	return MojingSDK_ResetTracker();
}


void   Gear_getLastHeadView(float* pfViewMatrix)
{
	MojingSDK_getLastHeadView(pfViewMatrix);
}
void   Gear_getLastHeadEulerAngles(float* pfEulerAngles)
{
	MojingSDK_getLastHeadEulerAngles(pfEulerAngles);
}
void   Gear_getLastHeadQuarternion(float *w, float *x, float *y, float *z)
{
	MojingSDK_getLastHeadQuarternion(w, x, y, z);
}

bool  Gear_EnterMojingWorld()
{
	String sDefaultKey = MojingSDK_GetMojingWorldKey(MOJING_WORLDKEY_DEFAULT);
	if (MojingSDK_EnterMojingWorld(sDefaultKey, false, false))
	{
		MOJING_TRACE(g_APIlogger , "Using defalut MojingWorld");		
		return true;
	}
	return false;
}
bool  Gear_LeaveMojingWorld()
{
	return MojingSDK_LeaveMojingWorld();
}


#define WRITE_2_FILE 0
// API for distortion
int  Gear_GetMojingWorldDistortionMesh(int iWidthCells, int iHeightCells, float * pMeshBuffer, int iBufferLen)
{
	int iRet = -1;
	int iTotleCount = (iWidthCells + 1)* (iHeightCells + 1) * 6 * 2;
	int iTotleSize = iTotleCount * sizeof(float);
	if (iBufferLen >= iTotleSize)
	{// 测算Gear的参数
		Distortion* pDistortion = Manager::GetMojingManager()->GetDistortion();
#if WRITE_2_FILE
		FILE * pFile = fopen("/sdcard/MojingSDK/TTT.dat", "wb");
		FILE * pFile2 = fopen("/sdcard/MojingSDK/TTT2.dat", "wb");
		FILE * pFileText = fopen("/sdcard/MojingSDK/TTT.txt", "w");
		int iFileBufferCount = iTotleCount;
		int iFileBufferSize = iTotleSize;
		float *pFileBuffer = new float[iFileBufferCount];
		memset(pFileBuffer, 0, iFileBufferSize);
#endif
		float fFov = pDistortion->GetFOV();
		float fMa = pDistortion->GetMetersPerTanAngleAtCenter();
		float fMaNew = fMa * tan(fFov / 2 * PI / 180);
		Manager::GetMojingManager()->GetDistortion()->SetMetersPerTanAngleAtCenter(fMaNew);
		float *pBuffer2 = (float *)Manager::GetMojingManager()->GetDistortion()->BuildDistortionBuffer(iWidthCells, iHeightCells);
		float *bufferVerts = pBuffer2 + 3;
		float fKG[40];
		int iSegment = Manager::GetMojingManager()->GetDistortion()->GetDistortionParamet(NULL, fKG, NULL);
		float fKG_MAX = fKG[iSegment - 1];
		// Write temp file
		// 			FILE * pFile = fopen("/sdcard/MojingSDK/TTT.dat", "wb");
		// 			FILE * pFileText = fopen("/sdcard/MojingSDK/TTT.dat", "w");
		Manager::GetMojingManager()->GetDistortion()->SetMetersPerTanAngleAtCenter(fMa);

		for (int eye = 0; eye < 2; eye++)
		{
#if WRITE_2_FILE
			fprintf(pFileText, " float * pDistionBuffer_%s = {\n", eye == 0 ? "Left" : "Right");
#endif
			for (int y = 0; y <= iHeightCells; y++)
			{
				for (int x = 0; x <= iWidthCells; x++)
				{
					int iIndexXX = (y * (iWidthCells + 1) * 2 + eye * (iWidthCells + 1)+x) * 6;
					for (int i = 0; i < 6; i++)
					{
#if WRITE_2_FILE
						pFileBuffer[iIndexXX + i] = 
#endif
							pMeshBuffer[iIndexXX + i] = bufferVerts[(y*(iWidthCells + 1) * 2 + x + eye * (iWidthCells + 1)) * 7 + i];
					}
#if WRITE_2_FILE
					fprintf(pFileText, " /*%02d , %02d*/ %.4f ,  %.4f ,\n",
						y + 1, x + 1,
						pFileBuffer[2 + iIndexXX], pFileBuffer[2 + iIndexXX + 1]);
#endif
				}
#if WRITE_2_FILE
				fprintf(pFileText, "\n");
				fflush(pFileText);
#endif
			}
#if WRITE_2_FILE
			fprintf(pFileText, " };\n\n/***************************************/\n");
#endif
		}
#if WRITE_2_FILE
		fflush(pFileText);
		fclose(pFileText);

		fwrite(pFileBuffer, 1, iFileBufferSize, pFile);
		fflush(pFile);
		fclose(pFile);

		fwrite(pBuffer2, 1, 2 * (iWidthCells + 1)* (iHeightCells + 1)* 7 * sizeof(float)+12, pFile2);
		fflush(pFile2);
		fclose(pFile2);
		delete pBuffer2;
		delete pFileBuffer;
#endif
		iRet = 0;
	}
	else
	{
		MOJING_ERROR(g_APIlogger, "Cells : " << iWidthCells  << " x " << iHeightCells << " , Not Enough Memory : " << iBufferLen << " < " << iTotleSize);
		iRet = -1;
	}
	return iRet;// 内存不足
}

// API for get glass info
void   Gear_FreeString(char * pReleaseString)
{
	delete pReleaseString;
}
char*   Gear_AllocString(const char * pCopyString)
{
	char * szRet = new char[strlen(pCopyString) + 16 ];
	strcpy(szRet , pCopyString);
	return szRet;
}
const char*   Gear_GetManufacturerList(const char* strLanguageCodeByISO639)
{
	String ss = MojingSDK_GetManufacturerList(strLanguageCodeByISO639);
	const char*   pRet = Gear_AllocString(ss.ToCStr());
	return pRet;
}

const char*   Gear_GetProductList(const char* strManufacturerKey, const char* strLanguageCodeByISO639)
{
	String ss = MojingSDK_GetProductList(strManufacturerKey , strLanguageCodeByISO639);
	const char*   pRet = Gear_AllocString(ss.ToCStr());
	return pRet;
}
const char*   Gear_GetGlassList(const char* strProductKey, const char* strLanguageCodeByISO639)
{
	String ss = MojingSDK_GetGlassList(strProductKey, strLanguageCodeByISO639);
	const char*   pRet = Gear_AllocString(ss.ToCStr());
	return pRet;
}
const char*   Gear_GetGlassInfo(const char* strGlassKey, const char* strLanguageCodeByISO639)
{
	String ss = MojingSDK_GetGlassInfo(strGlassKey, strLanguageCodeByISO639);
	const char*   pRet = Gear_AllocString(ss.ToCStr());
	return pRet;
}
const char*   Gear_GenerationGlassKey(const char* strProductQRCode, const char* strGlassQRCode)
{
	String ss = MojingSDK_GenerationGlassKey(strProductQRCode, strGlassQRCode);
	const char*   pRet = Gear_AllocString(ss.ToCStr());
	return pRet;
}

// API for get SDK status
bool   Gear_GetInitSDK(void)
{
	return MojingSDK_GetInitSDK();
}
bool   Gear_GetStartTracker(void)
{
	return MojingSDK_GetStartTracker();
}
bool   Gear_GetInMojingWorld(void)
{
	return MojingSDK_GetInMojingWorld();
}
const char*   Gear_GetSDKVersion(void)
{
	return Gear_AllocString(MojingSDK_GetSDKVersion());
}
bool   Gear_IsGlassesNeedDistortion(void)
{
	return MojingSDK_IsGlassesNeedDistortion();
}


// API for mojing5
bool  Gear_IsLowPower(void)
{
	return MojingSDK_IsLowPower();
}

