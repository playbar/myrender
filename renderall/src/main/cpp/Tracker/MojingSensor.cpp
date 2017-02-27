#include "MojingSensor.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_Sensorlogger;
#endif

namespace Baofeng
{
	namespace Mojing
	{
		SensorDataHandler::SensorDataHandler()
		: m_pSensor (NULL)
		{
		}

		SensorDataHandler::~SensorDataHandler()
		{
			RemoveHandlerFromSensor();
		}

		void SensorDataHandler::RemoveHandlerFromSensor()
		{
			if (m_pSensor != NULL)
			{
				m_pSensor->DetachHandler();
			}
		}


		Sensor::Sensor()
			: m_pHandler(NULL)
			, m_nSampleFrequence(100)
			, m_iMaxSampleRate(0)
		{
			//m_sRecordPath.Clear();
		}

		Sensor::~Sensor()
		{
			DetachHandler();
		}

		void Sensor::Initialize(Parameters* pParameters)
		{
			MOJING_FUNC_TRACE(g_Sensorlogger);
			m_pParameters = pParameters;
//			MojingFactoryCalibrationParameters* pFactoryCalibrationParam = m_pParameters->GetFactoryCalibrationParameters();
//			MojingGyroOffsetReportSet* pGyroOffsetReportSet = m_pParameters->GetGyroOffsetReportSet();
		}

		bool Sensor::StartSensor(int nSampleFrequence/*, const char *pRecordPath*/)
		{
			MOJING_FUNC_TRACE(g_Sensorlogger);
			m_nSampleFrequence = nSampleFrequence;
			//m_sRecordPath = pRecordPath;
			//if (pRecordPath)
			//{
			//	m_pLogFile = fopen(pRecordPath, "wt");
			//}
			return Start();
		}

		void Sensor::StopSensor()
		{
			//MOJING_FUNC_TRACE(g_Sensorlogger);
			SetExitFlag(true);
			int iTryCount = 0;
			while (!IsFinished() && iTryCount++ < 200)
			{
				Thread::MSleep(5);
			}
		}

		void Sensor::AttachHandler(SensorDataHandler* pHandler)
		{
			if (pHandler != NULL)
			{
				m_pHandler = pHandler;
				pHandler->m_pSensor = this;
			}
		}

		void Sensor::DetachHandler()
		{
			if (m_pHandler != NULL)
			{
				StopSensor();
				m_pHandler->m_pSensor = NULL;
				m_pHandler = NULL;
			}
		}

		void Sensor::GetFactoryCalibration(FactoryCalibration* pFactoryCalibration)
		{
			// TODO: Get those from Parameters
			//*AccelOffset = AccelCalibrationOffset;
			//*GyroOffset = GyroCalibrationOffset;
			//*AccelMatrix = AccelCalibrationMatrix;
			//*GyroMatrix = GyroCalibrationMatrix;
			//*Temperature = CalibrationTemperature;
		}

		bool Sensor::GetGyroOffsetReport(GyroOffsetReport* pReport)
		{
			// TODO: Get those from Parameters
			return false;
		}
	}
}
