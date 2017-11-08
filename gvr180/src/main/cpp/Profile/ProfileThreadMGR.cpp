#include "ProfileThreadMGR.h"
#include "../Base/MojingTypes.h"
#include "../Base/MojingFile.h"
#include "FileProfile.h"
#include  <stdio.h>
#include  <stdlib.h>
#include <unistd.h>
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
		Mutex* ProfileThreadMGR::g_mtAllReporter = NULL;
		std::list<Thread*> ProfileThreadMGR::g_AllReporter;
		ProfileThreadMGR* ProfileThreadMGR::g_pReporterMGR = NULL;
								
		unsigned char g_EncKey[16] = { 'M', 'o', 'j', 'i', 'n', 'g', 'S', 'D',
								       'K', 'P', 'r', 'o', 'F', 'i', 'l', 'e' };

		ProfileThreadMGR::ProfileThreadMGR()
		{
			
		}


		ProfileThreadMGR::~ProfileThreadMGR()
		{
		}

		int ProfileThreadMGR::Run()
		{
			return 0;

			while (true)
			{
				g_mtAllReporter->DoLock();
				if (g_AllReporter.size() != 0)
				{
					std::list<Thread*>::iterator it = g_AllReporter.begin();
					g_mtAllReporter->Unlock();

					while (it != g_AllReporter.end())
					{
						if ((*it)->IsFinished())
						{
							g_mtAllReporter->DoLock();
							std::list<Thread*>::iterator itRemove = it;
							it++;
							g_AllReporter.erase(itRemove);
							delete *itRemove; 
							g_mtAllReporter->Unlock();
						}
					}// end while 
				}
				else
				{
					g_mtAllReporter->Unlock();
					Suspend(); // 挂起
				}// end else		
			}// end while
			return 1;
		}

		bool ProfileThreadMGR::CreateLoaclProfileThread(String szURL, String szClassName, MojingJsonElement *pElement, ProfileIO IO)
		{
			Thread* pLoaclProfile = new FileProfile(szURL,szClassName, pElement , IO == Profile_SAVE);
#if 0
			AppendThreadToRun(pLoaclProfile);
#else
			int iRet = pLoaclProfile->Run();
			// 注意：线程对象需要用Release删除
			pLoaclProfile->Release();
			return iRet;
#endif
		}

		void ProfileThreadMGR::AppendThreadToRun(Thread* pThread)
		{
			// 启动工作线程
			pThread->Start();
			// 工作线程增加到线程列表
			if (g_mtAllReporter == NULL)
				g_mtAllReporter = new Mutex;
			g_mtAllReporter->DoLock();
			ProfileThreadMGR::g_AllReporter.push_back(pThread);
			g_mtAllReporter->Unlock();

			// 启动守护线程
			if (g_pReporterMGR == NULL)
			{
				g_pReporterMGR = new ProfileThreadMGR;
				g_pReporterMGR->Start();
			}
		}
		String ProfileThreadMGR::GetLocalProfileName(String szClassName)
		{
			String szBaseDir = MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath();//  "sdcard/MojingSDK/";
			MakeDirs(szBaseDir);
			// 拼接目录分隔符
			if (szBaseDir.Right(1) != "/")
				szBaseDir += "/";

			if (szClassName == "")
			{
			}
			else
			{
				return szBaseDir + szClassName + ".dat";
			}
			return "";
		}
        
		String ProfileThreadMGR::GetInternetURL(String szClassName, ProfileIO IO)
		{
			if (szClassName == "")
			{// 空字符串
				return "";
			}
#ifdef MJ_OS_MAC
            String strRet = "https://";
#else
            String strRet = "http://";
#endif
			if (szClassName == "JoystickProfile")
			{
#ifdef _DEBUG
				strRet = "http://192.168.12.62:8084/api/sdk/getjoystickconfig";
                MOJING_TRACE(g_APIlogger, "JoystickProfile using inside address." << (IO == Profile_LOAD ? "Load..." : "Post..."));
#else
                strRet += "fuwu.mojing.cn/api/sdk/getjoystickconfig";
				MOJING_TRACE(g_APIlogger, "JoystickProfile using web address." << (IO == Profile_LOAD ? "Load..." : "Post..."));
#endif
                return strRet;
			}
			if (szClassName == "GlassesConfigProfileV2")
			{
#ifdef _DEBUG
				strRet = "http://192.168.12.62:8084/api/sdk/getglassconfig";
				MOJING_TRACE(g_APIlogger, "GlassesConfigProfileV2 using inside address." << (IO == Profile_LOAD ? "Load..." : "Post..."));
#else
				strRet += "fuwu.mojing.cn/api/sdk/getglassconfig";
				MOJING_TRACE(g_APIlogger, "GlassesConfigProfileV2 using web address." << (IO == Profile_LOAD ? "Load..." : "Post..."));
#endif
				return strRet;
			}
			else if (szClassName == "InfoReporter" || szClassName == "DatabaseInfoReporter")
			{
#ifdef REPORTER_URL_ENCRYPT
//#ifdef _DEBUG
//				strRet = "https://192.168.12.58/logger.php";
//#else
                strRet += "mjsdk2.log.mojing.cn/logger.php";
//#endif
#else
   				strRet += "mjsdk.log.mojing.cn/logger.php";
#endif
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "InfoReporter using web address." << (IO == Profile_LOAD ? "Load..." : "Post..."));
#endif
				return strRet;
			}
			else if (szClassName == "UserActionReporter")
			{
#ifdef _DEBUG
				strRet += "192.168.12.58:8088/mojingcms02/170101-1-1-1/android/zh/1/user_actionup";
				MOJING_TRACE(g_APIlogger, "UserActionReporter using web address." << (IO == Profile_LOAD ? "Load..." : "Post..."));
#else
				strRet += "res.static.mojing.cn/170501-1-1-1/android/zh/1/user_actionup";
#endif
				return strRet;
			}
			else if (szClassName == "MojingMerchantVerify")
			{
				strRet += "open.mojing.cn/api/developer/merchantverify";
				MOJING_TRACE(g_APIlogger, "MerchantVerify using web address." << (IO == Profile_LOAD ? "Load..." : "Post..."));
				return strRet;
			}
			else
			{
				// 这里是原始的老戴的服务器
#ifdef _DEBUG
                strRet = "http://192.168.12.61:8886/release_api/v1.0/";
				MOJING_TRACE(g_APIlogger, "Profile using inside address." << (IO == Profile_LOAD ? "Load..." : "Post..."));
#else
				// strURL = "http://api.mojing.baofeng.com/mj/v1.0/"; // <-- 停用老的服务器
				strRet += "api.platform.mojing.cn/mj/v1.0/";
				MOJING_TRACE(g_APIlogger, "Profile using web address." << (IO == Profile_LOAD ? "Load..." : "Post..."));
#endif
				if (szClassName == "GyroTempCalibrationReporter")
				{
					if (IO == Profile_LOAD)
					{
						strRet += "mjswitchconfig.php";
					}
					else// IO == Profile_SAVE
					{
						strRet += "mjuploadconfig.php";
					}
				}
				else if (szClassName == "MobileConfigUpdate")
				{
					if (IO == Profile_LOAD)
					{
						strRet += "mjgetgyroscopeconfig.php";
					}
				}
				else if (szClassName == "CrashReporter")
				{
					if (IO == Profile_SAVE)
					{
						strRet += "mjsdkuploadcrash.php";
					}
				}
			}
			return strRet;
		}

		bool ProfileThreadMGR::UpdateProfile(String szClassName, MojingJsonElement *pElement, ProfileIO IO)
		{
			String szLocalProfile = GetLocalProfileName(szClassName);
			if (szLocalProfile != "")
			{
				return CreateLoaclProfileThread(szLocalProfile, szClassName, pElement, IO);
			}
            return  false;
		}

		void ProfileThreadMGR::UpdateInternetProfile(String szClassName, String strParamData, ProfileIO IO, FP_InternetProfileCallBack fpCallBack, void * CallBackParam, const char* szTransportFiles, bool bWait /*= false*/)
		{
			InternetProfile * pProfile = new InternetProfile();
			if (strParamData.Substring(0 , 4).ToLower() != "http")
			{
				String strURL = GetInternetURL(szClassName, IO);
				if (strURL.IsEmpty())
					return;

				pProfile->SetURL(strURL);
				pProfile->SetParametData(strParamData);
			}
			else
			{
				pProfile->SetURL(strParamData);
			}
			if (IO == Profile_SAVE)
			{
				if (szClassName == "InfoReporter" || szClassName == "DatabaseInfoReporter")
				{
					pProfile->SetFunction(IPF_GET_UP);
				}
				else
				{
					pProfile->SetFunction(IPF_POST);
				}
			}
			else if (IO == Profile_LOAD)
			{
				pProfile->SetFunction(IPF_GET_QUERY);
			}
			
			pProfile->SetCallBack(fpCallBack);
			pProfile->SetCallBackParam(CallBackParam);
			pProfile->SetTransportFiles(szTransportFiles ? szTransportFiles : "");
			if (bWait)
			{
				pProfile->Run();
				delete pProfile;
			}
			else
			{
				AppendThreadToRun(pProfile);
			}
		}
	}
}
