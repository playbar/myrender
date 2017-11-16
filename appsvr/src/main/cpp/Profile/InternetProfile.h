#pragma once
#include "../Base/MojingThreads.h"
#include "../Base/MojingString.h"

namespace Baofeng
{
	namespace Mojing
	{
		enum InternetProfileFunction{
			IPF_POST,		//Post mode
			IPF_GET_QUERY,  //Get mode, query
			IPF_GET_UP		//Get mode, insert or update
		};
		typedef void(*FP_InternetProfileCallBack)(const unsigned char* lpszRespString,unsigned int uiSize , int RespCode , void * pCallBackParam);
		class InternetProfile :public Thread
		{
			static size_t WriteCallbackString(void *ptr, size_t size, size_t numemb, void *strJason);

		public:
			InternetProfile();
			virtual ~InternetProfile();
			virtual int Run();
			CLASS_MEMBER_STR(String, m_str, URL);
			CLASS_MEMBER_STR(String, m_str, ParametData);
			CLASS_MEMBER_STR(String, m_str, TransportFiles);
			CLASS_MEMBER(InternetProfileFunction , m_ , Function);
			CLASS_MEMBER(FP_InternetProfileCallBack, m_fp, CallBack);
			CLASS_MEMBER(void*, m_p, CallBackParam);
        private:
#ifdef MJ_OS_MAC
            int HttpRequest();
#else
            int CurlPerform();
#endif
		};

	}
}

