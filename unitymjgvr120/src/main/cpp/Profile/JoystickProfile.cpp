#include "JoystickProfile.h"
#include <sys/time.h>
#include "../MojingManager.h"
#include "../3rdPart/MD5/MD5.h"
#include "../Parameters/MojingParameters.h"
#include "../Platform/MojingPlatformBase.h"
#include "../Profile/ProfileThreadMGR.h"
#include "../Reporter/ReporterTools.h"
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef MJ_OS_ANDROID
#include <jni.h>
extern JNIEnv* gJNIEnv;
extern JavaVM* gJavaVM;
extern jclass g_MojingInputManager ;
extern jmethodID g_midReloadConfig ;
#endif
extern MojingLogger g_APIlogger;
namespace Baofeng
{
	namespace Mojing
	{

		JoystickProfile::JoystickProfile()
		{
			m_uiReleaseDate = 0;
			SetClassName(__FUNCTION__);
		}


		JoystickProfile::~JoystickProfile()
		{
#ifdef MJ_OS_IOS
			m_AllNodes.clear();
#endif
		}

		bool JoystickProfile::UpdateFromProfile(const char * lpszProfilePath, JSON * pJsonFromUpdate)
		{
			char filename[256];
			const char* szError = NULL;
			strcpy(filename, lpszProfilePath);
			if (filename[strlen(filename) - 1] != '/')
				strcat(filename, "/");
#ifdef MJ_OS_IOS
			strcat(filename, GetClassName());
			strcat(filename, ".json");
#else
			strcat(filename, "InputMap_mojing_default.json");
#endif
			JSON *pJsonFromPacket = JSON::Load(filename, NULL, &szError);

			if (pJsonFromPacket == NULL && pJsonFromUpdate == NULL)
			{
				MOJING_ERROR(g_APIlogger, "Can not load profile from : " << lpszProfilePath);
				return false;
			}
			
			JSON * pJsonFromSDCard = NULL;
			String LocalPath = MojingPlatformBase::GetPlatform()->GetDefaultLocalProfilePath();
			if (LocalPath.Right(1) != "/")
				LocalPath += "/";
			LocalPath += GetClassName();
			LocalPath += ".dat";

			pJsonFromSDCard = JSON::Load(LocalPath, NULL, (char const**)&szError);

			MOJING_TRACE(g_APIlogger, "Load profile from PACKET = " << (pJsonFromPacket ? "YES" : "NO") << "; SDCARD = " << (pJsonFromSDCard ? "YES" : "NO") << "; Update = " << (pJsonFromUpdate ? "YES" : "NO"));
			char szNewVersion[16];
			*szNewVersion = 0;
			JSON *pUseNode = NULL;
#ifdef _DEBUG
			pUseNode = pJsonFromSDCard ? pJsonFromSDCard : pJsonFromPacket;
			pUseNode = pJsonFromUpdate ? pJsonFromUpdate : pUseNode;
#else
			pUseNode = JSON_CompReleaseData(pJsonFromPacket, pJsonFromSDCard, szNewVersion);
			pUseNode = JSON_CompReleaseData(pUseNode, pJsonFromUpdate, szNewVersion);
#ifdef MJ_OS_IOS
            if (pJsonFromSDCard == NULL)
            {
                int iReleaseDate = 0;
                SetReleaseDate(iReleaseDate);
            }
            else
            {
                ReleaseDateFromJson(pJsonFromSDCard);
            }
#endif
#ifndef MJ_OS_IOS
			int iReleaseDate = 0;
			sscanf(szNewVersion, "%d", &iReleaseDate);
			SetReleaseDate(iReleaseDate);
#endif
#endif
			if (pUseNode == pJsonFromPacket)
			{
				MOJING_TRACE(g_APIlogger, "Using PACKET Profile");
				SetProfilePathName(filename);
			}
			else if (pUseNode == pJsonFromSDCard)
			{
				MOJING_TRACE(g_APIlogger, "Using SDCARD Profile");
				SetProfilePathName(LocalPath);
			}
			else if (pUseNode == pJsonFromUpdate)
			{
				MOJING_TRACE(g_APIlogger, "Using UPDATE Profile");
				pJsonFromUpdate->Save(LocalPath);
				SetProfilePathName(LocalPath);
			}
#ifdef MJ_OS_IOS			
			LoadFromJSON(pUseNode);
#endif
			if (pJsonFromPacket)
			{
				pJsonFromPacket->Release();
				pJsonFromPacket = NULL;
			}
			if (pJsonFromSDCard)
			{
				pJsonFromSDCard->Release();
				pJsonFromSDCard = NULL;
			}

			int iNewVersion = 20150101;
			if (*szNewVersion != 0)
				sscanf(szNewVersion, "%d", &iNewVersion);

			int iRet = iNewVersion != (int)GetReleaseDate();

			return iRet;
		}


		void JoystickProfile::CheckUpdate()
		{
			double dLastCheckJoystickProfileTime = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->GetCheckMobileConfig();
			//request once per day.
			if (fabs(ReporterTools::GetCurrentTime() - dLastCheckJoystickProfileTime) < CHECK_JOYSTICKPROFILE_INTERVEL)
			{
				return;
			}
			
			char szTime[32];
			char szReleaseDate[32];
			String data;

			data += "releasedate=";
			sprintf(szReleaseDate, "%u", m_uiReleaseDate);
			data += szReleaseDate;
			data += "&curr_time=";
			MOJING_TRACE(g_APIlogger, "Check update Version = " << m_uiReleaseDate);
			struct timeval curtimeval;
			struct timezone curtimezone;
			
			gettimeofday(&curtimeval, &curtimezone);
			sprintf(szTime, "%u", (int)curtimeval.tv_sec);
			data += szTime;
			/////////////////////////////////////////////////////

#ifdef MJ_OS_IOS
			data += "&os_type=2";
#else// MJ_OS_ANDROID
			data += "&os_type=1";
#endif

			MD5 md5;
			md5.reset();

			size_t  buflen = strlen(szTime) + strlen(szReleaseDate) + 128;
			char * pMD5SrcBuffer = new char[buflen];
			strcpy(pMD5SrcBuffer, szReleaseDate);
			strcat(pMD5SrcBuffer, szTime);
			strcat(pMD5SrcBuffer, "26c216f0a56c6c385b4c3d8e28f452f8");
			data += "&sign=";

			md5.update(pMD5SrcBuffer, strlen(pMD5SrcBuffer));
			data += md5.toString();
			delete[] pMD5SrcBuffer;

			MOJING_TRACE(g_APIlogger, "URL_DATA = " << data);
			ProfileThreadMGR::UpdateInternetProfile(GetClassName(), data, Profile_LOAD, CheckUpdateCallBack, this);
		}

		void JoystickProfile::CheckUpdateCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			if (RespCode != 200)
			{
				MOJING_TRACE(g_APIlogger, "Update FAILD! Code = " << RespCode);
				return;
			}
			
			Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->SetCheckJoystickProfile(ReporterTools::GetCurrentTime());
			Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile()->Save();
			
			//MOJING_TRACE(g_APIlogger, "JoystickProfile::CheckUpdateCallBack res: " << lpszRespString);
			JoystickProfile * pThis = (JoystickProfile *)pCallBackParam;
			char *pBuffer = new char[uiSize + 1];
			memcpy(pBuffer, lpszRespString, uiSize);
			pBuffer[uiSize] = 0;
			JSON *pJson = JSON::Parse(pBuffer);
			delete[] pBuffer;
			if (pJson != NULL)
			{
				if (JSON *pResultNode = pJson->GetItemByName("Result"))
				{
					if (pResultNode->GetStringValue().CompareNoCase("SUCCEED") == 0)
					{
						JSON *pProfileNode = pJson->GetItemByName("JoystickConfig");
						if (pProfileNode && pProfileNode->Type == JSON_Object)
						{
							JoystickProfile *pProfile = (JoystickProfile *)pCallBackParam;
							MojingPlatformBase *pPlatform = MojingPlatformBase::GetPlatform();
							if (pPlatform)
							{
								pProfile->UpdateFromProfile(pPlatform->GetPacketProfilePath(), pProfileNode);
							}
						}
					}
				}
				pJson->Release();
				pJson = NULL;
			}
		}

#ifdef MJ_OS_IOS 

		bool JoystickProfile::LoadFromJSON(JSON * pJSON)
		{
            if(pJSON == NULL) return false;
            
			JSON *pClass = pJSON->GetItemByName("Class");
			JSON *pReleaseDate = pJSON->GetItemByName("ReleaseDate");
			JSON *pOS = pJSON->GetItemByName("OS");
            if(pOS == NULL || pClass == NULL || pReleaseDate == NULL)
                return false;
            String szOSName = pOS->GetStringValue();
			JSON *pJoystickConfig = pJSON->GetItemByName("JoystickConfig");
			if (pJoystickConfig != NULL && pJoystickConfig->Type == JSON_Array)
			{
                String szClassName = pClass->GetStringValue();
                if ( szClassName == GetClassName() && pReleaseDate->GetInt32Value() >= GetReleaseDate() && pJoystickConfig->Type == JSON_Array && szOSName == "iOS")
				{
					m_AllNodes.clear();
					int iItemCount = pJoystickConfig->GetItemCount();
					for (int iItem = 0 ; iItem < iItemCount ; iItem++)
					{
						JSON *pSubItem = pJoystickConfig->GetItemByIndex(iItem);
						JoystickProfileNode JPN;
						if (JPN.FromJson(pSubItem))
						{
							m_AllNodes[JPN.GetID()] = JPN;
						}
					}
                    ReleaseDateFromJson(pJSON);
					return true;
				}
			}
			return false;
		}

		String JoystickProfile::GetJsonProfile()
		{
			String szRet;
			JSON *pJsonRet = JSON::CreateObject();
			ClassNameToJson(pJsonRet);
			JSON *pJsonConfig = JSON::CreateArray();
			for (map<int, JoystickProfileNode>::iterator  it = m_AllNodes.begin() ; it != m_AllNodes.end() ; it++)
			{
				pJsonConfig->AddArrayElement(it->second.ToJsonWithOutKeys());
			}
			pJsonRet->AddItem("JoystickProfile" , pJsonConfig);

			szRet = pJsonRet->PrintValue(0,0);
			pJsonRet->Release();
			return szRet;
		}

		String JoystickProfile::GetJoystickKeys(int iID)
		{
			String szRet;
			map<int, JoystickProfileNode>::iterator  it =  m_AllNodes.find(iID);
			if (it != m_AllNodes.end())
			{
				szRet = it->second.GetKeys();
			}
			return szRet;
		}
#endif

	}
}
