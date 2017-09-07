#include "GlassesConfigProfileV2.h"
#include <sys/time.h>
#include <math.h>
#include "../MojingSDKStatus.h"

#include "../Base/Base32.h"
#include "../Base/CRC.h"
#include "../Base/MojingTimer.h"
#include "../3rdPart/MD5/MD5.h"
#include "../Platform/MojingPlatformBase.h"
#include "../Distortion/MojingDistortion.h"
#include "../MojingManager.h"
#include "../Render/MojingRenderBase.h"
#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDisplayParameters.h"
#include "../Profile/ProfileThreadMGR.h"
#include "../Reporter/ReporterTools.h"

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
		GlassesConfigProfileV2::GlassesConfigProfileV2() :
			m_uiAppID(0), m_uiReleaseDate(20150101)
		{
			const char* szAppID = MojingPlatformBase::GetPlatform()->GetAppID();
			if ((szAppID == NULL) || (atoi(szAppID) == 0))
			{
				String szAppName = MojingPlatformBase::GetPlatform()->GetAppName();
				SetAppID(GenerationAppID(szAppName));
			}
			else
			{
				SetAppID((unsigned int)atoi(szAppID));
			}
			SetClassName(__FUNCTION__);
		}

		GlassesConfigProfileV2::~GlassesConfigProfileV2()
		{
			Release();
		}
		// 如果APPID和PlatformID缺失的话，补充上这两个信息
		void GlassesConfigProfileV2::MakeFinalKey(MojingProfileKey& Key)
		{
			if (Key.GetAppID() == 0)
				Key.SetAppID(GetAppID());

			if (Key.GetPlatformID() == 0)
				Key.SetPlatformID(MojingPlatformBase::GetPlatformID());

		}
		unsigned int GlassesConfigProfileV2::GenerationAppID(const char* szAppName)
		{
			int iBegin = 0;
#if defined(MJ_OS_ANDROID)
			iBegin = 1;
#elif defined(MJ_OS_IPHONE)
			iBegin = 2;
#endif
			int iRet = GetCrc16(iBegin, (unsigned char*)szAppName, (int)strlen(szAppName));

			unsigned char *szKey = (unsigned char *)&iRet;
			int iLength = (int)strlen(szAppName);
			int i;
			for (i = iBegin; szAppName[i] != 0; i++)
			{
				szKey[i % 4] ^= (szAppName[i] + iLength);
			}

			for (i = iLength; i >= 0; i--)
			{
				szKey[i % 4] ^= (szAppName[i]);
			}

			szKey[0] &= 0x3F;

#if defined(MJ_OS_ANDROID)
			szKey[0] |= 0x40;
#elif defined(MJ_OS_IPHONE)
			szKey[0] |= 0x80;
#endif
			return iRet;
		}
		bool GlassesConfigProfileV2::UpdateManufacturer(JSON *pManufacturerArray)
		{
			ReleaseManufacturer();
			for (int iIndex = 0; iIndex < pManufacturerArray->GetArraySize(); iIndex++)
			{
				JSON *pManufacturer = pManufacturerArray->GetItemByIndex(iIndex);
				ManufacturerInfo *pNewNode = new ManufacturerInfo;
				if (pNewNode->FromJson(pManufacturer) && (m_ManufacturerMap.size() == 0 || m_ManufacturerMap.find(pNewNode->GetID()) == m_ManufacturerMap.end()))
				{
					m_ManufacturerMap[pNewNode->GetID()] = pNewNode;
				}
				else
				{
					delete pNewNode;
				}
			}
			return m_ManufacturerMap.size() > 0;
		}
		bool GlassesConfigProfileV2::UpdateMachineList(JSON *pMachineList)
		{
			for (int iIndex = 0; iIndex < pMachineList->GetArraySize(); iIndex++)
			{
				JSON *pJsonNode = pMachineList->GetItemByIndex(iIndex);
				JSON *pID = pJsonNode->GetItemByName("ID");
				JSON *pBrand = pJsonNode->GetItemByName("Brand");
				JSON *pModel = pJsonNode->GetItemByName("Model");
				JSON *pRender = pJsonNode->GetItemByName("Render");// 可选

				if (pID && pBrand && pModel &&
					pBrand->Type == JSON_String && pModel->Type == JSON_String &&
					pID->Type == JSON_Number)
				{
					int iID = pID->GetInt32Value();
					String szBrand = pBrand->GetStringValue();
					String szModel = pModel->GetStringValue();
					int iRender = 0;
					if (pRender && pRender->Type == JSON_Number)
					{
						iRender = pRender->GetInt32Value();
					}
					MojingDeviceParameters::AppendMachine(iID, szBrand, szModel, iRender);
					MOJING_TRACE(g_APIlogger, "Add Machine :: " << szBrand << "-" << szModel);
				}
			}
			return true;
		}
		bool GlassesConfigProfileV2::UpdateProduct(JSON *pProductArray)
		{
			ReleaseProduct();
			for (int iIndex = 0; iIndex < pProductArray->GetArraySize(); iIndex++)
			{
				JSON *pProduct = pProductArray->GetItemByIndex(iIndex);
				ProductInfo *pNewNode = new ProductInfo;
				if (pNewNode->FromJson(pProduct) &&
					pNewNode->CheckEnable() &&
					(m_ProductMap.size() == 0 || m_ProductMap.find(pNewNode->GetID()) == m_ProductMap.end()))
				{
					m_ProductMap[pNewNode->GetID()] = pNewNode;
				}
				else
				{
					MOJING_TRACE(g_APIlogger, "Disable PID = " << pNewNode->GetID());
					delete pNewNode;
				}
			}
			return m_ProductMap.size() > 0;
		}

		bool GlassesConfigProfileV2::UpdateGlass(JSON *pGlassArray)
		{
			ReleaseGlass();
			for (int iIndex = 0; iIndex < pGlassArray->GetArraySize(); iIndex++)
			{
				JSON *pGlass = pGlassArray->GetItemByIndex(iIndex);
				GlassInfo *pNewNode = new GlassInfo;
				if (pNewNode->FromJson(pGlass) && (m_GlassMap.size() == 0 || m_GlassMap.find(pNewNode->GetID()) == m_GlassMap.end()))
				{
					m_GlassMap[pNewNode->GetID()] = pNewNode;
				}
				else
				{
					delete pNewNode;
				}
			}
			return m_GlassMap.size() > 0;
		}

		void GlassesConfigProfileV2::Release()
		{
			ReleaseManufacturer();
			ReleaseProduct();
			ReleaseGlass();
		}
		void GlassesConfigProfileV2::ReleaseManufacturer()
		{
			std::map<unsigned int, ManufacturerInfo*>::iterator it = m_ManufacturerMap.begin();
			while (it != m_ManufacturerMap.end())
			{
				delete it->second;
				it++;
			}
			m_ManufacturerMap.clear();
		}
		void GlassesConfigProfileV2::ReleaseProduct()
		{
			std::map<unsigned int, ProductInfo*>::iterator it = m_ProductMap.begin();
			while (it != m_ProductMap.end())
			{
				delete it->second;
				it++;
			}
			m_ProductMap.clear();
		}
		void GlassesConfigProfileV2::ReleaseGlass()
		{
			std::map<unsigned int, GlassInfo*>::iterator it = m_GlassMap.begin();
			while (it != m_GlassMap.end())
			{
				delete it->second;
				it++;
			}
			m_GlassMap.clear();
		}
		JSON* GlassesConfigProfileV2::ToJson()
		{
			return NULL;
		}

		bool GlassesConfigProfileV2::FromJson(JSON* pJson)
		{
			if (pJson == NULL)
				return false;

			if (ReleaseDateFromJson(pJson))
			{
				if (JSON *pSDKLimit = pJson->GetItemByName("SDKLimit"))
				{
					uint64_t ui64Version = pSDKLimit->GetInt64Value();
					MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();
					if (ui64Version >= pStatus->GetSDKBuildNumber())
					{
						pStatus->SetVerfiyStatus(VERIFY_VERSION_LOW);
					}
				}

				JSON *pGlassesConfig = pJson->GetItemByName("GlassesConfig");
				JSON *pManufacturerConfig = pJson->GetItemByName("ManufacturerConfig");
				JSON *pProductConfig = pJson->GetItemByName("ProductConfig");
				JSON *pMachineList = pJson->GetItemByName("MachineList");
				if (pManufacturerConfig && pManufacturerConfig->Type == JSON_Array && pManufacturerConfig->GetArraySize() > 0 &&
					pProductConfig && pProductConfig->Type == JSON_Array && pProductConfig->GetArraySize() > 0 &&
					pGlassesConfig && pGlassesConfig->Type == JSON_Array && pGlassesConfig->GetArraySize() > 0)
				{
					// 先要更新一体机列表再解析
					if (pMachineList)
					{
						UpdateMachineList(pMachineList);
					}
					return UpdateManufacturer(pManufacturerConfig) &&
						UpdateProduct(pProductConfig) &&
						UpdateGlass(pGlassesConfig);
				}
			}
			return false;
		}
		bool GlassesConfigProfileV2::UpdateFromProfile(const char * lpszProfilePath, JSON * pJsonFromUpdate)
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			/*来自安装包的节点*/
			char filename[256];
			char* szError = NULL;
			strcpy(filename, lpszProfilePath);
			if (filename[strlen(filename) - 1] != '/')
				strcat(filename, "/");
			strcat(filename, "GlassesConfig.json");
			JSON *pJsonFromPacket = JSON::Load(filename, g_EncKey, (char const**)&szError);
			/*部分应用（如Daydream游戏）的cache中没有GlassesConfig文件，
			若直接退出将无法从sdcard或网络更新GlassesConfig数据，导致MachineList也无法更新，故去掉该判断*/
			/*
			if (pJsonFromPacket == NULL)
			{
				MOJING_ERROR(g_APIlogger, "Can not load profile from : " << filename);
				return false;
			}
			*/
			/*来自SD卡的节点*/
			JSON * pJsonFromSDCard = NULL;
			String LocalPath = MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath();
			if (LocalPath.Right(1) != "/")
				LocalPath += "/";
			LocalPath += GetClassName();
			LocalPath += ".dat";

			pJsonFromSDCard = JSON::Load(LocalPath, g_EncKey, (char const**)&szError);

			JSON *pUseNode = JSON_CompReleaseData(pJsonFromPacket, pJsonFromSDCard);
			pUseNode = JSON_CompReleaseData(pUseNode, pJsonFromUpdate);

			MOJING_TRACE(g_APIlogger, "Load profile from PACKET(GlassV2) = " << (pJsonFromPacket ? "YES" : "NO") << "; SDCARD = " << (pJsonFromSDCard ? "YES" : "NO") << "; Update = " << (pJsonFromUpdate ? "YES" : "NO"));

			if (pUseNode == pJsonFromPacket)
			{
				MOJING_TRACE(g_APIlogger, "Using PACKET Profile");
			}
			else if (pUseNode == pJsonFromSDCard)
			{
				MOJING_TRACE(g_APIlogger, "Using SDCARD Profile");
			}
			else if (pUseNode == pJsonFromUpdate)
			{
				MOJING_TRACE(g_APIlogger, "Using UPDATE Profile");
			}

			Release();
			bool bRet = FromJson(pUseNode);
			if (bRet && pUseNode == pJsonFromUpdate)
			{// 使用网络配置文件更新当前的配置信息
				pUseNode->Save(LocalPath, g_EncKey);
			}

			if (pJsonFromPacket)
			{
				pJsonFromPacket->Release();
				pJsonFromPacket = NULL;
			}
			if (pJsonFromSDCard)
			{
				pJsonFromSDCard->Release();
				pJsonFromSDCard = NULL;
			}

			return bRet;
		}
		JSON* GlassesConfigProfileV2::GetManufacturerJson(MojingProfileKey AppKey, unsigned short wLanguageCode)
		{
			JSON* pJsonRet = JSON::CreateObject();
			pJsonRet->AddStringItem("ClassName", "ManufacturerList");
			char szReleaseDate[16];
			sprintf(szReleaseDate, "%d", GetReleaseDate());
			pJsonRet->AddStringItem("ReleaseDate", szReleaseDate);

			JSON* pManufacturerList = JSON::CreateArray();
			pJsonRet->AddItem("ManufacturerList", pManufacturerList);

			map<unsigned int, ManufacturerInfo*>::iterator it = m_ManufacturerMap.begin();
			while (it != m_ManufacturerMap.end())
			{
				unsigned int MID = it->first;
				JSON* pManufacturer = it->second->ToJson(wLanguageCode);
				MojingProfileKey ManufacturerKey;
				ManufacturerKey.SetManufacturerID(MID);
				MakeFinalKey(ManufacturerKey);
				pManufacturer->AddStringItem("KEY", ManufacturerKey.GetString());

				pManufacturerList->AddArrayElement(pManufacturer);
				it++;
			}
			return pJsonRet;
		}

		JSON* GlassesConfigProfileV2::GetProductJson(MojingProfileKey ManufacturerKey, unsigned short wLanguageCode)
		{
			JSON* pJsonRet = JSON::CreateObject();
			pJsonRet->AddStringItem("ClassName", "ProductList");
			char szReleaseDate[16];
			sprintf(szReleaseDate, "%d", GetReleaseDate());
			pJsonRet->AddStringItem("ReleaseDate", szReleaseDate);

			JSON* pProductList = JSON::CreateArray();
			pJsonRet->AddItem("ProductList", pProductList);

			if (m_ManufacturerMap.find(ManufacturerKey.GetManufacturerID()) != m_ManufacturerMap.end())
			{
				ManufacturerInfo* pManufacturer = m_ManufacturerMap[ManufacturerKey.GetManufacturerID()];
				for (unsigned int i = 0; i < pManufacturer->m_AllProductID.size(); i++)
				{
					unsigned int PID = pManufacturer->m_AllProductID[i];
					if (m_ProductMap.find(PID) != m_ProductMap.end())
					{
						JSON *pProduct = m_ProductMap[PID]->ToJson(wLanguageCode);
						if (pProduct)
						{
							MojingProfileKey ProductKey;
							ProductKey.SetManufacturerID(ManufacturerKey.GetManufacturerID());
							ProductKey.SetProductID(PID);
							MakeFinalKey(ProductKey);
							pProduct->AddStringItem("KEY", ProductKey.GetString());
							pProductList->AddArrayElement(pProduct);
						}
					}
				}
			}
			return pJsonRet;
		}
		JSON* GlassesConfigProfileV2::CheckIsFinalKey(MojingProfileKey Key)
		{
			unsigned int AID = Key.GetAppID();
			unsigned int MID = Key.GetManufacturerID();
			unsigned int PID = Key.GetProductID();
			unsigned int GID = Key.GetGlassID();
			unsigned char PlatformID = Key.GetPlatformID();
			JSON *pRet = CheckKeyConnection(Key);
			if (pRet)
				return pRet;


			if (AID && MID && PID && GID && PlatformID)
				return NULL;

			pRet = JSON::CreateObject();
			if (AID == 0)
				pRet->AddStringItem("ERROR", "APP ID MISSING");
			if (MID == 0)
				pRet->AddStringItem("ERROR", "MANUFACTURER ID MISSING");
			if (PID == 0)
				pRet->AddStringItem("ERROR", "PRODUCT ID MISSING");
			if (GID == 0)
				pRet->AddStringItem("ERROR", "GLASS ID MISSING");
			if (PlatformID == 0)
				pRet->AddStringItem("ERROR", "PLATFORM ID MISSING");
			return pRet;
		}
		JSON* GlassesConfigProfileV2::CheckKeyConnection(MojingProfileKey Key)
		{
			String strErrorString;
			unsigned int AID = Key.GetAppID();
			unsigned int MID = Key.GetManufacturerID();
			unsigned int PID = Key.GetProductID();
			unsigned int GID = Key.GetGlassID();
			unsigned char PlatformID = Key.GetPlatformID();
			if (PlatformID != 0 && PlatformID != MojingPlatformBase::GetPlatformID())
			{
				strErrorString = "INVALID PLATFORM ID";
			}
			else
			{
				if (AID != 0 && AID != GetAppID())
				{
					strErrorString = "INVALID APP ID";
				}
				else
				{
					if (MID != 0)
					{
						if (m_ManufacturerMap.find(MID) != m_ManufacturerMap.end())
						{
							ManufacturerInfo *pManufacturer = m_ManufacturerMap[MID];
							if (PID != 0)
							{
								if (m_ProductMap.find(PID) != m_ProductMap.end() &&
									pManufacturer->m_AllProductID.end() != VECTOR_FIND(pManufacturer->m_AllProductID, PID))
								{
									ProductInfo *pProduct = m_ProductMap[PID];
									if (GID != 0)
									{
										if (m_GlassMap.find(GID) != m_GlassMap.end() &&
											pProduct->m_AllGlassID.end() != VECTOR_FIND(pProduct->m_AllGlassID, GID))
										{
											/*程序执行到这里，表示正确*/
										}
										else
										{
											strErrorString = "INVALID GLASS ID";
										}
									}
								}
								else
								{
									strErrorString = "INVALID PRODUCT ID";
								}
							}
						}
						else
						{
							strErrorString = "INVALID MANUFACTURER ID";
						}
					}
				}

			}

			if (strErrorString.GetLength())
			{
				JSON *pJson = JSON::CreateObject();
				pJson->AddStringItem("ERROR", strErrorString);
				return pJson;
			}
			return NULL;
		}
		JSON* GlassesConfigProfileV2::GetGlassJson(MojingProfileKey ProductKey, unsigned short wLanguageCode)
		{
			JSON* pJsonRet = JSON::CreateObject();
			pJsonRet->AddStringItem("ClassName", "GlassList");
			char szReleaseDate[16];
			sprintf(szReleaseDate, "%d", GetReleaseDate());
			pJsonRet->AddStringItem("ReleaseDate", szReleaseDate);

			JSON* pGlassList = JSON::CreateArray();
			pJsonRet->AddItem("GlassList", pGlassList);
			unsigned int MID = ProductKey.GetManufacturerID();
			unsigned int PID = ProductKey.GetProductID();
			if (m_ManufacturerMap.find(MID) != m_ManufacturerMap.end() &&
				m_ProductMap.find(PID) != m_ProductMap.end())
			{// MID 和 PID都是存在的 
				ManufacturerInfo *pManufacturer = m_ManufacturerMap[ProductKey.GetManufacturerID()];
				if (pManufacturer->m_AllProductID.end() != find(pManufacturer->m_AllProductID.begin(), pManufacturer->m_AllProductID.end(), ProductKey.GetProductID()))
				{// PID 和 MID的适配关系是 OK的 
					ProductInfo *pProductInfo = m_ProductMap[PID];
					for (unsigned int i = 0; i < pProductInfo->m_AllGlassID.size(); i++)
					{
						unsigned int GID = pProductInfo->m_AllGlassID[i];
						if (m_GlassMap.find(GID) != m_GlassMap.end())
						{
							JSON *pGlass = m_GlassMap[GID]->ToJson(wLanguageCode);
							if (pGlass)
							{
								MojingProfileKey GlassKey;
								GlassKey.SetManufacturerID(MID);
								GlassKey.SetProductID(PID);
								GlassKey.SetGlassID(GID);
								MakeFinalKey(GlassKey);
								pGlass->AddStringItem("KEY", GlassKey.GetString());
								pGlassList->AddArrayElement(pGlass);
							}
						}
					}
				}
			}
			return pJsonRet;
		}

		JSON* GlassesConfigProfileV2::GetGlassInfoJson(MojingProfileKey GlassKey, unsigned short wLanguageCode)
		{
			JSON *pRet = CheckIsFinalKey(GlassKey);
			if (!pRet)
			{
				unsigned int MID = GlassKey.GetManufacturerID();
				unsigned int PID = GlassKey.GetProductID();
				unsigned int GID = GlassKey.GetGlassID();

				ManufacturerInfo *pManufacturerInfo = m_ManufacturerMap[MID];
				ProductInfo *pProductInfo = m_ProductMap[PID];
				GlassInfo *pGlassInfo = m_GlassMap[GID];

				pRet = JSON::CreateObject();
				pRet->AddStringItem("ClassName", "GlassInfo");
				char szReleaseDate[16];
				sprintf(szReleaseDate, "%d", GetReleaseDate());
				pRet->AddStringItem("ReleaseDate", szReleaseDate);

				pRet->AddItem("Manufacturer", pManufacturerInfo->ToJson(wLanguageCode));
				pRet->AddItem("Product", pProductInfo->ToJson(wLanguageCode));
				pRet->AddItem("Glass", pGlassInfo->ToJson(wLanguageCode));
			}
			return pRet;
		}

		JSON* GlassesConfigProfileV2::GetMojingWorldJson(MojingProfileKey GlassKey, unsigned short wLanguageCode)
		{
			JSON *pRet = CheckIsFinalKey(GlassKey);
			if (!pRet)
			{
				unsigned int MID = GlassKey.GetManufacturerID();
				unsigned int PID = GlassKey.GetProductID();
				unsigned int GID = GlassKey.GetGlassID();

				ManufacturerInfo *pManufacturerInfo = m_ManufacturerMap[MID];
				ProductInfo *pProductInfo = m_ProductMap[PID];
				GlassInfo *pGlassInfo = m_GlassMap[GID];

				pRet = JSON::CreateObject();
				pRet->AddStringItem("ClassName", "GlassInfo");
				char szReleaseDate[16];
				sprintf(szReleaseDate, "%d", GetReleaseDate());
				pRet->AddStringItem("ReleaseDate", szReleaseDate);

				String strFinalKey = GlassKey.GetString();
				JSON *pInfo = pManufacturerInfo->ToJson(wLanguageCode);
				pInfo->AddStringItem("Key", strFinalKey.ToCStr());
				pRet->AddItem("Manufacturer", pInfo);
				pInfo = pProductInfo->ToJson(wLanguageCode);
				pInfo->AddStringItem("Key", strFinalKey.ToCStr());
				pRet->AddItem("Product", pInfo);
				pInfo = pGlassInfo->ToJson(wLanguageCode);
				pInfo->AddStringItem("Key", strFinalKey.ToCStr());
				pRet->AddItem("Glass", pInfo);
			}
			return pRet;
		}

		String GlassesConfigProfileV2::GetManufacturerJson(const char*lpszLanguageName)
		{
			String strRet;
			MojingProfileKey RootKey;
			unsigned short wLanguageCode = GetLanguageCode(lpszLanguageName);

			MakeFinalKey(RootKey);
			JSON *pRet = CheckKeyConnection(RootKey);
			if (!pRet)
				pRet = GetManufacturerJson(RootKey, wLanguageCode);
			if (pRet)
			{
				char *pErrorInfo = pRet->PrintValue(0, 0);
				strRet = pErrorInfo;
				MJ_FREE(pErrorInfo);
				pRet->Release();
			}
			return strRet;
		}
		String GlassesConfigProfileV2::GetProductJson(const char * lpszKey, const char*lpszLanguageName)
		{
			String strRet;
			MojingProfileKey RootKey;
			unsigned short wLanguageCode = GetLanguageCode(lpszLanguageName);
			if (!RootKey.SetString(lpszKey))
			{
				return "{\"ERROR\":\"INVALID KEY\"}";
			}
			MakeFinalKey(RootKey);
			JSON *pRet = CheckKeyConnection(RootKey);
			if (RootKey.GetManufacturerID() == 0)
			{
				return "{\"ERROR\":\"MANUFACTURER ID MISSING\"}";
			}
			if (!pRet)
				pRet = GetProductJson(RootKey, wLanguageCode);
			if (pRet)
			{
				char *pErrorInfo = pRet->PrintValue(0, 0);
				strRet = pErrorInfo;
				MJ_FREE(pErrorInfo);
				pRet->Release();
			}
			return strRet;
		}
		String GlassesConfigProfileV2::GetGlassJson(const char * lpszKey, const char*lpszLanguageName)
		{
			String strRet;
			MojingProfileKey RootKey;
			unsigned short wLanguageCode = GetLanguageCode(lpszLanguageName);
			if (!RootKey.SetString(lpszKey))
			{
				return "{\"ERROR\":\"INVALID KEY\"}";
			}
			MakeFinalKey(RootKey);
			JSON *pRet = CheckKeyConnection(RootKey);
			if (RootKey.GetManufacturerID() == 0)
			{
				return "{\"ERROR\":\"MANUFACTURER ID MISSING\"}";
			}
			if (RootKey.GetProductID() == 0)
			{
				return "{\"ERROR\":\"PRODUCT ID MISSING\"}";
			}
			if (!pRet)
				pRet = GetGlassJson(RootKey, wLanguageCode);
			if (pRet)
			{
				char *pErrorInfo = pRet->PrintValue(0, 0);
				strRet = pErrorInfo;
				MJ_FREE(pErrorInfo);
				pRet->Release();
			}
			return strRet;
		}
		String GlassesConfigProfileV2::GetGlassInfoJson(const char * lpszKey, const char*lpszLanguageName)
		{
			String strRet;
			MojingProfileKey RootKey;
			unsigned short wLanguageCode = GetLanguageCode(lpszLanguageName);
			if (!RootKey.SetString(lpszKey))
			{
				return "{\"ERROR\":\"INVALID KEY\"}";
			}
			MakeFinalKey(RootKey);
			JSON *pRet = CheckKeyConnection(RootKey);
			if (RootKey.GetManufacturerID() == 0)
			{
				return "{\"ERROR\":\"MANUFACTURER ID MISSING\"}";
			}
			if (RootKey.GetProductID() == 0)
			{
				return "{\"ERROR\":\"PRODUCT ID MISSING\"}";
			}
			if (RootKey.GetGlassID() == 0)
			{
				return "{\"ERROR\":\"GLASS ID MISSING\"}";
			}

			if (!pRet)
				pRet = GetGlassInfoJson(RootKey, wLanguageCode);
			if (pRet)
			{
				char *pErrorInfo = pRet->PrintValue(0, 0);
				strRet = pErrorInfo;
				MJ_FREE(pErrorInfo);
				pRet->Release();
			}
			return strRet;
		}

		String GlassesConfigProfileV2::GetMojingWorldJson(const char * lpszKey, const char*lpszLanguageName)
		{
			String strRet;
			MojingProfileKey RootKey;
			unsigned short wLanguageCode = GetLanguageCode(lpszLanguageName);
			if (!RootKey.SetString(lpszKey))
			{
				return "{\"ERROR\":\"INVALID KEY\"}";
			}
			MakeFinalKey(RootKey);
			JSON *pRet = CheckKeyConnection(RootKey);
			if (RootKey.GetManufacturerID() == 0)
			{
				return "{\"ERROR\":\"MANUFACTURER ID MISSING\"}";
			}
			if (RootKey.GetProductID() == 0)
			{
				return "{\"ERROR\":\"PRODUCT ID MISSING\"}";
			}
			if (RootKey.GetGlassID() == 0)
			{
				return "{\"ERROR\":\"GLASS ID MISSING\"}";
			}

			if (!pRet)
				pRet = GetMojingWorldJson(RootKey, wLanguageCode);
			if (pRet)
			{
				char *pErrorInfo = pRet->PrintValue(0, 0);
				strRet = pErrorInfo;
				MJ_FREE(pErrorInfo);
				pRet->Release();
			}
			return strRet;
		}

		/*String GlassesConfigProfileV2::GetElementJsonString(const char * lpszKey, const char*lpszLanguageName)
		{
		String strRet = "";
		MojingProfileKey RootKey;
		JSON *pRet = NULL;
		unsigned short wLanguageCode = GetLanguageCode(lpszLanguageName);
		if (RootKey.SetString(lpszKey) && wLanguageCode != 0)
		{
		MakeFinalKey(RootKey);
		pRet = CheckKeyConnection(RootKey);
		if (pRet == NULL)
		{
		if (RootKey.GetManufacturerID() == 0)
		{// 没有指定厂商，返回所有的厂商列表
		pRet = GetManufacturerJson(RootKey, wLanguageCode);
		}
		else if (RootKey.GetProductID() == 0)
		{// 没有指定产品，返回厂商的产品列表
		pRet = GetProductJson(RootKey, wLanguageCode);
		}
		else if (RootKey.GetGlassID() == 0)
		{// 没有指定眼镜，返回眼镜列表
		pRet = GetGlassJson(RootKey, wLanguageCode);
		}
		else
		{// 返回完整的信息
		pRet = GetGlassInfoJson(RootKey, wLanguageCode);
		}
		}

		if (pRet)
		{
		char *pErrorInfo = pRet->PrintValue(0, 0);
		strRet = pErrorInfo;
		MJ_FREE(pErrorInfo);
		delete pRet;
		}
		}
		else
		{
		return "{\"ERROR\":\"INVALID KEY\"}";
		}

		return strRet;
		}*/
		ManufacturerInfo* GlassesConfigProfileV2::GetManufacturer(unsigned int ID)
		{
			if (m_ManufacturerMap.size() && m_ManufacturerMap.find(ID) != m_ManufacturerMap.end())
			{
				return m_ManufacturerMap[ID];
			}
			return NULL;
		}
		ProductInfo* GlassesConfigProfileV2::GetProduct(unsigned int ID)
		{
			if (m_ProductMap.size() && m_ProductMap.find(ID) != m_ProductMap.end())
			{
				return m_ProductMap[ID];
			}
			return NULL;
		}

		GlassInfo* GlassesConfigProfileV2::GetGlass(unsigned int ID)
		{
			if (m_GlassMap.size() && m_GlassMap.find(ID) != m_GlassMap.end())
			{
				return m_GlassMap[ID];
			}
			return NULL;
		}

		bool GlassesConfigProfileV2::IsGlassesNeedDistortionByName(const char * lpszKey)
		{
			bool bRet = false;
			MojingProfileKey UsingKey;
			if (UsingKey.SetString(lpszKey))
			{
				if (UsingKey.GetAppID() == 0)
				{
					MOJING_TRACE(g_APIlogger, "Set APP ID " << GetAppID());
					UsingKey.SetAppID(GetAppID());
				}
				else
				{
					//MOJING_TRACE(g_APIlogger, "Use APP ID" << UsingKey.GetAppID());
				}
				if (UsingKey.GetPlatformID() == 0)
				{
					UsingKey.SetPlatformID(MojingPlatformBase::GetPlatformID());
				}


				bool bCheckManufacturerID = (UsingKey.GetManufacturerID() != 0 && m_ManufacturerMap.find(UsingKey.GetManufacturerID()) != m_ManufacturerMap.end());
				bool bProductID = (UsingKey.GetProductID() != 0 && m_ProductMap.find(UsingKey.GetProductID()) != m_ProductMap.end());
				bool bGlassID = (UsingKey.GetGlassID() != 0 && m_GlassMap.find(UsingKey.GetGlassID()) != m_GlassMap.end());
				bool bAppID = (UsingKey.GetAppID() == GetAppID());
				bool bPlatformID = (UsingKey.GetPlatformID() == MojingPlatformBase::GetPlatformID());
				//				MOJING_TRACE(g_APIlogger, "APP_ID " << UsingKey.GetAppID() << ":" << GetAppID());
				MOJING_TRACE(g_APIlogger, "CheckIDs " << bCheckManufacturerID << bProductID << bGlassID << bAppID << bPlatformID);
				// 适配检查
				if (bCheckManufacturerID && bProductID && bGlassID && bAppID && bPlatformID)
				{// 所有ID都存在,且APP ID 和 PLATFORM ID匹配
					ManufacturerInfo *pManufacturer = m_ManufacturerMap[UsingKey.GetManufacturerID()];
					ProductInfo *pProduct = m_ProductMap[UsingKey.GetProductID()];
					GlassInfo *pGlass = m_GlassMap[UsingKey.GetGlassID()];
					if (pManufacturer->m_AllProductID.end() != find(pManufacturer->m_AllProductID.begin(), pManufacturer->m_AllProductID.end(), UsingKey.GetProductID()) &&
						pProduct->m_AllGlassID.end() != find(pProduct->m_AllGlassID.begin(), pProduct->m_AllGlassID.end(), UsingKey.GetGlassID()))
					{
						return pGlass->GetSegments() != 0;
					}
				}
			}
			return false;
		}
		bool GlassesConfigProfileV2::UsingMojingWorldKey(const char* lpszKey)
		{
			bool bRet = false;
			MojingProfileKey UsingKey;
			if (UsingKey.SetString(lpszKey))
			{
				if (UsingKey.GetAppID() == 0)
				{
					MOJING_TRACE(g_APIlogger, "Set APP ID " << GetAppID());
					UsingKey.SetAppID(GetAppID());
				}
				else
				{
					//MOJING_TRACE(g_APIlogger, "Use APP ID" << UsingKey.GetAppID());
				}
				if (UsingKey.GetPlatformID() == 0)
				{
					UsingKey.SetPlatformID(MojingPlatformBase::GetPlatformID());
				}


				bool bCheckManufacturerID = (UsingKey.GetManufacturerID() != 0 && m_ManufacturerMap.find(UsingKey.GetManufacturerID()) != m_ManufacturerMap.end());
				bool bProductID = (UsingKey.GetProductID() != 0 && m_ProductMap.find(UsingKey.GetProductID()) != m_ProductMap.end());
				bool bGlassID = (UsingKey.GetGlassID() != 0 && m_GlassMap.find(UsingKey.GetGlassID()) != m_GlassMap.end());
				bool bAppID = (UsingKey.GetAppID() == GetAppID());
#ifndef MJ_OS_WIN32
				bool bPlatformID = (UsingKey.GetPlatformID() == MojingPlatformBase::GetPlatformID());
#else
				bool bPlatformID = true;
#endif
				//				MOJING_TRACE(g_APIlogger, "APP_ID " << UsingKey.GetAppID() << ":" << GetAppID());
				MOJING_TRACE(g_APIlogger, "CheckIDs " << bCheckManufacturerID << bProductID << bGlassID << bAppID << bPlatformID);
				if (!bProductID)
				{
					MOJING_TRACE(g_APIlogger, "PID = " << UsingKey.GetProductID() << " , PMap.Count = " << m_ProductMap.size());
				}

				// 适配检查
				if (bCheckManufacturerID && bProductID && bGlassID && bAppID && bPlatformID)
				{// 所有ID都存在,且APP ID 和 PLATFORM ID匹配

					MOJING_TRACE(g_APIlogger, "MID = " << UsingKey.GetManufacturerID() << " , PID = " << UsingKey.GetProductID() << " , GID = " << UsingKey.GetGlassID());


					ManufacturerInfo *pManufacturer = m_ManufacturerMap[UsingKey.GetManufacturerID()];
					ProductInfo *pProduct = m_ProductMap[UsingKey.GetProductID()];
					GlassInfo *pGlass = m_GlassMap[UsingKey.GetGlassID()];

					if (pManufacturer->m_AllProductID.end() != find(pManufacturer->m_AllProductID.begin(), pManufacturer->m_AllProductID.end(), UsingKey.GetProductID()) &&
						pProduct->m_AllGlassID.end() != find(pProduct->m_AllGlassID.begin(), pProduct->m_AllGlassID.end(), UsingKey.GetGlassID()))
					{// 检查厂商--产品，产品--镜片对应关系
						Distortion* pDistortion = Manager::GetMojingManager()->GetDistortion();
						pDistortion->SetFOV(pGlass->GetFOV());
						float fYOffset = 0.0f;

						/*1 得到手机边框和眼镜的装配镜片中心位置，以米为单位*/
						Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
						MojingDisplayParameters * pDisplay = pParameters->GetDisplayParameters();

						// 注意 ： 用户有可能会更新过配置，所以这里要再次获取一下用户设置的PPI
						pDisplay->UpdatePPIFromUserSetting();

						/*1.1 通过对角线 尺寸计算适用的高度*/
						float fLongerEdgeInMeter = fmax(pDisplay->GetScreenWidthMeter(), pDisplay->GetScreenHeightMeter());
						float fShorterEdgeInMeter = fmin(pDisplay->GetScreenWidthMeter(), pDisplay->GetScreenHeightMeter());

						double dScreenSize = sqrtf(fLongerEdgeInMeter*fLongerEdgeInMeter + fShorterEdgeInMeter*fShorterEdgeInMeter) / 2.54 * 100;

						const MobilePositionInfo ProductCenter = pProduct->GetMobilePositoin(dScreenSize);

#ifdef MJ_OS_ANDROID
						float fMobileEdge = pDisplay->GetMobileEdge();
						float fScreenHafe = 0;

						float fProductCenter = ProductCenter.GetMobilePosition();
						if (ProductCenter.GetMobilePosition() > 0 && fMobileEdge > 0.0001)
						{

							/*2 测算手机的屏幕高度和手机中心的位置，以米为单位*/
							fScreenHafe = fmin(fLongerEdgeInMeter, fShorterEdgeInMeter) / 2;
							/*3 计算中心高度调整量 ,以 米为单位。调高为正数，调低为负数*/
							if (ProductCenter.GetUpToDown())
								fYOffset = (fScreenHafe + fMobileEdge) - fProductCenter;// 处理自上而下的对齐
							else
								fYOffset = fProductCenter - (fScreenHafe + fMobileEdge);
						}
						float fUserPPI = pDisplay->GetUserPPI();
						if (fUserPPI > 1)
							MOJING_TRACE(g_APIlogger, "Using UserPPI = " << pDisplay->GetUserPPI() << " , XDPI = " << pDisplay->GetXdpi() << " , YDPI = " << pDisplay->GetYdpi());
						else
							MOJING_TRACE(g_APIlogger, "Using PPI = " << pDisplay->GetPPI() << " , XDPI = " << pDisplay->GetXdpi() << " , YDPI = " << pDisplay->GetYdpi());
						MOJING_TRACE(g_APIlogger, "Using Screen = " << pDisplay->GetScreenWidth() << " x " << pDisplay->GetScreenHeight() << " , " << pDisplay->GetScreenWidthMeter() << " x " << pDisplay->GetScreenHeightMeter());
						/*4 调整中心高度位置,以 米为单位。调高为正数，调低为负数*/

						MOJING_TRACE(g_APIlogger, "ProductCenter = " << fProductCenter << " , MobileSize = " << dScreenSize << "Inch , ScreenHafe = " << fScreenHafe << "M , MobileEdge" << fMobileEdge << "M , YOffset = " << fYOffset << "M");
#endif
						pDistortion->SetYOffset(fYOffset);
						int iSegments = pGlass->GetSegments();
						if (iSegments > 0)
						{
							pDistortion->SetMetersPerTanAngleAtCenter(pGlass->GetMetersPerTanAngleAtCenter());
							pDistortion->SetLensSeparation(pProduct->GetLensSeparation());
							if (!pGlass->GetReCalculationKT())
							{
								pDistortion->SetDistortionParamet(iSegments, pGlass->GetNoDispersion() != 0,
									pGlass->m_fR, pGlass->m_fG, pGlass->m_fB, pGlass->m_fL,
									pGlass->m_fRT, pGlass->m_fGT, pGlass->m_fBT);
							}
							else
							{
								pDistortion->SetDistortionParamet(iSegments, pGlass->GetNoDispersion() != 0,
									pGlass->m_fR, pGlass->m_fG, pGlass->m_fB, pGlass->m_fL,
									NULL, NULL, NULL);
							}
						}
						else
						{
							float fHight = fShorterEdgeInMeter / 2;
							float fWidth = fLongerEdgeInMeter / 4;
							float fHafeEyeSpace = pProduct->GetLensSeparation() / 2;
							// iPhone 上不能直接用Hight/2作为尺寸
							float fMetersPerTanAngleAtCenter = fHight > fWidth ? fHight : fWidth;
							fMetersPerTanAngleAtCenter = fMetersPerTanAngleAtCenter > fHafeEyeSpace ? fMetersPerTanAngleAtCenter : fHafeEyeSpace;
							fMetersPerTanAngleAtCenter *= 1.001;// 适当放大一点，用于消除浮点数精度误差
							pDistortion->SetMetersPerTanAngleAtCenter(fMetersPerTanAngleAtCenter);
							pDistortion->SetLensSeparation(pProduct->GetLensSeparation());
							// 必然没有色散
							pDistortion->SetDistortionParamet(0, true, NULL, NULL, NULL, NULL);

						}
						SetCurrentKey(UsingKey);
						pDistortion->SetGlassKey(lpszKey);
						String sDULR = pGlass->GetDURL_Original();
						if (sDULR.GetLength() < 1)
							sDULR = pGlass->GetDURL();
						pDistortion->SetDURL(sDULR);
#if 0
						float *pFVectex = NULL, * pFUV = NULL;
						int *pIndex = NULL;
						int iCount = pDistortion->UNITY_BuildDistortionMesh(40, 40, NULL, NULL, NULL);
						pFVectex = new float[iCount * 3]; // 每个定点3个浮点数
						pFUV = new float[iCount *  2]; // 每个顶点对应的UV占2个浮点数
						pIndex = new int[iCount * 2 * 6]; // 索引数
						pDistortion->UNITY_BuildDistortionMesh(40, 40, pFVectex, pFUV, pIndex);

						delete pFUV;
						delete pFVectex;
						delete pIndex;
#endif
#if 0
						Unreal_DistortionVertexBuffer *pDistortionVertexBuffer = pDistortion->BuildUnrealDistortionVertexBuffer(32, 32);

						String strTempFile = MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath();
						if (strTempFile.Right(1) != "/")
							strTempFile += "/";
						strTempFile += UsingKey.GetString() + ".txt";
						FILE *pDistortionFile = fopen(strTempFile.ToCStr() , "wt");
						if (pDistortionFile)
						{
							fprintf(pDistortionFile ,"//Glass: %s \n" , UsingKey.GetGlassKeyIDString());

							if (pDistortionVertexBuffer)
							{
								String DistortionScript = pDistortionVertexBuffer->GetString();
								fputs(DistortionScript.ToCStr(), pDistortionFile);
								fflush(pDistortionFile);
							}
							fclose(pDistortionFile);
						}

						delete pDistortionVertexBuffer;
#endif
						MojingRenderBase::SetModify();
						bRet = true;
					}
					else
					{
						MOJING_ERROR(g_APIlogger, "OUT 2");
					}
				}
				else
				{
					MOJING_ERROR(g_APIlogger, "OUT 1");
				}
			}
			return bRet;
		}

		
		// 检查网络升级信息
		void GlassesConfigProfileV2::CheckUpdate()
		{
			double dLastCheckGlassConfigTime = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->GetCheckGlassConfig();
			//request once per day.
			if (fabs(ReporterTools::GetCurrentTime() - dLastCheckGlassConfigTime) < CHECK_GLASSCONFIG_INTERVEL)
			{
				return;
			}

			char szTime[32];
			char szReleaseDate[32];
			String data;
				
			data += "releasedate=";
			sprintf(szReleaseDate, "%u", m_uiReleaseDate);
			data += szReleaseDate;
			data += "&curr_time=";
			
			MOJING_TRACE(g_APIlogger, "Check update Version = " << m_uiReleaseDate);
			
			struct timeval curtimeval;
			struct timezone curtimezone;

			gettimeofday(&curtimeval, &curtimezone);
			sprintf(szTime, "%u", (int)curtimeval.tv_sec);
			data += szTime;
			/////////////////////////////////////////////////////
			MD5 md5;
			md5.reset();
			
			size_t  buflen = strlen(szTime) + strlen(szReleaseDate) + 128;
			char * pMD5SrcBuffer = new char[buflen];
			strcpy(pMD5SrcBuffer, szReleaseDate);
			strcat(pMD5SrcBuffer, szTime);
			strcat(pMD5SrcBuffer, "26c216f0a56c6c385b4c3d8e28f452f8");
			data += "&sign=";

			md5.update(pMD5SrcBuffer, strlen(pMD5SrcBuffer));
			data += md5.toString();
			delete[] pMD5SrcBuffer;
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "URL_DATA = " << data);
#endif
			ProfileThreadMGR::UpdateInternetProfile(GetClassName(), data, Profile_LOAD, CheckUpdateCallBack, this);
		}
		// 检查网络升级反馈回调
		void GlassesConfigProfileV2::CheckUpdateCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			if (RespCode != 200)
			{
				MOJING_TRACE(g_APIlogger, "Update FAILD! Code = " << RespCode);
				return;
			}
			Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->SetCheckGlassConfig(ReporterTools::GetCurrentTime());
			Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->Save();

			// 这是没有加密的JSON反馈，如果成功的返回了URL那么下载
			GlassesConfigProfileV2 * pThis = (GlassesConfigProfileV2 *)pCallBackParam;
			char *pBuffer = new char[uiSize + 1];
			memcpy(pBuffer , lpszRespString , uiSize);
			pBuffer[uiSize] = 0;
			JSON *pJson = JSON::Parse(pBuffer);
			delete[] pBuffer;

			if (pJson != NULL)
			{
				if (JSON *pResultNode = pJson->GetItemByName("Result"))
				{
					if (pResultNode->GetStringValue().CompareNoCase("SUCCEED") == 0)
					{
						JSON *pURLNode = pJson->GetItemByName("GlassesURL");
						if (pURLNode)
						{
							String szURL = pURLNode->GetStringValue();
#if defined(_DEBUG) && defined(MJ_OS_ANDROID)
							char *pUrlBuffer = new char[szURL.GetLength() + 128];
							char *pUrlBuffer2 = new char[szURL.GetLength() + 128];
							memset(pUrlBuffer, 0, szURL.GetLength() + 128);
							memset(pUrlBuffer2, 0, szURL.GetLength() + 128);
							const char *pReplace = "fuwu.mojing.cn";
							int iReplaceLength = strlen(pReplace);
							const char *pReplaceTo = "192.168.12.62:8084";
							strcpy(pUrlBuffer , szURL.ToLower().ToCStr());
							
							while (char *pPos = strstr(pUrlBuffer, pReplace))
							{
								strncpy(pUrlBuffer2, pUrlBuffer, pPos - pUrlBuffer);
								strcat(pUrlBuffer2, pReplaceTo);
								strcat(pUrlBuffer2, pPos + iReplaceLength);
								strcpy(pUrlBuffer, pUrlBuffer2);
							}
							ProfileThreadMGR::UpdateInternetProfile(pThis->GetClassName(), pUrlBuffer, Profile_LOAD, UpdateDownloadCallBack, pThis);
							delete[] pUrlBuffer;
							delete[] pUrlBuffer2;
#else
							ProfileThreadMGR::UpdateInternetProfile(pThis->GetClassName(), szURL , Profile_LOAD, UpdateDownloadCallBack, pThis);
#endif
						}
					}
				}
				pJson->Release();
			}
		}

		void GlassesConfigProfileV2::UpdateDownloadCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			if (RespCode != 200)
			{
				MOJING_TRACE(g_APIlogger, "GlassesConfigProfileV2 Update FAILD! Code = " << RespCode);
				return;
			}
            if(lpszRespString == NULL || uiSize == 0)
            {
                MOJING_TRACE(g_APIlogger, "GlassesConfigProfileV2 Update NULL");
                return;
            }

			// 程序执行到这里表示成功下载了加密的配置文件
			unsigned char* pEncBuffer = (unsigned char*)lpszRespString;
			JSON *pNewJson = JSON::ParseEnc(lpszRespString, uiSize, g_EncKey);
			if (pNewJson)
			{
				// 更新配置文件，并且根据需要存储之
				GlassesConfigProfileV2 *pProfile = (GlassesConfigProfileV2 *)pCallBackParam;
				MojingPlatformBase *pPlatform = MojingPlatformBase::GetPlatform();
				if (pPlatform)
					pProfile->UpdateFromProfile(pPlatform->GetPacketProfilePath(), pNewJson);
				pNewJson->Release();
			}
		}

	}
}
