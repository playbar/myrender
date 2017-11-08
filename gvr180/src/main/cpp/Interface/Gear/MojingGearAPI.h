#pragma once
#include "../../Base/MojingTypes.h"
#ifdef MJ_OS_ANDROID
#ifdef __cplusplus
extern "C" {
#endif
bool   Gear_Init(int nWidth, int nHeight, float xdpi, float ydpi, const char *  Brand, const char *  Model, const char *  Serial, const char* ProfilePath);
bool   Gear_SetEngineVersion(const char* lpszEngine);

// API for tracking head
bool   Gear_StartTracker(int nSampleFrequence, const char* szGlassName = NULL);
void   Gear_StopTracker(void); 
int    Gear_CheckSensors();
void   Gear_ResetSensorOrientation(void);
void   Gear_ResetTracker(void);


void   Gear_getLastHeadView(float* pfViewMatrix);
void   Gear_getLastHeadEulerAngles(float* pfEulerAngles);
void   Gear_getLastHeadQuarternion(float *w, float *x, float *y, float *z);

// API for distortion
// API for distortion
bool  Gear_EnterMojingWorld();
bool  Gear_LeaveMojingWorld();

int  Gear_GetMojingWorldDistortionMesh(int iWidthCells, int iHeightCells, float * pMeshBuffer, int iBufferLen);

// API for get glass info
void   Gear_FreeString(char * pReleaseString);
const char*   Gear_GetManufacturerList(const char* strLanguageCodeByISO639);
const char*   Gear_GetProductList(const char* strManufacturerKey, const char* strLanguageCodeByISO639);
const char*   Gear_GetGlassList(const char* strProductKey, const char* strLanguageCodeByISO639);
const char*   Gear_GetGlassInfo(const char* strGlassKey, const char* strLanguageCodeByISO639);
const char*   Gear_GenerationGlassKey(const char* strProductQRCode, const char* strGlassQRCode);

// API for get SDK status
bool   Gear_GetInitSDK(void);
bool   Gear_GetStartTracker(void);
bool   Gear_GetInMojingWorld(void);
const char*   Gear_GetSDKVersion(void);
bool   Gear_IsGlassesNeedDistortion(void);

//API for log
enum LogLable
{
	LOGLABLE_TRACE,
	LOGLABLE_WARRING,
	LOGLABLE_ERROR
};

#ifdef __cplusplus
}
#endif

#endif //MJ_OS_ANDROID