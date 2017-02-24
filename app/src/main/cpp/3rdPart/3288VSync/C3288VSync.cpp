#include "C3288VSync.h"
#include <dlfcn.h>


#ifdef ENABLE_LOGGER
#ifdef LOG4CPLUS_IMPORT
#include "../log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
extern MojingLogger g_APIlogger;
#endif // ENABLE_LOGGER

namespace Baofeng
{
	namespace Mojing
	{
		/*
		1) 初始化函数，hwcinit（）； 0：成功，-1 :失败
		2) hwsync（）； 为vysnc 阻塞函数，每调用一次为vysnc 耗时，函数返回值为当前 vysnc 时间戳。
		3) hwcdeint( );  反初始化

		*/
		C3288VSync::C3288VSync():
			m_hDLL(0),
			m_fpInit(0),
			m_fpDeInit(0),
			m_fpWaitVSync(0),
			m_bInitd(false)
		{
		}


		C3288VSync::~C3288VSync()
		{
			CloseVSync();
		}
		bool C3288VSync::OpenVSync()
		{
			if (m_bInitd)
				return true;

			m_hDLL = dlopen("/system/lib/libvsync.so", RTLD_NOW | RTLD_LOCAL);
			if (m_hDLL)
			{
				m_fpInit = (FP_hwvsync_init)dlsym(m_hDLL, "_Z12hwvsync_initv");
				m_fpWaitVSync = (FP_hwvsync)dlsym(m_hDLL, "_Z8hw_vsyncv");
				m_fpDeInit = (FP_hwvsync_deinit)dlsym(m_hDLL, "_Z12vsync_Deinitv");
				if (m_fpInit != NULL && m_fpWaitVSync != NULL && m_fpDeInit != NULL)
				{
					if (m_fpInit() != 0)
						CloseVSync();// 初始化失败
					else
						m_bInitd = true;
				}
				else
				{// 有函数指针拿不到
					CloseVSync();
				}
			}
			else// m_hDLL == NULL
			{
				const char* err = dlerror();
				MOJING_ERROR(g_APIlogger, "Can not load libary \" / system / lib / libvsync.so\"  Error = " << (err ? err : "Unknown"));
			}

			return m_bInitd;
		}
		bool C3288VSync::CloseVSync()
		{
			if (m_bInitd && m_fpDeInit)
				m_fpDeInit();
			
			if (m_hDLL)
				dlclose(m_hDLL);

			m_hDLL = NULL;
			m_fpInit = NULL;
			m_fpDeInit = NULL;
			m_fpWaitVSync = NULL;
			m_bInitd = (false);
			return true;
		}

		double C3288VSync::WaitNextVSync()
		{
			double dRet = -1;
			if (m_bInitd)
			{
				dRet = m_fpWaitVSync();
				dRet *= 1e-9;
			}
			return dRet;
		}
	}
}