#pragma once
#include "MojingProfileKey.h"
#include "../Base/MojingThreads.h"
#include "ProfileV2/GlassInfo.h"
#include "ProfileV2/ProductInfo.h"
#include "ProfileV2/ManufacturerInfo.h"

namespace Baofeng
{
	namespace Mojing
	{
		class GlassesConfigProfileV2 :
			public MojingJsonElement
		{


			bool UpdateManufacturer(JSON *pManufacturerArray);
			bool UpdateProduct(JSON *pProductArray);
			bool UpdateMachineList(JSON *pMachineListArray);
			bool UpdateGlass(JSON *pGlassArray);
			void ReleaseManufacturer();
			void ReleaseProduct();
			void ReleaseGlass();
			void Release();
			
			JSON* GetManufacturerJson(MojingProfileKey AppKey , unsigned short wLanguageCode);
			JSON* GetProductJson(MojingProfileKey ManufacturerKey, unsigned short wLanguageCode);
			JSON* GetGlassJson(MojingProfileKey ProductKey, unsigned short wLanguageCode);
			JSON* GetGlassInfoJson(MojingProfileKey GlassKey, unsigned short wLanguageCode);
			JSON* GetMojingWorldJson(MojingProfileKey GlassKey, unsigned short wLanguageCode);
			unsigned int GenerationAppID(const char* szAppName);
			
			// 检查网络升级反馈回调
			static void CheckUpdateCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam);
			// 网络更新完成反馈回调
			static void UpdateDownloadCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam);
		public:
			GlassesConfigProfileV2();
			virtual ~GlassesConfigProfileV2();
			virtual bool FromJson(JSON*);
			virtual JSON* ToJson();
			bool UsingMojingWorldKey(const char* lpszKey);
			bool IsGlassesNeedDistortionByName(const char * szGlassesName);
			bool UpdateFromProfile(const char * lpszProfilePath , JSON * pInternetJson = NULL);
			////////////////////////////////////////////////////////////////////////////////////
			String GetManufacturerJson(const char*lpszLanguageName);
			String GetProductJson(const char * lpszKey, const char*lpszLanguageName);
			String GetGlassJson(const char * lpszKey, const char*lpszLanguageName);
			String GetGlassInfoJson(const char * lpszKey, const char*lpszLanguageName);
			String GetMojingWorldJson(const char * lpszKey, const char*lpszLanguageName);
			
			///////////////////////////////////////////////////////////////////////////////////
			// 如果APPID和PlatformID缺失的话，补充上这两个信息
			void MakeFinalKey(MojingProfileKey& Key);
			// 检查KEY的厂商-产品-眼睛-平台-APP是否与配置文件和运行环境匹配
			JSON* CheckKeyConnection(MojingProfileKey Key);
			// 除了检查匹配，还检查信息是否完整
			JSON* CheckIsFinalKey(MojingProfileKey Key);

			// 通过ID获取信息节点
			ManufacturerInfo* GetManufacturer(unsigned int ID);
			ProductInfo* GetProduct(unsigned int ID);
			GlassInfo* GetGlass(unsigned int ID);

			// 检查网络升级信息
			void CheckUpdate();
			
			CLASS_INTERFACE_J(unsigned int ,m_ui , ReleaseDate);
			CLASS_INTERFACE_J(unsigned int, m_ui, AppID);
			CLASS_INTERFACE(MojingProfileKey , m_ , CurrentKey);
			// Mutex m_Lock;
			map<unsigned int, ManufacturerInfo* > m_ManufacturerMap;
			map<unsigned int, ProductInfo*> m_ProductMap;
			map<unsigned int, GlassInfo*> m_GlassMap;

			//void UpdateDURL();
			
		};

	}
}