#pragma once

#include "../Parameters/MojingJsonElement.h"
#include <map>
using namespace std;

namespace Baofeng
{
	namespace Mojing
	{
#ifdef MJ_OS_IOS 
        class JoystickProfileNode:public MojingJsonElement
		{
		public :
			JoystickProfileNode(){SetClassName(__FUNCTION__);};
            virtual ~JoystickProfileNode(){};
			virtual JSON* ToJson(){
				JSON *pRet = JSON::CreateObject();
				NameToJson(pRet);
				URLToJson(pRet);
				KeysToJson(pRet);
				return pRet;
			};
			virtual bool FromJson(JSON* pJson)
			{
				return IDFromJson(pJson) &&
					NameFromJson(pJson) &&
					URLFromJson(pJson)&&
					KeysFromJson(pJson);
			};
			virtual JSON* ToJsonWithOutKeys()
			{
				JSON *pRet = JSON::CreateObject();
				IDToJson(pRet);
				NameToJson(pRet);
				URLToJson(pRet);
				return pRet;
			}
			CLASS_INTERFACE_J(int ,m_i , ID);
			CLASS_INTERFACE_STR_J(String ,m_ , Name );
			CLASS_INTERFACE_STR_J(String ,m_ , URL);
			CLASS_INTERFACE_STR_J(String ,m_ , Keys);
		};
#endif
		class JoystickProfile
		{
#ifdef MJ_OS_IOS 
            std::map<int, JoystickProfileNode> m_AllNodes;
#endif
			// 检查网络升级反馈回调
			static void CheckUpdateCallBack(const unsigned char* lpszRespString, unsigned int uiSize, int RespCode, void * pCallBackParam);
		public:
			JoystickProfile();
			virtual ~JoystickProfile();
			bool UpdateFromProfile(const char * lpszProfilePath, JSON * pInternetJson = NULL);
			void CheckUpdate();
#ifdef MJ_OS_IOS 
			String GetJsonProfile();
			String GetJoystickKeys(int iID);
			bool LoadFromJSON(JSON * pJSON);
#endif

			CLASS_INTERFACE_STR(String, m_sz, ProfilePathName);
			CLASS_INTERFACE_STR_J(String, m_sz, ClassName);
			CLASS_INTERFACE_J(unsigned int ,m_ui , ReleaseDate);// 最新版本文件的发布日期
		};

	}
}