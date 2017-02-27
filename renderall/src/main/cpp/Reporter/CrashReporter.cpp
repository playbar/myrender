#include "CrashReporter.h"
#include "../MojingManager.h"
#include "../MojingSDKStatus.h"
#include "../Base/MojingCompress.h"
#include "../Platform/MojingPlatformBase.h"
#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDeviceParameters.h"
#include "../Profile/ProfileThreadMGR.h"
#include "../3rdPart/MD5/MD5.h"
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#ifdef MJ_OS_ANDROID
extern char *g_pAPPName;
#endif

namespace Baofeng
{
	namespace Mojing
	{
#ifdef USING_MINIDUMP
		CrashReporter::CrashReporter()
		{
			SetClassName(__FUNCTION__);
		}


		CrashReporter::~CrashReporter()
		{
		}

		CrashReporter* CrashReporter::GetCrashReporter(const char* szMiniDumpFileName)
		{
			CrashReporter* pRet = new CrashReporter;
			pRet->SetMiniDumpFileName(szMiniDumpFileName != NULL ? szMiniDumpFileName : "");
			return pRet;
		}

		bool CrashReporter::DoReport(bool bWait)
		{
			String szFileName = GetMiniDumpFileName();
			if (strstr(szFileName.ToCStr(), ".zip"))
			{
				return DoReport_ZIP(szFileName);
			}

			MD5 md5;
			char szTime[256];
			String data = "";
			Manager *pManager = Manager::GetMojingManager();
			MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus();

			String strAPP = /*AES_Value_S*/ (pStatus->GetApp());
#ifdef MJ_OS_ANDROID
			if (strAPP.IsEmpty() && g_pAPPName != NULL)
			{
				strAPP = g_pAPPName;
			}
#endif		
			data += "szapp=";
			data += strAPP;

			String strSDKVer = /*AES_Value_S*/(pStatus->GetSDKVersion());
			data += "&";
			data += "szsdkversion=";
			data += strSDKVer;

			data += "&";
			data += "szfunc=";
			data += /*AES_Value_S*/(g_MojingMinidump.szFunc);

			data += "&";
			data += "m_binit=";
			data += /*AES_Value_S*/(pStatus->GetInitStatus() == INIT_DONE ? "1" : "0" );

			data += "&";
			data += "m_bsensoron=";
			data += /*AES_Value_S*/(pStatus->GetTrackerStatus() == TRACKER_START ? "1" : "0");

			data += "&";
			data += "m_binmojingworld=";
			const char * szGlassName = pStatus->GetGlassesName();
			if (szGlassName == NULL || *szGlassName == 0)
				data += /*AES_Value_S*/("0");
			else
				data += /*AES_Value_S*/("1");

			data += "&";
			data += "szbrand=";
			data += /*AES_Value_S*/(pManager->GetParameters()->GetDeviceParameters()->GetBrand());

			data += "&";
			data += "szmodel=";
			data += /*AES_Value_S*/(pManager->GetParameters()->GetDeviceParameters()->GetModel());
			/*
			data += "&";
			data += "Serial=";
			data += AES_Value_S(pManager->GetParameters()->GetDeviceParameters()->GetSerial());
			*/
			data += "&";
			data += "szglassesname=";
			data += /*AES_Value_S*/MojingProfileKey::GetGlassKeyIDString(szGlassName);

			int iCurTime = GetCurrentTime();
			data += "&curr_time=";
			sprintf(szTime, "%d", iCurTime);
			data += szTime;

			data += "&sign=";
			// MAKE MD5
			md5.reset();
			size_t  buflen = strlen(szTime) + strAPP.GetSize() + strSDKVer.GetSize() + 64;
			char * pMD5SrcBuffer = new char[buflen];
			strcpy(pMD5SrcBuffer, szTime);
			strcat(pMD5SrcBuffer, strAPP.ToCStr());
			strcat(pMD5SrcBuffer, strSDKVer.ToCStr());
			strcat(pMD5SrcBuffer, "41f836e3d488337eeb49b7f6e87175db");
			md5.update(pMD5SrcBuffer, strlen(pMD5SrcBuffer));
			data += md5.toString();
			delete[] pMD5SrcBuffer;

			if (!szFileName.IsEmpty())
			{
				String szDmpZip = MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath();
				if (szDmpZip.Right(1) != "/")
					szDmpZip += "/";
				szDmpZip += "MojingSDK.CD.";
				
				char szRand[128];
				sprintf(szRand, "%s.%d.%04X.zip", 
					pManager->GetParameters()->GetDeviceParameters()->GetSerial() , 
						iCurTime, (int)rand());
				szDmpZip += szRand;
				if (MojingCompress::CompressFile(szFileName, szDmpZip, data.ToCStr()))
				{
					remove(szFileName);
					
					DoReport_ZIP(szDmpZip, bWait);
				}
			}
			else
			{
				SetMiniDumpFileName("");
				ProfileThreadMGR::UpdateInternetProfile(GetClassName(), data, Profile_SAVE, InternetProfileCallBack, this, GetMiniDumpFileName(), bWait);
			}
			//g_MojingMinidump.szFunc[0] == 0，表示这个异常不是由SDK的调用产生的。这种情况下返回false表示我们没有处理掉，需要系统继续处理。
			return g_MojingMinidump.szFunc[0] != 0;

		}

		void CrashReporter::InternetProfileCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			CrashReporter *pReporter = (CrashReporter *)pCallBackParam;
			const char * szMiniDumpFile = pReporter->GetMiniDumpFileName();
			if (szMiniDumpFile && *szMiniDumpFile && 200 == RespCode)
			{// 已经成功上传了崩溃日志文件，可以把本地的文件删掉了
				remove(pReporter->GetMiniDumpFileName());
			}
			if (pReporter)
				delete pReporter;
		}
		
		bool CrashReporter::DoReport_ZIP(const char* szMinidumpFile, bool bWait)
		{
			char * szReportParam = MojingCompress::GetFileAppendInfo(szMinidumpFile);
			if (szReportParam)
			{
				SetMiniDumpFileName(szMinidumpFile);
				delete szReportParam;
				ProfileThreadMGR::UpdateInternetProfile(GetClassName(), szReportParam, Profile_SAVE, InternetProfileCallBack, this, GetMiniDumpFileName(), bWait);
				return true;
			}
			else
			{// 不是可以发送的Zip文件

			}
			return false;
		}
#endif// end of USING_MINIDUMP
	}
}