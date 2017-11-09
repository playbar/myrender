#include <map>
#include "../../MojingAPI.h"
#include "../../MojingManager.h"
#include "../../Parameters/MojingParameters.h"
#include "../../Base/GlUtils.h"
#include "../../Base/MojingMath.h"
#include "../../Platform/MojingPlatformBase.h"

#include "../../Render/MojingRenderMultiThread_3288.h"
#include "../../Render/Models/Model.h"
#include "../../Render/Models/SkyboxModel.h"
#include "../../Render/Models/SphereModel.h"
#include "../../Render/Models/RectangleModel.h"
#include "../../Render/GLProgram.h"

using namespace std;
using namespace Baofeng::Mojing;
typedef map<int, Model*> ModelMap;
ModelMap g_ModelMap;

int MojingVRRomAPI_CreateModel(int modelType, int textureType, const char* atlasJson)
{
	int iKey = -1;
	Model*pModel = Model::CreateModel((MODEL_TYPE)modelType, (TEXTURE_TYPE)textureType, atlasJson);
	if (pModel)
	{
		do 
		{
			iKey = rand();
		} while (iKey <= 0 || 
			g_ModelMap.find(iKey) != g_ModelMap.end());
		g_ModelMap[iKey] = pModel;
	}
	return iKey;
}
void MojingVRRomAPI_DrawModel(int iModelKey, int iEye, int iTextureId, float* pMVP)
{
	ModelMap::iterator it = g_ModelMap.find(iModelKey);
	if (it != g_ModelMap.end())
	{
		Model *pModel = it->second;
		pModel->RenderModel(iEye, iTextureId, pMVP);
	}
}

void MojingVRRomAPI_SetRectModelPosition(int iModelKey, float* pPosition)
{
	ModelMap::iterator it = g_ModelMap.find(iModelKey);
	if (it != g_ModelMap.end())
	{
		Model *pModel = it->second;

		if (0 == strcmp(pModel->GetClassName() , "RectangleModel"))
		{
			RectangleModel *pRectangleModel = (RectangleModel *)pModel;

			pRectangleModel->SetTopLeft(Vector3f(pPosition[0], pPosition[1], pPosition[2]));
			pRectangleModel->SetTopRight(Vector3f(pPosition[3], pPosition[4], pPosition[5]));
			pRectangleModel->SetBottomLeft(Vector3f(pPosition[6], pPosition[7], pPosition[8]));
			pRectangleModel->SetBottomRight(Vector3f(pPosition[9], pPosition[10], pPosition[11]));
		}
	}
}

void MojingVRRomAPI_ReleaseModel(int iModelKey)
{
	ModelMap::iterator it = g_ModelMap.find(iModelKey);
	if (it != g_ModelMap.end())
	{
		Model *pModel = it->second;
		g_ModelMap.erase(it);
		delete pModel;
	}
}
bool MojingVRRomAPI_DrawOverlay(int iOverlayIndex, int iLeftEyeTexID, int iRightEyeTexID, float fLeft, float fTop, float fWidth, float fHeight, float fDistanceInMetre)
{
	MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
	if (pDeviceParameters->GetIsMachine() && 0 == strcmp("rk3288", pDeviceParameters->GetModel()))
	{
		MojingRenderMultiThread_3288 *pRender = (MojingRenderMultiThread_3288 *)MojingRenderBase::GetCurrentRender();
		OverlayParmeat Parmeat;
		Parmeat.SetChanged(true);
		Parmeat.SetDrawCount(5);
		Vector4f v4LeftOverlayRect, v4RightOverlayRect;
		MojingSDK_Math_GetOverlayPosition3D(fLeft, fTop, fWidth, fHeight, fDistanceInMetre, v4LeftOverlayRect, v4RightOverlayRect);
		Parmeat.SetLeftOverlayTextureID(iLeftEyeTexID);
		Parmeat.SetRightOverlayTextureID(iRightEyeTexID);
		Parmeat.SetLeftOverlayRect(v4LeftOverlayRect);
		Parmeat.SetRightOverlayRect(v4RightOverlayRect);
#if USING_OVERLAY
		if (iOverlayIndex == 0)
		{
			pRender->SetRedrawAIMParmeat(Parmeat);
			return true;
		}
		else if (iOverlayIndex == 1)
		{
			pRender->SetRedrawOSDParmeat(Parmeat);
			return true;
		}
#endif
	}
	return false;
}

GlProgram	*g_pProg = NULL;
GlGeometryTriangles *g_pTriangles = NULL;

bool MojingVRRomAPI_CreateDistortion(void *pBuffer, int iLength)
{
#ifdef _DEBUG
	bool bNeedDeleteBuffer = false;
	if (pBuffer == NULL)
	{
		FILE* pFile = fopen("/sdcard/MojingSDK/DDD2.dat", "rb");
		if (pFile)
		{
			fseek(pFile, 0, SEEK_END);
			int iBufferSize = ftell(pFile);
			fseek(pFile, 0, SEEK_SET);
			pBuffer = malloc(iBufferSize);
			fread(pBuffer, 1, iBufferSize, pFile);
			fclose(pFile);

			bNeedDeleteBuffer = true;
		}
		else
		{
			return false;
		}
	}
#endif
	GlProgram Prog = MojingRenderBase::BuildProgram(VertexShader, FragmentShaderNoDispersion);
	g_pTriangles = new GlGeometryTriangles;
	if (g_pTriangles->BuildGeometry(0, 0, pBuffer))
	{
		g_pProg = new GlProgram;
		*g_pProg = Prog;
		return true;
	}
	else
	{
		delete g_pTriangles;
		g_pTriangles = NULL;
	}
#ifdef _DEBUG
	if (bNeedDeleteBuffer)
	{
		free(pBuffer);
	}
#endif
	return false;
}
bool MojingVRRomAPI_DrawDistortion(int iTextureBothEye, float * pM16A, float * pM16B)
{
	if (!g_pProg || !g_pTriangles)
		return false;

	const Matrix4f LeftEye(
		0.25, 0, 0, 0,
		0, 0.5, 0, 0,
		-0.25, -0.5, -1, -1,
		0, 0, 0, 0
		);
	const Matrix4f RightEye(
		0.25, 0, 0, 0,
		0, 0.5, 0, 0,
		-0.75, -0.5, -1, -1,
		0, 0, 0, 0
		);
	const Matrix4f DrawEyes[] = { LeftEye, RightEye };
	bool bUseClamp = true;
	glDepthMask(GL_FALSE);	// don't write to depth, even if Unity has depth on window
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_SCISSOR_TEST);// 允许区域剪裁
	glDisable(GL_BLEND);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	// BindWarpProgram
	static Matrix4f LandscapeOrientationMatrix = Matrix4f(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
	// BindEyeTextures

	glUseProgram(g_pProg->program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, iTextureBothEye);
	glUniformMatrix4fv(g_pProg->uMvp, 1, GL_FALSE, LandscapeOrientationMatrix.M[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glUniform1i(g_pProg->uTexture0, 0);
	// BindEyeTextures End
	g_pTriangles->BindVBO();

	for (int iEye = 0; iEye < 2; iEye++)
	{
		Matrix4f DrawEye = DrawEyes[iEye];
		glUniformMatrix4fv(g_pProg->uTexm, 1, GL_FALSE, DrawEye.M[0]);
		if (g_pProg->uTexClipX > 0)
		{
			// split screen clamping for UE4
			Vector2f clampX(0, 1.0);
			if (bUseClamp)
			{
				clampX.x = 0.5 * iEye + iEye * 0.001f;
				clampX.y = 0.5 * iEye + 0.5 - (1 - iEye) * 0.001f;
			}
			glUniform2fv(g_pProg->uTexClipX, 1, &clampX.x);
		}

		
		g_pTriangles->DrawElements(iEye);
	}

	// UnBindEyeTextures
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	g_pTriangles->UnBindVBO();
	return glGetError() == GL_NO_ERROR;
}
void MojingVRRomAPI_ReleaseDistortion()
{

}