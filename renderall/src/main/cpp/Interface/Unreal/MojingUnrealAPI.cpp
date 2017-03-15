#include "../../MojingAPI.h"
#include "MojingUnrealAPI.h"
#include "../../Base/MojingMath.h"
#include "../../MojingSDKStatus.h"
#include "../../Platform/MojingPlatformBase.h"

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

bool  Unreal_SetEngineVersion(const char* lpszEngine)
{
	return MojingSDK_SetEngineVersion(lpszEngine);
}

void  Unreal_Validate(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szChannelID)
{
	MojingSDK_Validate(szMerchantID, szAppID, szAppKey, szChannelID);
}

// API for tracking head
bool   Unreal_StartTracker(int nSampleFrequence)
{
	MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
	if (pStatus->GetEngineStatus() != ENGINE_UNREAL)
	{
		pStatus->SetEngine("Unreal");
		pStatus->SetEngineStatus(ENGINE_UNREAL);
	}

	return MojingSDK_StartTracker(nSampleFrequence);
}

int  Unreal_CheckSensors()
{
	return MojingSDK_CheckSensors();
}

void  Unreal_ResetSensorOrientation(void)
{
	MojingSDK_ResetSensorOrientation();
}

void  Unreal_ResetTracker(void)
{
	MojingSDK_ResetTracker();
}

int  Unreal_StartTrackerCalibration()
{
	return MojingSDK_StartTrackerCalibration();
}

float  Unreal_IsTrackerCalibrated()
{
	return MojingSDK_IsTrackerCalibrated();
}

void  Unreal_getLastHeadView(float* pfViewMatrix)
{
	Matrix4f Ret;
	MojingSDK_getLastHeadView((float*)&Ret.M[0][0]);
	memcpy(pfViewMatrix, (float*)&Ret.M[0][0], sizeof(float)* 16);
}

void  Unreal_getLastHeadEulerAngles(float* pfEulerAngles)
{
	//MojingSDK_getLastHeadEulerAngles(pfEulerAngles);
}

void  Unreal_getLastHeadQuarternion(float &w, float &x, float &y, float &z)
{
	Quatf Ret;
	MojingSDK_getLastHeadQuarternion(&Ret.w, &Ret.x, &Ret.y, &Ret.z);
	w = Ret.w;
	x = Ret.x;
	y = Ret.y;
	z = Ret.z;
}

void  Unreal_StopTracker(void)
{
	MojingSDK_StopTracker();
}

// API for distortion
bool  Unreal_EnterMojingWorld(const char * szGlassesName)
{
	return MojingSDK_EnterMojingWorld(szGlassesName, false, false);
}

bool  Unreal_ChangeMojingWorld(const char * szGlassesName)
{
	MOJING_FUNC_TRACE(g_APIlogger);
	return MojingSDK_ChangeMojingWorld(szGlassesName);
}

bool  Unreal_LeaveMojingWorld()
{
	return MojingSDK_LeaveMojingWorld();
}

int  Unreal_GetMojingWorldDistortionMesh(const char * szGlassesName, int iScreenWidth, int iScreenHeight, int iWidthCells, int iHeightCells, float *pFOV, float *pGlassesSeparationInMeter, void * pVerts, void * pIndices)
{
	return MojingSDK_GetDistortionMesh(szGlassesName, iScreenWidth, iScreenHeight, iWidthCells, iHeightCells, 
	pFOV, pGlassesSeparationInMeter,	
	pVerts, NULL, pIndices);
	//return MojingSDK_GetMojingWorldDistortionMesh(iScreenWidth, iScreenHeight, iWidthCells, iHeightCells, pVerts, pIndices);
}

float  Unreal_GetFOV()
{
	return MojingSDK_GetFOV();
}

float  Unreal_GetGlassesSeparation()
{
	return MojingSDK_GetGlassesSeparation();
}

float  Unreal_GetGlassesSeparationInPix()
{
	return MojingSDK_GetGlassesSeparationInPix();
}

bool  Unreal_DistortionSetLensSeparation(float fNewValue)
{
	return MojingSDK_DistortionSetLensSeparation(fNewValue);
}

float   Unreal_DistortionGetLensSeparation()
{
	return MojingSDK_DistortionGetLensSeparation();
}

// API for get glass info
void  Unreal_FreeString(char * pReleaseString)
{
	if (pReleaseString)
		free(pReleaseString);
}

/* Get a copy from String */
char*  Unreal_AllocString(const String &sReleaseString)
{
	char* pRet = (char*)malloc(sReleaseString.GetSize() + 1);
	strcpy(pRet, sReleaseString.ToCStr());
	return pRet;
}

const char*  Unreal_GetDefaultMojingWorld(const char* strLanguageCodeByISO639)
{
	return Unreal_AllocString(MojingSDK_GetDefaultMojingWorld(strLanguageCodeByISO639));
}

const char*  Unreal_GetLastMojingWorld(const char* strLanguageCodeByISO639)
{
	return Unreal_AllocString(MojingSDK_GetLastMojingWorld(strLanguageCodeByISO639));
}

const char*  Unreal_GetManufacturerList(const char* strLanguageCodeByISO639)
{
	return Unreal_AllocString(MojingSDK_GetManufacturerList(strLanguageCodeByISO639));
}

const char*  Unreal_GetProductList(const char* strManufacturerKey, const char* strLanguageCodeByISO639)
{
	return Unreal_AllocString(MojingSDK_GetProductList(strManufacturerKey, strLanguageCodeByISO639));
}

const char*  Unreal_GetGlassList(const char* strProductKey, const char* strLanguageCodeByISO639)
{
	return Unreal_AllocString(MojingSDK_GetGlassList(strProductKey, strLanguageCodeByISO639));
}

const char*  Unreal_GetGlassInfo(const char* strGlassKey, const char* strLanguageCodeByISO639)
{
	return Unreal_AllocString(MojingSDK_GetGlassInfo(strGlassKey, strLanguageCodeByISO639));
}

const char*  Unreal_GenerationGlassKey(const char* strProductQRCode, const char* strGlassQRCode)
{
	return Unreal_AllocString(MojingSDK_GenerationGlassKey(strProductQRCode, strGlassQRCode));
}

// API for get SDK status
bool  Unreal_GetInitSDK(void)
{
	return MojingSDK_GetInitSDK();
}

bool  Unreal_GetStartTracker(void)
{
	return MojingSDK_GetStartTracker();
}

bool  Unreal_GetInMojingWorld(void)
{
	return MojingSDK_GetInMojingWorld();
}

const char*  Unreal_GetSDKVersion(void)
{
	return MojingSDK_GetSDKVersion();
}

bool  Unreal_IsGlassesNeedDistortion(void)
{
	return MojingSDK_IsGlassesNeedDistortion();
}

bool  Unreal_IsLowPower(void)
{
	return MojingSDK_IsLowPower();
}

void  Unreal_getProjectionMatrix(int eye, bool bVrMode, float fFOV, float fNear, float fFar,
	float* pfProjectionMatrix, int* pfViewRect)
{
	MojingSDK_GetProjectionMatrix(eye, bVrMode, fFOV, fNear, fFar, pfProjectionMatrix, pfViewRect);
}

void  Unreal_AppSetContinueInterval(int interval)
{
	MojingSDK_AppSetContinueInterval(interval);
}

void  Unreal_AppSetReportInterval(int interval)
{
	MojingSDK_AppSetReportInterval(interval);
}

void  Unreal_AppSetReportImmediate(bool bImmediate)
{
	MojingSDK_AppSetReportImmediate(bImmediate);
}

void  Unreal_AppPageStart(const char* szPageName)
{
	MojingSDK_AppPageStart(szPageName);
}

void  Unreal_AppPageEnd(const char* szPageName)
{
	MojingSDK_AppPageEnd(szPageName);
}

void  Unreal_AppSetEvent(const char* szEventName, const char* szEventChannelID, const char* szEventInName, float eInData, const char* szEventOutName, float eOutData)
{
	MojingSDK_AppSetEvent(szEventName, szEventChannelID, szEventInName, eInData, szEventOutName, eOutData);
}

void  Unreal_AppReportLog(int iLogType, const char* szTypeName, const char* szLogContent)
{
	MojingSDK_ReportLog(iLogType, szTypeName, szLogContent, false);
}

const char* Unreal_AppGetUserID()
{
    return MojingPlatformBase::GetPlatform()->GetUserID();
}

const char* Unreal_AppGetRunID()
{
	return MojingPlatformBase::GetPlatform()->GetRunID();
}

void  Unreal_DoLog(LogLable iLable, const char* szLoginfo, const char * szFile, const char * szFunction, const unsigned int iFileLine)
{
	switch (iLable)
	{
	case LOGLABLE_TRACE:
		MOJING_TRACE(g_APIlogger, "Unreal.Log : " << szLoginfo << " [ " << szFunction << " " << szFile << ":" << iFileLine << " ] ");
		break;

	case LOGLABLE_ERROR:
		MOJING_ERROR(g_APIlogger, "Unreal.Log : " << szLoginfo << " [ " << szFunction << " " << szFile << ":" << iFileLine << " ] ");
		break;
	case LOGLABLE_WARRING:
		MOJING_WARN(g_APIlogger, "Unreal.Log : " << szLoginfo << " [ " << szFunction << " " << szFile << ":" << iFileLine << " ] ");
		break;

	default:
		break;
	}
}

