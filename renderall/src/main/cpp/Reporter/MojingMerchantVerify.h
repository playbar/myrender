#pragma once
#include "../Base/MojingTypes.h"
#include "../Base/MojingString.h"
#include "ReporterTools.h"
namespace Baofeng
{
	namespace Mojing
	{
		class MojingMerchantVerify :public ReporterTools
		{
			MojingMerchantVerify();
			static void InternetProfileCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam);
		public:
			virtual ~MojingMerchantVerify();
			CLASS_MEMBER_STR(String, m_str, ClassName);
			CLASS_MEMBER_STR(String, m_str, MerchantID);
			CLASS_MEMBER_STR(String, m_str, AppID);
			CLASS_MEMBER_STR(String, m_str, AppKey);
			CLASS_MEMBER_STR(String, m_str, PackageName);
			static MojingMerchantVerify* GetMojingMerchantVerify();
			void AppVerify(const char* szMerchantID, const char* szAppID, const char* szAppKey, const char* szPackageName);

		private:
			bool IsNeedVerifyFromServer();
			void SaveVerifyResult(bool bVerifyOK);
			void VerifyFromServer();
		};
	}
}
