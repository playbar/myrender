#pragma once
#include "GlassInfo.h"
namespace Baofeng
{
	namespace Mojing
	{
		class MobilePositionInfo :public MojingJsonElement
		{
		public:
			MobilePositionInfo(){ m_dMobileSize = m_dMobilePosition = 0; m_bUpToDown = false; };
			virtual ~MobilePositionInfo(){};
			virtual JSON* ToJson(){ return NULL; };
			virtual bool FromJson(JSON*pJsonNode){
				bool bRet = MobileSizeFromJson(pJsonNode) && MobilePositionFromJson(pJsonNode);
				if (bRet && pJsonNode->GetItemByName("UpToDown") != NULL)
				{
					UpToDownFromJson(pJsonNode);
				}
                return bRet;
			};
			CLASS_INTERFACE_J(double, m_d, MobileSize);
			CLASS_INTERFACE_J(double, m_d, MobilePosition);
			CLASS_INTERFACE_J(double, m_b, UpToDown);
		};
		
		class ProductInfo :public MojingJsonElement
		{
		public:
			ProductInfo();
			virtual ~ProductInfo();
			virtual JSON* ToJson();
			virtual bool FromJson(JSON*);
			JSON* ToJson(unsigned short wLanguageCode);
			
			bool CheckEnable()const;
			
			const MobilePositionInfo GetMobilePositoin(const float fMobileSize)const;
			
			CLASS_INTERFACE_J(unsigned int, m_ui, ID);
			CLASS_INTERFACE_STR_J(String, m_sz, Name);
			CLASS_INTERFACE_STR_J(String, m_sz, URL);

			CLASS_INTERFACE_STR_J(String, m_sz, OS);// 适用的操作系统
// 			CLASS_INTERFACE_STR_J(String, m_sz, Brand);// 适用的厂商
// 			CLASS_INTERFACE_STR_J(String, m_sz, Model);// 适用的机型
			CLASS_INTERFACE_J(int , m_i , MachineID);

			CLASS_INTERFACE_J(double, m_d, LensSeparation);
			CLASS_INTERFACE_J(double, m_d, LensSeparationMin);
			CLASS_INTERFACE_J(double, m_d, LensSeparationMax);
			
			DisplayInfo m_Display;
			vector<unsigned int> m_AllGlassID;
			vector<MobilePositionInfo*> m_MobilePositon;
		};
	}
}

