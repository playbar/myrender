#include "MobileConfigProfile.h"

#include "../MojingManager.h"
#include "../Base/MojingTypes.h"
#include "../Render/MojingRenderBase.h"
#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDeviceParameters.h"
#include "../Parameters/MojingDisplayParameters.h"
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
		char g_EliminateBuiltin[128] = "[]";

		MobileConfigProfile::MobileConfigProfile()
		{
		}


		MobileConfigProfile::~MobileConfigProfile()
		{
			MOJING_TRACE(g_APIlogger , ">>>>>>>>>>~MobileConfigProfile<<<<<<<<<<<");
		}

		bool MobileConfigProfile::UpdateFromProfile(const char * lpszProfilePath , JSON *pUpdateJsonFile)
		{
			String sDeviceModel;
			MojingDeviceParameters *pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
			if (pDeviceParameters)
			{
				sDeviceModel = pDeviceParameters->GetModel();
				MOJING_TRACE(g_APIlogger, "Update profile , using Model = " << sDeviceModel);
			}
			else
			{
				MOJING_ERROR(g_APIlogger, "Can not get mobile Model");
				return false;
			}
			

			bool bRet = false;
			/*来自安装包的节点*/
			char filename[256];
			strcpy(filename, lpszProfilePath);
			if (filename[strlen(filename) - 1] != '/')
				strcat(filename, "/");
			strcat(filename, "MobileConfig.json");
			JSON * pJsonFromPacket = NULL;
			pJsonFromPacket = SelectJsonNode(sDeviceModel, filename);
			/*来自SD卡的节点*/
			JSON * pJsonFromSDCard = NULL;
			String LocalPath = MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath();
			if (LocalPath.Right(1) != "/")
				LocalPath += "/";
			
			pJsonFromSDCard = SelectJsonNode(sDeviceModel, LocalPath + "MobileConfig.dat");
			// 注意：如果是历史上由于BUG产生的存在无名节点的情况，这个本地配置文件应该是失效的// 20151113
			if (pJsonFromSDCard && pJsonFromSDCard->GetItemByName(""))
			{
				pJsonFromSDCard->Release();
				pJsonFromSDCard = NULL;
				// 删除掉这个文件
				remove(LocalPath + "MobileConfig.dat");
			}
			// 删除本地错误配置文件结束

			/*来自网络升级的节点*/
			JSON* pJsonFromUpdate = SelectJsonNode(sDeviceModel, pUpdateJsonFile);


			MOJING_TRACE(g_APIlogger, "Load profile from PACKET = " << (pJsonFromPacket ? "YES" : "NO") << "; SDCARD = " << (pJsonFromSDCard ? "YES" : "NO") << "; Update = " << (pJsonFromUpdate ? "YES" : "NO"));

			/*选择发布日期最晚的节点*/
			// 注意：优先返回第一个元素
			char ReleaseData[16] = {0};
			JSON *pUseNode = JSON_CompReleaseData(pJsonFromSDCard, pJsonFromPacket, ReleaseData);
			pUseNode = JSON_CompReleaseData(pUseNode, pJsonFromUpdate, ReleaseData);

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
				// 记录最新的配置文件

				MojingPlatformBase::GetPlatform()->SetMobileConfigVersion(ReleaseData);
				
				// 手机节点不需要考虑合并的问题，直接保存就好了
				if (pUseNode != pJsonFromSDCard)
				{
					SaveToSDCardProfile(pUseNode , LocalPath + "MobileConfig.dat");
					// pUseNode->Save(LocalPath + "MobileConfig.json");
				}
			}

			if (pJsonFromPacket)
			{
				pJsonFromPacket->Release();
			}
			if (pJsonFromSDCard)
			{
				pJsonFromSDCard->Release();
			}
			return bRet;

		}

		bool MobileConfigProfile::CheckJsonNode(JSON *pNode, void *pParam)
		{
			return pNode->GetItemByName("Mobile") && pNode->GetItemByName("Mobile")->GetStringValue() == (const char*)pParam;
		}

		JSON* MobileConfigProfile::SelectJsonNode(const char *lpszMobileName, const char* filename)
		{
			const char* szError = NULL;
			JSON * pJson = JSON::Load(filename, g_EncKey, &szError);
			JSON *pRet = NULL;
			if (pJson)
			{
				pRet = SelectJsonNode(lpszMobileName, pJson);
				pJson->Release();
			}
			return pRet;
		}
		JSON* MobileConfigProfile::SelectJsonNode(const char *lpszMobileName, JSON * pJson)
		{
			JSON *pRet = NULL;
			if (pJson)
			{
				pRet = JSON_SelectNode(pJson, "MobileConfig", 1, CheckJsonNode, (void*)lpszMobileName);
			}
			return pRet;
		}
		bool MobileConfigProfile::UpdateFromProfile(JSON *pJsonRoot)
		{
			bool bRet = false;
			if (pJsonRoot->GetItemByName("ClassVersion"))
			{
				int iNodeVersion = pJsonRoot->GetItemByName("ClassVersion")->GetInt32Value();
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
		bool MobileConfigProfile::UpdateFromProfile_V1(JSON *pJsonRoot)
		{
			MojingDeviceParameters *pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
			MojingDisplayParameters *pDisplayParameters = Manager::GetMojingManager()->GetParameters()->GetDisplayParameters();
			MojingFactoryCalibrationParameters *pFactoryCalibrationParameters = Manager::GetMojingManager()->GetParameters()->GetFactoryCalibrationParameters();

			bool bUpdate = false;
			JSON *pDisplayJson = pJsonRoot->GetItemByName("Display");
			if (pDisplayJson)
			{
				JSON *pPPI = pDisplayJson->GetItemByName("PPI");
				if (pPPI != NULL)
				{
					double dPPI = pDisplayJson->GetItemByName("PPI")->GetDoubleValue();
					pDisplayParameters->SetPPI(dPPI);
					MOJING_TRACE(g_APIlogger, "Update PPI=" << dPPI);
				}
			}
			else
			{
				pDisplayParameters->SetPPI(0);// 无修正 
			}
			pDisplayParameters->UpdatePPIFromUserSetting();// 无论把PPI更新成什么数，当有用户设置的时候这里都是必须使用用户的设置。

			JSON *pSensorJson = pJsonRoot->GetItemByName("Sensor");
			if (pSensorJson && !pFactoryCalibrationParameters->GetHaveLoaclProfile())
			{
				pFactoryCalibrationParameters->FromJson(pSensorJson);
				MOJING_TRACE(g_APIlogger, "Update CalibrationParameters");
				bUpdate = true;
			}
			
			JSON *pEdgeJson = pJsonRoot->GetItemByName("Edge");
			if (pEdgeJson)
			{
				float fEdge = pEdgeJson->GetDoubleValue();
				pDisplayParameters->SetMobileEdge(fEdge);
				MOJING_TRACE(g_APIlogger, "Update Edge=" << fEdge);
				bUpdate = true;
			}
			else
			{
				float fEdge = 0.004;
				MOJING_TRACE(g_APIlogger, "Using Defaule Edge=" << fEdge);
				pDisplayParameters->SetMobileEdge(fEdge);// 无修正 
			}

			JSON *pEliminateBuiltin = pJsonRoot->GetItemByName("EliminateBuiltin");
			if (pEliminateBuiltin)
			{
				char * strEliminateBuiltin = pEliminateBuiltin->PrintValue(0, false);
				sprintf(g_EliminateBuiltin, "%s", strEliminateBuiltin);
				MJ_FREE(strEliminateBuiltin);
				bUpdate = true;
			}

			JSON *pSensorFromJava = pJsonRoot->GetItemByName("SensorFromJava");
			if (pSensorFromJava)
			{
				bool bSensorDataFromJava = pSensorFromJava->GetBoolValue();
				if (pDeviceParameters)
				{
					pDeviceParameters->SetSensorDataFromJava(bSensorDataFromJava);
					if (bSensorDataFromJava)
					{
						MOJING_TRACE(g_APIlogger, "Always get sensor data from Java.");
						UserSettingProfile* pProfile = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile();
						if (pProfile && pProfile->GetSensorDataFromJava() == false)
						{
							pProfile->SetSensorDataFromJava(true);
							pProfile->Save();
						}
					}
				}
				bUpdate = true;
			}

			return bUpdate;
		}
		
		String MobileConfigProfile::GetEliminateBuiltin()
		{
			return g_EliminateBuiltin;
		}
		void MobileConfigProfile::ReplaceJsonNode(JSON *pSrc, JSON *pTarget , const char* szNodeName)
		{
			JSON *pOLD = pTarget->GetItemByName(szNodeName);
			JSON *pNEW = pSrc->GetItemByName(szNodeName);
			if (pOLD == NULL && pNEW == NULL)
				return;

			if (pOLD != NULL &&
				pNEW != NULL )
			{
				char * sOLD = pOLD->PrintValue(0, false);
				char * sNEW = pNEW->PrintValue(0, false);
				bool bSame = (strcmp(sOLD, sNEW) == 0);
				MJ_FREE(sOLD);
				MJ_FREE(sNEW);
				if (bSame)
					return;
			}

			if (pOLD)
			{
				pOLD->RemoveNode();
				pOLD->Release();
				MOJING_TRACE(g_APIlogger, "Remove node :" << szNodeName);
			}

			if (pNEW)
			{
				pTarget->AddItem(szNodeName, pNEW->Clon());
				MOJING_TRACE(g_APIlogger, "Append node:" << szNodeName);
			}
#ifdef _DEBUG
			char* s = pTarget->PrintValue(0, true);
			MOJING_TRACE(g_APIlogger, s);
			MJ_FREE(s);
#endif
		}
		bool MobileConfigProfile::SaveToSDCardProfile(JSON *pJsonNode , String strFileName)
		{
			JSON * pLocalJsonFile = JSON::Load(strFileName, g_EncKey);
			if (pLocalJsonFile == NULL)
			{
				pLocalJsonFile = JSON::CreateObject();
				pLocalJsonFile->AddStringItem("Class", "MobileConfig");
				pLocalJsonFile->AddItem("MobileConfig", pJsonNode->Clon());
			}
			else
			{
				JSON *pMobileConfig = pLocalJsonFile->GetItemByName("MobileConfig");
				if (pMobileConfig)
				{
					ReplaceJsonNode(pJsonNode, pMobileConfig, "Display");
					ReplaceJsonNode(pJsonNode, pMobileConfig, "Sensor");
					ReplaceJsonNode(pJsonNode, pMobileConfig, "Edge");
					ReplaceJsonNode(pJsonNode, pMobileConfig, "ReleaseDate");
					ReplaceJsonNode(pJsonNode, pMobileConfig, "EliminateBuiltin");
					ReplaceJsonNode(pJsonNode, pMobileConfig, "SensorFromJava");
				}
				else
				{
					pLocalJsonFile->AddItem("MobileConfig", pJsonNode->Clon());
				}
			}

			pLocalJsonFile->Save(strFileName, g_EncKey);

			return true;
		}
	}
}