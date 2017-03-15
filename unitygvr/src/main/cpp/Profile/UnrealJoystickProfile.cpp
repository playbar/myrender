#include "UnrealJoystickProfile.h"
#include <sys/time.h>
#include "../3rdPart/MD5/MD5.h"
#include "../Parameters/MojingParameters.h"
#include "../Platform/MojingPlatformBase.h"
#include "../Profile/ProfileThreadMGR.h"
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
#include "../MojingManager.h"
#include "../Interface/Android/MojingNativeInput.h"
extern MojingLogger g_APIlogger;
namespace Baofeng
{
	namespace Mojing
	{
#ifdef MJ_OS_ANDROID



        UnrealJoystickProfile::UnrealJoystickProfile()
		{
			m_uiReleaseDate = 0;
			SetClassName(__FUNCTION__);
		}


        UnrealJoystickProfile::~UnrealJoystickProfile()
		{
		}

        bool UnrealJoystickProfile::getDeviceMap(std::map<String, int> & devmap)
        {
            JoystickProfile* pProfile = Manager::GetMojingManager()->GetParameters()->GetJoystickProfile();
            MojingPlatformBase *pPlatform = MojingPlatformBase::GetPlatform();
            pProfile->UpdateFromProfile(pPlatform->GetPacketProfilePath());
            pProfile->CheckUpdate();// 启动网络更新
            String filepath = pProfile->GetProfilePathName();

            const char* szError = NULL;
            JSON *pJson = JSON::Load(filepath, NULL, &szError);
            if (pJson == NULL)
            {
                MOJING_ERROR(g_APIlogger, "Can not load profile from : " << filepath);
                return false;
            }
            
            JSON* pConfig = pJson->GetItemByName("JoyStickConfig");
            
            if (NULL == pConfig)
            {
                return false;
            }

            if (pConfig->Type != JSON_Array)
            {
                return false;
            }
            int iSize = pConfig->GetArraySize();
            JSON* pUnrealConfigList = NULL;
            JSON* pUnrealConfig = NULL;
            //JSON* p
            for (int Index = 0; Index < iSize ; Index++)
            {
                pUnrealConfigList = pConfig->GetItemByIndex(Index);

                if (pUnrealConfigList)
                {
                     pUnrealConfig = pUnrealConfigList->GetItemByName("JoyStickTypeListForUnreal");
                     if (pUnrealConfig)
                     {
                        break;
                     }
                }
            }


            if (pUnrealConfig->Type != JSON_Array)
            {
                return false;
            }


            JSON* pUnrealConfigItem = NULL;
            iSize = pUnrealConfig->GetArraySize();
            for (int Index = 0; Index < iSize; Index++)
            {
                pUnrealConfigItem = pUnrealConfig->GetItemByIndex(Index);
                JSON* p = pUnrealConfigItem->GetItemByName("Connection");
                if (p)
                {
                    if (0 != strcmp(p->GetStringValue(), "bluetooth"))
                    {
                        continue;
                    }
                }
                else
                {
                    continue;
                }

                p = pUnrealConfigItem->GetItemByName("ClassName");
                const char* pstr = p->GetStringValue();
                String str = pstr;
                int iMojingType = UnrealJoystickProfile::GetMojingType(str);
                JSON* pDeviceNamelist = pUnrealConfigItem->GetItemByName("JoyStickList");
                if (pDeviceNamelist != NULL && pDeviceNamelist->Type != JSON_Array)
                {
                    continue;
                }
                
                for (int Index = 0; Index < pDeviceNamelist->GetArraySize() ; Index++)
                {
                    JSON *p = pDeviceNamelist->GetItemByIndex(Index);
                    if (p)
                    {
                        devmap[p->GetStringValue()] = iMojingType;
                    }
                }
            }

            return true;
        }

        int UnrealJoystickProfile::GetMojingType(String & name)
        {
            int iRet = MID_NONE;
            if (0 == strcmp("Mojing2", name.ToCStr()))
            {
                iRet = MID_JOYSTICK_MJ2;
            }
            else if (0 == strcmp("Mojing3", name.ToCStr()))
            {
                iRet = MID_JOYSTICK_MJ3;
            }
            else if (0 == strcmp("Mojing4", name.ToCStr()))
            {
                iRet = MID_JOYSTICK_MJ4;
            }
            else if (0 == strcmp("Mojing5", name.ToCStr()))
            {
                iRet = MID_JOYSTICK_MJ5;
            }
            else if (0 == strcmp("mojing-motion", name.ToCStr()))
            {
                iRet = MID_JOYSTICK_MJ_Motion;
            }
            return iRet;
        }

        //         bool UnrealJoystickProfile::getDeviceMap(std::map<String, std::map<int, int>> & devmap)
//         {
//             JoystickProfile* pProfile = Manager::GetMojingManager()->GetParameters()->GetJoystickProfile();
//             MojingPlatformBase *pPlatform = MojingPlatformBase::GetPlatform();
//             pProfile->UpdateFromProfile(pPlatform->GetPacketProfilePath());
//             pProfile->CheckUpdate();// 启动网络更新
//             String filepath = pProfile->GetProfilePathName();
// 
//             const char* szError = NULL;
//             JSON *pJson = JSON::Load(filepath, NULL, &szError);
//             if (pJson == NULL)
//             {
//                 MOJING_ERROR(g_APIlogger, "Can not load profile from : " << filepath);
//                 return false;
//             }
// 
//             JSON* pConfig = pJson->GetItemByName("JoyStickConfig");
// 
//             if (NULL == pConfig)
//             {
//                 return false;
//             }
// 
//             if (pConfig->Type != JSON_Array)
//             {
//                 return false;
//             }
//             int iSize = pConfig->GetArraySize();
//             JSON* pUnrealConfigList = NULL;
//             JSON* pUnrealConfig = NULL;
//             //JSON* p
//             for (int Index = 0; Index < iSize ; Index++)
//             {
//                 pUnrealConfigList = pConfig->GetItemByIndex(Index);
// 
//                 if (pUnrealConfigList)
//                 {
//                     pUnrealConfig = pUnrealConfigList->GetItemByName("JoyStickTypeListForUnreal");
//                     if (pUnrealConfig)
//                     {
//                         break;
//                     }
//                 }
//             }
// 
// 
//             if (pUnrealConfig->Type != JSON_Array)
//             {
//                 return false;
//             }
// 
// 
//             JSON* pUnrealConfigItem = NULL;
//             iSize = pUnrealConfig->GetArraySize();
//             for (int Index = 0; Index < iSize; Index++)
//             {
//                 pUnrealConfigItem = pUnrealConfig->GetItemByIndex(Index);
//                 JSON* p = pUnrealConfigItem->GetItemByName("Connection");
//                 if (p)
//                 {
//                     if (0 != strcmp(p->GetStringValue(), "bluetooth"))
//                     {
//                         continue;
//                     }
//                 }
//                 else
//                 {
//                     continue;
//                 }
// 
//                 JSON* pKeymap = pUnrealConfigItem->GetItemByName("KeyMap");
// 
//                 if (pKeymap->Type != JSON_Array)
//                 {
//                     continue;
//                 }
//                 iSize = pKeymap->GetArraySize();
//                 if (iSize == 0)
//                 {
//                     continue;
//                 }
// 
//                 std::map<int, int> keymaptemp;
//                 for (int Index = 0; Index < iSize; Index++)
//                 {
//                     JSON* pMapItem = pKeymap->GetItemByIndex(Index);
// 
//                     if (pMapItem == NULL)
//                     {
//                         continue;
//                     }
// 
//                     JSON* pSrcCode = pMapItem->GetItemByName("DesCode");
//                     JSON* pDesCode = pMapItem->GetItemByName("SrcCode");
// 
//                     keymaptemp.insert(std::map<int, int>::value_type(pSrcCode->GetInt32Value(), pDesCode->GetInt32Value()));
// 
//                 }
// 
//                 p = pUnrealConfigItem->GetItemByName("ClassName");
//                 String str = p->GetStringValue();
//                 JSON* pDeviceNamelist = pUnrealConfigItem->GetItemByName("JoyStickList");
//                 if (pDeviceNamelist->Type != JSON_Array)
//                 {
//                     continue;
//                 }
// 
//                 std::list<STr>
//                     for (int Index = 0; Index < pDeviceNamelist->GetArraySize() ; Index++)
//                     {
//                         //	keymap.insert()
//                     }
//             }
// 
//             return true;
// 
//         }
#if 0

        bool UnrealJoystickProfile::UpdateFromProfile(const char * lpszProfilePath, JSON * pJsonFromUpdate)
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


        void UnrealJoystickProfile::CheckUpdate()
		{
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

        void UnrealJoystickProfile::CheckUpdateCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam)
		{
			if (RespCode != 200)
			{
				MOJING_TRACE(g_APIlogger, "Update FAILD! Code = " << RespCode);
				return;
			}
			
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
#endif
#endif
	}
}