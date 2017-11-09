#include "GlassesConfigProfile.h"
#include "../Base/MojingTypes.h"
#include "../Distortion/MojingDistortion.h"
#include "../MojingManager.h"
#include "../Render/MojingRenderBase.h"
#include "../Platform/MojingPlatformBase.h"
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif
namespace Baofeng
{
	namespace Mojing
	{
		extern unsigned char g_EncKey[16];
		GlassesConfigProfile::GlassesConfigProfile()
		{
		}


		GlassesConfigProfile::~GlassesConfigProfile()
		{
		}
		
		
		bool GlassesConfigProfile::CheckJsonNode(JSON *pNode, void *pParam)
		{
			return pNode->GetItemByName("GlassesName") && pNode->GetItemByName("GlassesName")->GetStringValue() ==(const char*) pParam;
		}

		bool GlassesConfigProfile::UpdateFromProfile(const char * lpszGlassesName, const char * lpszProfilePath, JSON *pUpdateJson )
		{
			if (lpszGlassesName == NULL || lpszProfilePath == NULL)
				return false;
			if (*lpszGlassesName == 0 || *lpszProfilePath == 0)
				return false;

			bool bRet = false;
			/*来自安装包的节点*/
			char filename[256];
			strcpy(filename, lpszProfilePath);
			if (filename[strlen(filename) - 1] != '/')
				strcat(filename, "/");
			strcat(filename, "GlassesConfig.json");
			JSON * pJsonFromPacket = NULL; 
			pJsonFromPacket = SelectJsonNode(lpszGlassesName, filename);
			/*来自SD卡的节点*/
			JSON * pJsonFromSDCard = NULL;
			String LocalPath = MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath();
			if (LocalPath.Right(1) != "/")
				LocalPath += "/";

			pJsonFromSDCard = SelectJsonNode(lpszGlassesName, LocalPath + "GlassesConfig.dat");
			/*来自网络升级的节点*/
			JSON * pJsonFromUpdate = SelectJsonNode(lpszGlassesName, pUpdateJson);



			/*选择发布日期最晚的节点*/
			JSON *pUseNode = JSON_CompReleaseData(pJsonFromPacket , pJsonFromSDCard);
			pUseNode = JSON_CompReleaseData(pUseNode, pJsonFromUpdate);

			MOJING_TRACE(g_APIlogger, "Load profile from PACKET(Glass) = " << (pJsonFromPacket ? "YES" : "NO") << "; SDCARD = " << (pJsonFromSDCard ? "YES" : "NO") << "; Update = " << (pJsonFromUpdate ? "YES" : "NO"));
			
			
			/*使用该节点*/
			if (pUseNode)
			{
				if (pUseNode == pJsonFromPacket)
					MOJING_TRACE(g_APIlogger, "Using PACKET Profile");
				else if (pUseNode == pJsonFromSDCard)
					MOJING_TRACE(g_APIlogger, "Using SDCARD Profile");
				else if (pUseNode == pJsonFromUpdate)
					MOJING_TRACE(g_APIlogger, "Using UPDATE Profile");
				bRet = UpdateFromProfile(pUseNode);

				// 眼镜节点需要考虑合并的问题，不能直接保存！！
// 				if (pUseNode != pJsonFromSDCard)
// 					pUseNode->Save(LocalPath + "GlassesConfig.json");
			}
			else
			{
				MOJING_TRACE(g_APIlogger, "NO Profile.");
			}

			if (pJsonFromPacket)
				pJsonFromPacket->Release();
			if (pJsonFromSDCard)
				pJsonFromSDCard->Release();
			if (pJsonFromUpdate)
				pJsonFromUpdate->Release();

			return bRet;
		}

		JSON * GlassesConfigProfile::SelectJsonNode(const char * lpszGlassesName, const char * lpszJsonFileName)
		{
			JSON * pJson = JSON::Load(lpszJsonFileName, g_EncKey);
			JSON *pRet = NULL;
			if (pJson)
			{
				pRet = SelectJsonNode(lpszGlassesName , pJson);

				pJson->Release();
			}
			return pRet;
		}
		JSON* GlassesConfigProfile::SelectJsonNode(const char *lpszGlassesName, JSON * pJson)
		{
			JSON *pRet = NULL;
			if (pJson)
			{
				pRet = JSON_SelectNode(pJson, "GlassesConfig", 1, CheckJsonNode, (void*)lpszGlassesName);
			}
			return pRet;
		}
		bool GlassesConfigProfile::UpdateFromProfile(JSON *pJsonRoot)
		{
			bool bRet = false;
			JSON * pVersionNode = pJsonRoot->GetItemByName("ClassVersion");
			if (pJsonRoot != NULL && NULL != pVersionNode)
			{
				int iNodeVersion = pVersionNode->GetInt32Value();
				switch (iNodeVersion)
				{
				case 1:
					bRet = UpdateFromProfile_V1(pJsonRoot);
				default:
					break;
				}
			}


			return bRet;
		}
		bool GlassesConfigProfile::UpdateFromProfile_V1(JSON *pJsonRoot)
		{
			Distortion* pDistortion = Manager::GetMojingManager()->GetDistortion();

			JSON *pDistortionJson = pJsonRoot->GetItemByName("Distortion");
			if (NULL == pDistortionJson)
			{
				return false;
			}
			int Segments = pDistortionJson->GetItemByName("Segments")->GetInt32Value();
			if (Segments > 0)
			{
				float *fKG = new float[Segments];
				float *fKR = new float[Segments];
				float *fKB = new float[Segments];			
				
				float MetersPerTanAngleAtCenter = pDistortionJson->GetItemByName("MetersPerTanAngleAtCenter")->GetFloatValue();
				float LensSeparation = pDistortionJson->GetItemByName("LensSeparation")->GetFloatValue();
				float fFOV = pDistortionJson->GetItemByName("FOV")->GetFloatValue();

				for (int i = 0; i < Segments; i++)
				{
					fKG[i] = pDistortionJson->GetItemByName("KG")->GetItemByIndex(i)->GetFloatValue();
					fKR[i] = pDistortionJson->GetItemByName("KR")->GetItemByIndex(i)->GetFloatValue();
					fKB[i] = pDistortionJson->GetItemByName("KB")->GetItemByIndex(i)->GetFloatValue();
				}

				pDistortion->SetFOV(fFOV);
				pDistortion->SetMetersPerTanAngleAtCenter(MetersPerTanAngleAtCenter);
				pDistortion->SetLensSeparation(LensSeparation);
				pDistortion->SetDistortionParamet(Segments, false , fKR, fKG, fKB);
				delete[]fKG;
				delete[]fKR;
				delete[]fKB;
			}
			else
			{
				float fFOV = pDistortionJson->GetItemByName("FOV")->GetFloatValue();
				pDistortion->SetFOV(fFOV);
				pDistortion->SetDistortionParamet(0, true, NULL, NULL, NULL);
			}
			MojingRenderBase::SetModify();

			return true;
		}
	}
}