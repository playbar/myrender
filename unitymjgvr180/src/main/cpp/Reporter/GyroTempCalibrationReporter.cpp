#include "GyroTempCalibrationReporter.h"
#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"
#include "../Platform/MojingPlatformBase.h"
#include "../Tracker/MojingTracker.h"
#include "../3rdPart/MD5/MD5.h"
#include "../3rdPart/AES/AESPro.h"
#include "../Profile/InternetProfile.h"
#include "../Profile/ProfileThreadMGR.h"
#include "../Base/MojingTypes.h"

#include "../MojingSDKStatus.h"
#include "../Render/MojingRenderBase.h"
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
		// 注意：因为本对象可能被多次调用，所以建立一个全局对象，运行期间不释放
		GyroTempCalibrationReporter *g_pGyroTempCalibrationRepoterRepoter = NULL;
		GyroTempCalibrationReporter::GyroTempCalibrationReporter()
		{
			SetClassName(__FUNCTION__);
			SetFile(MojingSDKStatus::GetSDKStatus()->GetSDKVersion());
			SetApp("UNKNOWN");
			SetMobile("UNKNOWN");
			SetDisplay("UNKNOWN");
			SetSensor("UNKNOWN");		
			SetBrand("UNKNOWN");
			SetSerial("UNKNOWN");
			SetOS(MojingPlatformBase::GetPlatform()->GetOSString());
			SetGLES("UNKNOWN");
			SetEGL("UNKNOWN");
			SetGlassesName("UNKNOWN");
			SetGlassesReportName("UNKNOWN");
			SetMojingSN("");
			SetGyroOffsetReportSet("UNKNOWN");
			SetEnablePostRepote(false);
		}


		GyroTempCalibrationReporter::~GyroTempCalibrationReporter()
		{

		}
		GyroTempCalibrationReporter* GyroTempCalibrationReporter::GetGyroTempCalibrationRepoter()
		{
			if (g_pGyroTempCalibrationRepoterRepoter == NULL)
				g_pGyroTempCalibrationRepoterRepoter = new GyroTempCalibrationReporter;

			return g_pGyroTempCalibrationRepoterRepoter;
		}

	

		void GyroTempCalibrationReporter::Post()
		{
			if (!GetEnablePostRepote())
			{
				MOJING_TRACE(g_APIlogger, "GyroTempCalibrationReporter :: DISABLED.");
				return;
			}

			Manager *pManager = Manager::GetMojingManager();
			if (pManager == NULL)
			{
				return;
			}
			// update display info
			JSON *pJson = pManager->GetParameters()->GetDisplayParameters()->ToJson();
			if (pJson)
			{
				/*拼接平均显示帧率信息*/
				char *pJsonValue = pJson->PrintValue(0, false);
				SetDisplay(pJsonValue);
				MJ_FREE(pJsonValue);
				pJson->Release();
			}
			else
			{
				SetDisplay("UNKNOWN");
			}

#if 0
			pJson = pManager->GetParameters()->GetSensorParameters()->ToJson();
			if (pJson)
			{
				char *pJsonValue = pJson->PrintValue(0, false);
				SetSensor(pJsonValue);
				MJ_FREE(pJsonValue);
				pJson->Release();
			}
			else
			{
				SetSensor("UNKNOWN");
			}
#else
			Tracker* pTracker = pManager->GetTracker();
			if (pTracker)
			{
				SetSensor(pTracker->GetCheckSensorString());
			}
			else
			{
				SetSensor("UNKNOWN");
			}
#endif

			MD5 md5;
			char szTime[256];
			String data = "file=";
			data += AES_Value_S(GetFile());
			data += "&app=";
            //MOJING_TRACE(g_APIlogger, "<<<<1>>>>");
			data += AES_Value_S(ConvertToUrlEncode(GetApp()));
			data += "&brand=";
            //MOJING_TRACE(g_APIlogger, "<<<<2>>>>");
			data += AES_Value_S(ConvertToUrlEncode(GetBrand()));
			data += "&mobile=";
			data += AES_Value_S(GetMobile());
			data += "&serial=";
            //MOJING_TRACE(g_APIlogger, "<<<<3>>>>");
			data += AES_Value_S(ConvertToUrlEncode(GetSerial()));
			data += "&display=";
			String strDisplay = AES_Value_S(GetDisplay());
			data += strDisplay;
			data += "&sensor=";
			String strSenser = AES_Value_S(GetSensor());
			data += strSenser;
			data += "&os=";
			data += AES_Value_S(GetOS());
			data += "&gles=";
			data += AES_Value_S(GetGLES());
			data += "&egl=";
			data += AES_Value_S(GetEGL());
			data += "&glassesname=";
			data += AES_Value_S(GetGlassesName());
			data += "&gyrooffsetreportset=";
			String strJason = AES_Value_S("RESERVED");//(GetGyroOffsetReportSet());
			data += strJason;

			data += "&curr_time=";
			sprintf(szTime, "%d", GetCurrentTime());
			data += szTime;

			data += "&sign=";
			// MAKE MD5
			md5.reset();
			size_t  buflen = strlen(szTime) + strJason.GetSize() + strDisplay.GetSize() + strSenser.GetSize() + 128;
			char * pMD5SrcBuffer = new char[buflen];
			strcpy(pMD5SrcBuffer, szTime);
			strcat(pMD5SrcBuffer, strJason.ToCStr());
			strcat(pMD5SrcBuffer, strDisplay.ToCStr());
			strcat(pMD5SrcBuffer, strSenser.ToCStr());
			strcat(pMD5SrcBuffer, "41f836e3d488337eeb49b7f6e87175db");
			md5.update(pMD5SrcBuffer, strlen(pMD5SrcBuffer));
			data += md5.toString();
			delete[] pMD5SrcBuffer;
			// post by thread 
			ProfileThreadMGR::UpdateInternetProfile(GetClassName() , data , Profile_SAVE , NULL , NULL);
		}	

		void GyroTempCalibrationReporter::UpdateConfig()
		{
			String data = "curr_time=";
			MD5 md5;
			char szTime[256];
			sprintf(szTime, "%d", GetCurrentTime());
			data += szTime;
			data += "&sign=";
			size_t  buflen = strlen(szTime) + 64;
			char * pMD5SrcBuffer = new char[buflen];
			strcpy(pMD5SrcBuffer, szTime);
			strcat(pMD5SrcBuffer, "41f836e3d488337eeb49b7f6e87175db");
			md5.update(pMD5SrcBuffer, strlen(pMD5SrcBuffer));
			data += md5.toString();
			delete[] pMD5SrcBuffer;

			ProfileThreadMGR::UpdateInternetProfile(GetClassName(), data, Profile_LOAD, InternetProfileCallBack, this);
		}

		void GyroTempCalibrationReporter::InternetProfileCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			GyroTempCalibrationReporter *pRepoter = (GyroTempCalibrationReporter *)pCallBackParam;
			
			char *pBuffer = new char[uiSize + 1];
			memcpy(pBuffer, lpszRespString, uiSize);
			pBuffer[uiSize] = 0;

			JSON *pJson = JSON::Parse(pBuffer);

			delete[] pBuffer;

			if (pJson)
			{
				const char *pValueObjectName = "config";
				if (JSON *pValueObjectNode = pJson->GetItemByName(pValueObjectName))
				{
					const char *pValueName = "upload_switch";
					if (JSON *pValueNode = pValueObjectNode->GetItemByName(pValueName))
					{
						if (pValueNode->Type == JSON_String)
						{
							pRepoter->SetEnablePostRepote(pValueNode->GetStringValue() == "1");
						}
					}
				}
				pJson->Release();
			}
		}
	}
}