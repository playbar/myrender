#include <stddef.h>
#include <unistd.h>
#include "Base/MojingTypes.h"
#include "MojingManager.h"
#include "Tracker/MojingTracker.h"

#if defined(MJ_OS_ANDROID)
//#include "Tracker/MojingControlPose.h"
#include "Tracker/MojingControllerSocket.h"
#include "Tracker/MojingGlassSensor.h"
#include "Tracker/MojingAndroidSensor.h"
#include "Tracker/AndroidInternalSensorChecker.h"
#elif defined(MJ_OS_MAC)
#include "Tracker/MojingIOSSensor.h"
#include "Tracker/MojingControllerSocket.h"
#elif defined(MJ_OS_WIN32)
#include "Tracker/MojingWinSensor.h"
#endif
#include "Distortion/MojingDistortion.h"
#include "Parameters/MojingParameters.h"
#include "Reporter/DatabaseInfoReporter.h"

namespace Baofeng
{
	namespace Mojing
	{
		Manager::Manager()
			: m_bInited(false)
			, m_pTracker(NULL)
			, m_pDistortion(NULL)
			, m_pParameters(NULL)
			, m_pSensor(NULL)
			, m_pDefaultSensor(NULL)
#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
   			, m_pControlTracker(NULL)
#endif
#ifdef MJ_OS_ANDROID
			, m_pGlassSensor(NULL)
			, m_pCheckSensor(NULL)
#endif
			, m_Reporter(NULL)
		{

		}


		Manager::~Manager()
		{
			if (m_bInited)
			{
				if (m_pTracker)
				{
					m_pTracker->StopTrack(); // 注意 ： 这里无论如何要尝试停下Sensor线程 ，否则下面会把Sensor对象删除掉，而Sensor线程可能还在跑，就会导致崩溃产生。
					delete m_pTracker;
				}
				if (m_pDistortion) delete m_pDistortion;
				if (m_pSensor) m_pSensor = NULL;
#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
   				if (m_pControlTracker) delete m_pControlTracker;
#endif
#ifdef MJ_OS_ANDROID
				if (m_pGlassSensor)
				{
					m_pGlassSensor->Release();
				}
#endif
				if (m_pDefaultSensor)
				{
					m_pDefaultSensor->Release();
				}
				if (m_pParameters) delete m_pParameters;
				if (m_Reporter)
				{
					m_Reporter->Release();
				}
				m_bInited = false;
			}
		}

		int Manager::Init()
		{
			m_bInited = true;
			if (m_bInited && m_pDistortion == NULL)
			{
				m_pDistortion = new Distortion();
			}
			if (m_bInited && m_pTracker == NULL)
				m_pTracker = new Tracker();
			if (m_bInited && m_pParameters == NULL)
			{
				m_pParameters = new Parameters();
				m_bInited = m_pParameters->Init();
			}
			if (m_bInited && m_Reporter == NULL)
			{
				m_Reporter = new DatabaseInfoReporter();
				m_bInited = m_Reporter->AppInit();
			}
#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
            if (m_bInited && m_pControlTracker == NULL)
            {
                m_pControlTracker = new ControllerTracker();
                m_bInited = m_pControlTracker->Init();
            }
#endif
#if defined(MJ_OS_ANDROID)
			if (m_bInited && m_pDefaultSensor == NULL)
				m_pDefaultSensor = new AndroidInternalSensor();
			if (m_bInited && m_pGlassSensor == NULL)
				m_pGlassSensor = new GlassSensor();
			if (m_bInited && m_pCheckSensor == NULL)
				m_pCheckSensor = new AndroidInternalSensorChecker();
#elif defined(MJ_OS_IOS)
            if (m_bInited && m_pSensor == NULL)
                m_pDefaultSensor = new IOSInternalSensor();
#elif defined(MJ_OS_WIN32)
			if (m_bInited && m_pSensor == NULL)
				m_pDefaultSensor = new WinInternalSensor();
#endif
            SetSensor();
			return m_bInited ? 0: 1;
		}

		Tracker* Manager::GetTracker(void)
		{
			return m_pTracker;
		}

		Distortion*	Manager::GetDistortion(void)
		{
			return m_pDistortion;
		}

		Parameters*	Manager::GetParameters(void)
		{
			return m_pParameters;
		}

		void Manager::SetSensor(const char* szGlassName)
		{

#ifdef MJ_OS_ANDROID
			if (szGlassName != NULL)
			{
				m_pSensor = m_pGlassSensor;
				m_pParameters->GetSensorParameters()->SetGlassName(szGlassName);
			}
			else
			{
				m_pSensor = m_pDefaultSensor;
				m_pParameters->GetSensorParameters()->SetGlassName("");
			}
#else
			m_pSensor = m_pDefaultSensor;
			m_pParameters->GetSensorParameters()->SetGlassName("");
#endif
		}

		Sensor*	Manager::GetSensor(void)
		{
			return m_pSensor;
		}
        
#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
        ControllerTracker*	Manager::GetControlTracker(void)
        {
            return m_pControlTracker;
        }
#endif
        
#ifdef MJ_OS_ANDROID
		Sensor*	Manager::GetCheckSensor(void)
		{
			return m_pCheckSensor;
		}

#endif
		DatabaseInfoReporter* Manager::GetReporter(void)
		{
			return m_Reporter;
		}
#if 1
		Manager g_Manager;

		Manager* Manager::GetMojingManager(void)
		{
			if (!g_Manager.m_bInited)
			{
				if (0 != g_Manager.Init())
					return NULL;
			}
			if (g_Manager.m_bInited)
				return &g_Manager;
			return NULL;
		}

#else
		Manager* g_pManager = NULL;

		Manager* Manager::GetMojingManager(void)
		{
			if (g_pManager == NULL )
			{
				g_pManager = new Manager;
				if (!g_pManager->Init())
				{
					delete g_pManager;
					g_pManager = NULL;
				}
			}
			
			return g_pManager;
		}
#endif
	}
}
