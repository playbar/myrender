#pragma once
int MojingVRRomAPI_CreateModel(int modelType, int textureType, const char* atlasJson);
void MojingVRRomAPI_DrawModel(int iModelKey, int iEye, int iTextureId, float* pMVP);
void MojingVRRomAPI_SetRectModelPosition(int iModelKey, float* pPosition);
void MojingVRRomAPI_ReleaseModel(int iModelKey);

bool MojingVRRomAPI_DrawOverlay(int iOverlayIndex, int iLeftEyeTexID, int iRightEyeTexID, float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre);

bool MojingVRRomAPI_CreateDistortion(void *pBuffer, int iLength);
bool MojingVRRomAPI_DrawDistortion(int iTextureBothEye, float * pM16A, float * pM16B);
void MojingVRRomAPI_ReleaseDistortion();
