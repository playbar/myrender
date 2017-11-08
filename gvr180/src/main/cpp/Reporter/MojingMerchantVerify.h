#pragma once
#include "../Base/MojingTypes.h"
#include "../Base/MojingString.h"
#include "ReporterTools.h"
#include "../3rdPart/AES/AESPro.h"
namespace Baofeng
{
	namespace Mojing
	{
#define MOJING_KEY "com.baofeng.mj.CPAPP."
#define MOJING_CHAIN ".MOJING"
#define MOJING_PACKAGE_NAME_JSON_NODE "package_name"
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
			void AppCheckPackage(const char* szAppName, const char* szCaseCode);
		private:
			bool IsNeedVerifyFromServer();
			void SaveVerifyResult(bool bVerifyOK);
			void VerifyFromServer();
			String GetPackageNameFromCaseCode(const char* szAppName, const char* szCaseCode);
			bool AESKeyInit(const char* szAppName);
			String AESDecrypt(const char* szCaseCode);
		private:
			CAESPro m_cAesProHandle;
		};
	}
}
