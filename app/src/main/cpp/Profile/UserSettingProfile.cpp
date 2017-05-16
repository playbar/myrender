#include "UserSettingProfile.h"
#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"

namespace Baofeng
{
	namespace Mojing
	{
#define MIN_SIZE 4.7
#define MAX_SIZE 7
		UserSettingProfile::UserSettingProfile()
		{
			SetClassName(__FUNCTION__);
			m_bEnableScreenSize = false;
			m_fScreenSize = 0;
			m_bSensorDataFromMJSDK = false;
			m_bSensorDataFromJava = false;
			m_dCheckGlassConfig = 0;
			m_dCheckMobileConfig = 0;
			m_dCheckJoystickProfile = 0;
		}


		UserSettingProfile::~UserSettingProfile()
		{
		}
		JSON* UserSettingProfile::ToJson()
		{
			JSON* pRet = JSON::CreateObject();
			ClassNameToJson(pRet);
			EnableScreenSizeToJson(pRet);
			ScreenSizeToJson(pRet);
			SensorDataFromMJSDKToJson(pRet);
			SensorDataFromJavaToJson(pRet);
			CheckGlassConfigToJson(pRet);
			CheckMobileConfigToJson(pRet);
			CheckJoystickProfileToJson(pRet);
			return pRet;
		}

		bool UserSettingProfile::FromJson(JSON* pJson)
		{	
			bool bRet = false;
			if (JSON * pClassName = pJson->GetItemByName("ClassName"))
			{
				if (pClassName && (pClassName->GetStringValue() == GetClassName()))
				{
					bRet = EnableScreenSizeFromJson(pJson) &&
						ScreenSizeFromJson(pJson);
					if (bRet)// 成功的条件是 设置禁用用户自定义屏幕尺寸或者设置在4.7到7英寸之间
					{
						bRet = (!m_bEnableScreenSize || (MIN_SIZE <= m_fScreenSize &&MAX_SIZE >= m_fScreenSize));
					}
					bool bCheckFromParameters = false;
					Manager* pManager = Manager::GetMojingManager();
					if (pManager)
					{
						Parameters* pParam = pManager->GetParameters();
						if (pParam)
						{
							MojingDeviceParameters* pDeviceParam = pParam->GetDeviceParameters();
							if (pDeviceParam && pDeviceParam->GetSensorDataFromJava())
							{
								bRet = (m_bSensorDataFromJava == false) | bRet;
								m_bSensorDataFromJava = true;
								bCheckFromParameters = true;
							}
						}
					}
					if (!bCheckFromParameters)
					{
						bRet = SensorDataFromJavaFromJson(pJson) | bRet;
					}

					SensorDataFromMJSDKFromJson(pJson);

					CheckMobileConfigFromJson(pJson);
					CheckGlassConfigFromJson(pJson);
					CheckJoystickProfileFromJson(pJson);

					/*
					JSON* pMobileCfgJson = pJson->GetItemByName("CheckMobileConfig");
					if (pMobileCfgJson)
						m_dCheckMobileConfig = pMobileCfgJson->GetDoubleValue();
					JSON* pGlassCfgJson = pJson->GetItemByName("CheckGlassConfig");
					if (pMobileCfgJson)
						m_dCheckGlassConfig = pGlassCfgJson->GetDoubleValue();
					JSON* pJoystickProfileJson = pJson->GetItemByName("CheckJoystickProfile");
					if (pJoystickProfileJson)
						m_dCheckJoystickProfile = pJoystickProfileJson->GetDoubleValue();
					*/
				}
			}

			return bRet;
		}
		float UserSettingProfile::GetUserScreenSizePPI()
		{
			float fPPI = 0;
			if (GetEnableScreenSize() && m_fScreenSize >= MIN_SIZE && m_fScreenSize <= MAX_SIZE)
			{
				Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
				MojingDisplayParameters * pDisplay = pParameters->GetDisplayParameters();
				fPPI = sqrtf(pDisplay->GetScreenWidth() * pDisplay->GetScreenWidth() + pDisplay->GetScreenHeight()*pDisplay->GetScreenHeight()) / m_fScreenSize;
			}
			return fPPI;
		}
	}
}