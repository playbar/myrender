#include "MojingSDKStatus.h"
#include "Base/Version.h"
#include "Platform/MojingPlatformBase.h"

namespace Baofeng
{
	namespace Mojing
	{
		MojingSDKStatus *g_pMojingSDKStatus = NULL;
		MojingSDKStatus::MojingSDKStatus():
			m_ui64SDKBuildNumber(0),
			m_VerfiyStatus(VERIFY_OK)
		{
			String strVersion = "MojingSDK ";
#ifdef _DEBUG
			strVersion += "(D) ";
#else
			strVersion += "(R) ";
#endif
#ifdef MJ_OS_MAC
            //strVersion += __DATE__" "__TIME__;
#else
            strVersion += "A " __DATE__ " " __TIME__;
#endif
            SetSDKVersion(SDK_VERSION);

			SetGlassesName("");
			SetInitStatus(INIT_NO);
            SetVerfiyStatus(VERIFY_OK);
			SetTrackerStatus(TRACKER_STOP);
			SetEngineStatus(ENGINE_NONE);
			SetSensorOrigin(SENSOR_FROM_DEFAULT);
			SetEngine("");
			ParseVersion();
		}


		MojingSDKStatus::~MojingSDKStatus()
		{
		}

		void MojingSDKStatus::ParseVersion()
		{
			m_ui64SDKBuildNumber = 0;
			char szTemp[128];
			strcpy(szTemp, GetSDKVersion());
			char *pEndPos = strchr(szTemp , '(');
			if (pEndPos)
			{
				char *pBeginPos = pEndPos - 1;
				while (pBeginPos > szTemp && *pBeginPos != '.')
				{
					pBeginPos--;
				}
				if (*pBeginPos == '.')
				{
					pBeginPos++;
					while (*pBeginPos >= '0' && *pBeginPos <= '9')
					{
						m_ui64SDKBuildNumber *= 10;
						m_ui64SDKBuildNumber += *pBeginPos - '0';
						pBeginPos++;
					}
				}
			}
		}
		MojingSDKStatus * MojingSDKStatus::GetSDKStatus()
		{
			if (g_pMojingSDKStatus == NULL)
				g_pMojingSDKStatus = new MojingSDKStatus;

			return g_pMojingSDKStatus;
		}

		bool MojingSDKStatus::IsMojingSDKEnbaled()
		{
			if (GetInitStatus() == INIT_DONE )
			{
				return  true;
			}

			return false;
		}
	}
}

