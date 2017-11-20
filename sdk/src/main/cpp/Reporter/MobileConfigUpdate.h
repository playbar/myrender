#pragma once
#include "ReporterTools.h"
namespace Baofeng
{
	namespace Mojing
	{
		class MobileConfigUpdate :public ReporterTools
		{
			static void InternetProfileCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam);
			MobileConfigUpdate();
		public:
			
			virtual ~MobileConfigUpdate();
			void	UpdateConfig();
			static MobileConfigUpdate* GetMobileConfigUpdate();
			CLASS_INTERFACE_STR(String , m_str , ClassName);
		};


	}
}
