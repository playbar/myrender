#pragma once

#ifdef MJ_OS_ANDROID
#include <jni.h>
#endif

namespace Baofeng
{
	namespace Mojing
	{
		class Distortion;
		class Tracker;
		class Parameters;
		class Sensor;
		class DatabaseInfoReporter;
#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
   		class ControllerTracker;
#endif

		class Manager
		{
			friend class Distortion;
			friend class Tracker;
			friend class MojingRenderBase;
			friend class MobileConfigProfile;
			friend class DatabaseInfoReporter;
#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
			friend class ControllerTracker;
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
#if defined(MJ_OS_ANDROID) || defined(MJ_OS_IOS)
			ControllerTracker* GetControlTracker(void);
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
#elif defined(MJ_OS_IOS)
            ControllerTracker* m_pControlTracker;
#endif
			DatabaseInfoReporter* m_Reporter;
		};
	}
}

