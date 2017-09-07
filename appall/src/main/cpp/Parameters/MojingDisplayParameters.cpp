#include "MojingDisplayParameters.h"
#include "../Base/MojingTypes.h"
#include "../MojingManager.h" 
#include "../Parameters/MojingParameters.h"
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
		//MODEL = SM-N9108V
		MojingDisplayParameters::MojingDisplayParameters() :m_fScale(1), m_fAvgRenderTime(0),m_fUserPPI(0), m_fMobileEdge(0.004)
// 			: m_fScreenHeightMeter(0)
// 			, m_fScreenWidthMeter(0)
		{
			SetClassName(__FUNCTION__);
		}


		MojingDisplayParameters::~MojingDisplayParameters()
		{
		}

		bool MojingDisplayParameters::UpdatePPIFromUserSetting()
		{
			UserSettingProfile* pUserSettingProfile = Manager::GetMojingManager()->GetParameters()->GetUserSettingProfile();
			float fUserSettingPPI = 0;
			if (pUserSettingProfile && pUserSettingProfile->GetUserScreenSizePPI())
			{
				fUserSettingPPI = pUserSettingProfile->GetUserScreenSizePPI();
				SetUserPPI(fUserSettingPPI);
				MOJING_TRACE(g_APIlogger, "Using User PPI = " << fUserSettingPPI);
				return true;
			}
			else
			{
				SetUserPPI(0);
			}
			return false;
		}
		
		JSON* MojingDisplayParameters::ToJson()
		{
			JSON* pJson = JSON::CreateObject();
			ClassNameToJson(pJson);
			ClassVersionToJson(pJson);
			ScreenWidthToJson(pJson);
			ScreenHeightToJson(pJson);
// 			DensityToJson(pJson);
// 			DensityDPIToJson(pJson);
// 			ScaledDensityToJson(pJson);
			XdpiToJson(pJson);
			YdpiToJson(pJson);
			PPIToJson(pJson);
			// 20160114 因为这个东西会捣乱，影响我们洗数据判定手机型号，所以要取消掉
			// AvgRenderTimeToJson(pJson);
			return pJson;
		}
		bool MojingDisplayParameters::FromJson(JSON*pJsonItem)
		{
			JSON*pJson = pJsonItem->GetItemByName("DisplayParameters");
			if (pJson)
			{
				ScreenWidthFromJson(pJson);
				ScreenHeightFromJson(pJson);
				XdpiFromJson(pJson);
				YdpiFromJson(pJson);
				PPIFromJson(pJson);

			//	SetScreenMeters();
				return true;
			}
			return false;
		}

		float MojingDisplayParameters::GetScreenPPI()
		{
			float fRet = m_fPPI < 1 ? m_fYdpi : m_fPPI;
			fRet = m_fUserPPI < 1 ? fRet : m_fUserPPI;
			return fRet;
		}

		float MojingDisplayParameters::GetScreenWidthMeter()const
		{
			const static float METERS_PER_INCH = 0.0254f;
			float fPPI = m_fPPI < 1 ? m_fYdpi : m_fPPI;
			fPPI = m_fUserPPI < 1 ? fPPI : m_fUserPPI;

			return m_iScreenWidth * METERS_PER_INCH / fPPI;
		}
		
		float MojingDisplayParameters::GetScreenHeightMeter()const
		{
			const static float METERS_PER_INCH = 0.0254f;
			float fPPI = m_fPPI < 1 ? m_fXdpi : m_fPPI;
			fPPI = m_fUserPPI < 1 ? fPPI : m_fUserPPI;
			return m_iScreenHeight * METERS_PER_INCH / fPPI;
		}

		float MojingDisplayParameters::Pix2Meter(float fPix)const
		{
			const static float METERS_PER_INCH = 0.0254f;
			float fPPI = m_fPPI < 1 ? m_fXdpi : m_fPPI;
			fPPI = m_fUserPPI < 1 ? fPPI : m_fUserPPI;
			return fPix * METERS_PER_INCH / fPPI;
		}
		float MojingDisplayParameters::Meter2Pix(float fMeter)const
		{
			const static float METERS_PER_INCH = 0.0254f;
			float fPPI = m_fPPI < 1 ? m_fXdpi : m_fPPI;
			fPPI = m_fUserPPI < 1 ? fPPI : m_fUserPPI;
			return fMeter / METERS_PER_INCH * fPPI;
		}

// 		void MojingDisplayParameters::SetScreenMeters(void)
// 		{
// 			const static float METERS_PER_INCH = 0.0254f;
// 			if (m_fPPI < 1)
// 			{
// 				m_fScreenHeightMeter = m_iScreenHeight * METERS_PER_INCH / m_fXdpi;
// 				m_fScreenWidthMeter = m_iScreenWidth * METERS_PER_INCH / m_fYdpi;
// 			}
// 			else
// 			{
// 				m_fScreenHeightMeter = m_iScreenHeight * METERS_PER_INCH / m_fPPI;
// 				m_fScreenWidthMeter = m_iScreenWidth * METERS_PER_INCH / m_fPPI;
// 			}
// 		}
	}//namespace Mojing
}//namespace Baofeng