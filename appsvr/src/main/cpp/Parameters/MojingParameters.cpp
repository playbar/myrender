#include "MojingParameters.h"
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
		Parameters::Parameters()
		{
			m_pDisplayParameters = NULL;
			m_pDeviceParameters = NULL;
			m_pFactoryCalibrationParameters = NULL;
			m_pGlassesConfigProfile = NULL;
			m_pUserSettingProfile = NULL;
			m_bHDMWorking = false;
		}


		Parameters::~Parameters()
		{
            //if (m_pDeviceParameters)
			//	delete m_pDeviceParameters;
			//if (m_pDisplayParameters)
			//	delete m_pDisplayParameters;
			if (m_pFactoryCalibrationParameters)
				delete m_pFactoryCalibrationParameters;
			if (m_pSensorParameters)
				delete m_pSensorParameters;
			if (m_pGlassesConfigProfile)
				delete m_pGlassesConfigProfile;
			if (m_pUserSettingProfile)
				delete m_pUserSettingProfile;
		}
		bool Parameters::Init()
		{
			bool bRet = true;
			m_pFactoryCalibrationParameters = new MojingFactoryCalibrationParameters;
			m_pGyroOffsetReportSet = new MojingGyroOffsetReportSet(14 , 5);
			m_pSensorParameters = new MojingSensorParameters;
			m_pUserSettingProfile = new UserSettingProfile;
			// 读取用户配置文件
			m_pUserSettingProfile->Load();
			m_pDebugSettingsProfile = new DebugSettingsProfile;
			m_pDebugSettingsProfile->Load();
			MojingPlatformBase *pPlatform = MojingPlatformBase::GetPlatform();
			if (pPlatform)
			{
				/************************************************************************/
				/* 通过环境变量获得的平台相关数据                                       */
				/************************************************************************/
				m_pDeviceParameters = pPlatform->CreateDeviceParameters();
				m_pDisplayParameters = pPlatform->CreateDisplayParameters();
				

				m_pJoystickProfile = new  JoystickProfile;
				m_pJoystickProfile->UpdateFromProfile(pPlatform->GetPacketProfilePath());
				m_pJoystickProfile->CheckUpdate();

// 				if (m_pDeviceParameters && m_pDisplayParameters)
// 					bRet= m_pDisplayParameters->UpdatePPIFromModelName(m_pDeviceParameters->GetModel());

				GlassesConfigProfileV2* pGlassesConfigProfile = new GlassesConfigProfileV2;
				if (pGlassesConfigProfile->UpdateFromProfile(pPlatform->GetPacketProfilePath()))
				{
					m_pGlassesConfigProfile = pGlassesConfigProfile;
					// 升级
					pGlassesConfigProfile->CheckUpdate();
				}
				else
				{
					delete pGlassesConfigProfile;
				}
			}
			else
			{
				MOJING_ERROR(g_APIlogger , "Can not get Platform object");
				bRet = false;
			}
			return bRet;
		}
	}
}