#pragma once

#ifdef MJ_OS_ANDROID
#include <jni.h>
#endif

//#define ENABLE_DRMHELPER

namespace Baofeng
{
	namespace Mojing
	{
		class Distortion;
		class Tracker;
		class Parameters;
		class Sensor;
		class DatabaseInfoReporter;
#ifdef MJ_OS_ANDROID
		class ControllerTracker;
#ifdef ENABLE_DRMHELPER
		class DRMHelper;
#endif
#endif

		class Manager
		{
			friend class Distortion;
			friend class Tracker;
			friend class MojingRenderBase;
			friend class MobileConfigProfile;
			friend class DatabaseInfoReporter;
#ifdef MJ_OS_ANDROID
			friend class ControllerTracker;
#ifdef ENABLE_DRMHELPER
			friend class DRMHelper;
#endif
#endif
		public:
			static Manager* GetMojingManager(void);
			
		public:
			Manager();
			virtual ~Manager();

			int Init();
			Tracker*	GetTracker(void);
			Distortion*	GetDistortion(void);
			Parameters*	GetParameters(void); 
			DatabaseInfoReporter* GetReporter(void);
#ifdef MJ_OS_ANDROID
			ControllerTracker* GetControlTracker(void);
#ifdef ENABLE_DRMHELPER
			DRMHelper* GetDRMHelper();
#endif
#endif
		protected:
			void		SetSensor(const char* szGlassName = NULL);
			Sensor*		GetSensor(void);
#ifdef MJ_OS_ANDROID
			Sensor*		GetCheckSensor();
#endif			
		private:
			bool		m_bInited;
			Distortion* m_pDistortion;
			Tracker*	m_pTracker;
			Parameters* m_pParameters;
			Sensor*		m_pSensor;
			Sensor*		m_pDefaultSensor;
#ifdef MJ_OS_ANDROID
			Sensor*		m_pGlassSensor;
			Sensor*		m_pCheckSensor;
			ControllerTracker* m_pControlTracker;
#ifdef ENABLE_DRMHELPER
			DRMHelper*	m_pDRMHelper;
#endif
#endif
			DatabaseInfoReporter* m_Reporter;
		};
	}
}

