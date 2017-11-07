#include "MojingSensorCalibration.h"
#include "../Base/MojingLog.h"
#include "../Base/MojingThreads.h"
#include "../Base/MojingAlg.h"

#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDeviceParameters.h"
#include <time.h>
#include "MojingSensor.h"
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
extern MojingLogger g_Sensorlogger;
#endif
#ifdef _DEBUG
#define CALIBRATION_STAUES_DEBUG 0
#else
#define CALIBRATION_STAUES_DEBUG 0
#endif
namespace Baofeng
{
	namespace Mojing
	{
//		using namespace Alg;

		const UByte VERSION = 2;
		const UByte MAX_COMPAT_VERSION = 15;

		SensorCalibration::SensorCalibration(Sensor* pSensor, const char *lpszModel)
			: GyroFilter(1000)
			, GyroAutoTemperature(0)
			, m_nAutoSaveGap(0)
			, m_fCalibrationRate(0)
			//, m_iCalibrationResetCount(0)
			, alpha(0.4f)
			, absLimit(1.25 * 0.349066f)
			, noiseLimit(0.0175f)
		{
			if (lpszModel)
			{
				if (0 == strcmp(lpszModel, "MX4 Pro") || 
					0 == strcmp(lpszModel, "MI NOTE Pro"))
				{
					absLimit = 2 * 0.349066f;
					noiseLimit = 2.5 * 0.0175f;
				}
				else if (0 == strcmp(lpszModel, "KE-01"))
				{
#define KE_01_SENSOR_FILTER 0.2f
					absLimit *= KE_01_SENSOR_FILTER;
					noiseLimit *= KE_01_SENSOR_FILTER;
				}
			}
			this->pSensor = pSensor;
		};

		void SensorCalibration::Initialize(Parameters* pParameters)
		{
			m_pParameters = pParameters;
			GyroCalibration.Initialize(pParameters);

			// read factory calibration
			FactoryCalibration theFactoryCalibration;
			MojingFactoryCalibrationParameters* pFactoryCalibration = pParameters->GetFactoryCalibrationParameters();
			AccelOffset = pFactoryCalibration->GetAccelOffset();
			GyroAutoOffset = pFactoryCalibration->GetGyroOffset();
			AccelMatrix = pFactoryCalibration->GetAccelMatrix();
			GyroMatrix = pFactoryCalibration->GetGyroMatrix();
			GyroAutoTemperature = pFactoryCalibration->GetTemperature();

			/*
				LogText("LDC - SensorCalibration - Factory calibration GyroMatrix: %f %f %f %f, %f %f %f %f, %f %f %f %f, %f %f %f %f\n",	GyroMatrix.M[0][0], GyroMatrix.M[0][1], GyroMatrix.M[0][2], GyroMatrix.M[0][3],
				GyroMatrix.M[1][0], GyroMatrix.M[1][1], GyroMatrix.M[1][2], GyroMatrix.M[1][3],
				GyroMatrix.M[2][0], GyroMatrix.M[2][1], GyroMatrix.M[2][2], GyroMatrix.M[2][3],
				GyroMatrix.M[3][0], GyroMatrix.M[3][1], GyroMatrix.M[3][2], GyroMatrix.M[3][3]);

				LogText("LDC - SensorCalibration - Factory calibration GyroAutoOffset: %f %f %f temp=%f \n", GyroAutoOffset.x, GyroAutoOffset.y, GyroAutoOffset.z, GyroAutoTemperature);
				*/

			// if the headset has an autocalibrated offset, prefer it over the factory defaults
			GyroOffsetReport gyroReport;
			if (pSensor->GetGyroOffsetReport(&gyroReport))
			{
				GyroAutoOffset = (Vector3f)gyroReport.Offset;
				GyroAutoTemperature = (float)gyroReport.Temperature;
			}

			// read the temperature tables and prepare the interpolation structures
			GyroCalibration.GetAllTemperatureReports(&TemperatureReports);

			// MJ_ASSERT(result);
			for (int i = 0; i < 3; i++)
				Interpolators[i].Initialize(TemperatureReports, i);

		}

		void SensorCalibration::DebugPrintLocalTemperatureTable()
		{
			LogText("TemperatureReports:\n");
			for (int i = 0; i < (int)TemperatureReports.GetSize(); i++)
			{
				for (int j = 0; j < (int)TemperatureReports[i].GetSize(); j++)
				{
					TemperatureReport& tr = TemperatureReports[i][j];

					LogText("SensorCalibration - [%d][%d]: Version=%3d, Bin=%d/%d, "
						"Sample=%d/%d, TargetTemp=%3.1lf, "
						"ActualTemp=%4.1lf, "
						"Offset=(%7.2lf, %7.2lf, %7.2lf), "
						"Time=%d\n", i, j, tr.Version,
						tr.Bin, tr.NumBins,
						tr.Sample, tr.NumSamples,
						tr.TargetTemperature,
						tr.ActualTemperature,
						tr.Offset.x, tr.Offset.y, tr.Offset.z,
						tr.Time);
				}
			}
		}

		void SensorCalibration::DebugClearHeadsetTemperatureReports()
		{
			MJ_ASSERT(pSensor != NULL);

			Array<Array<TemperatureReport> > temperatureReports;

			GyroCalibration.GetAllTemperatureReports(&temperatureReports);

			MJ_ASSERT(temperatureReports.GetSize() > 0);
			MJ_ASSERT(temperatureReports[0].GetSize() > 0);

			TemperatureReport& tr = TemperatureReports[0][0];

			tr.ActualTemperature = 0.0;
			tr.Time = 0;
			tr.Version = 0;
			tr.Offset.x = tr.Offset.y = tr.Offset.z = 0.0;

			for (UByte i = 0; i < tr.NumBins; i++)
			{
				tr.Bin = i;

				for (UByte j = 0; j < tr.NumSamples; j++)
				{
					tr.Sample = j;

					GyroCalibration.SetTemperatureReport(tr);

					// Need to wait for the tracker board to finish writing to eeprom.
					// Thread::MSleep(50);
				}
			}
		}

		void SensorCalibration::Apply(MessageBodyFrame& msg)
		{
			AutocalibrateGyro(msg);

			// compute the interpolated offset
			Vector3f gyroOffset;
			for (int i = 0; i < 3; i++)
				gyroOffset[i] = (float)Interpolators[i].GetOffset(msg.Temperature, GyroAutoTemperature, GyroAutoOffset[i]);

			//MOJING_TRACE(g_Sensorlogger, "Gyro Offset (" << gyroOffset.x << ", " << gyroOffset.y<< ", " << gyroOffset.z << ")");

			// apply calibration
			msg.RotationRate = GyroMatrix.Transform(msg.RotationRate - gyroOffset);
			msg.Acceleration = AccelMatrix.Transform(msg.Acceleration - AccelOffset);
		}

		void SensorCalibration::AutocalibrateGyro(MessageBodyFrame const& msg)
		{
			Vector3f gyro = msg.RotationRate;
			// do a moving average to reject short term noise
			Vector3f avg = (GyroFilter.IsEmpty()) ? gyro : gyro * alpha + GyroFilter.PeekBack() * (1 - alpha);

			// Make sure the absolute value is below what is likely motion
			// Make sure it is close enough to the current average that it is probably noise and not motion
			float fAvg = avg.Length();
			float fFilterOffset = (avg - GyroFilter.Mean()).Length();
#ifdef CALIBRATION_STAUES_DEBUG
			static enum{
				CALIBRATION_NONE,
				CALIBRATION_START,
				CALIBRATION_CANCEL,
				CALIBRATION_HALF,
				CALIBRATION_DONE,
				CALIBRATION_UNKNOWN
			} sCalibration = CALIBRATION_UNKNOWN;

#endif
			if (fAvg >= absLimit || fFilterOffset >= noiseLimit)
			{
				//SetCalibrationResetCount(m_iCalibrationResetCount + 1);
#ifdef CALIBRATION_STAUES_DEBUG
				if (sCalibration != CALIBRATION_CANCEL)
				{
					sCalibration = CALIBRATION_CANCEL;
					MOJING_TRACE(g_Sensorlogger , "CALIBRATION_CANCEL");
				}
#endif
				GyroFilter.Clear();
				m_nAutoSaveGap = 0;
			}
#ifdef CALIBRATION_STAUES_DEBUG
			else if (sCalibration != CALIBRATION_START && sCalibration != CALIBRATION_HALF && sCalibration != CALIBRATION_DONE)
			{
				sCalibration = CALIBRATION_START;
				MOJING_TRACE(g_Sensorlogger, "CALIBRATION_START");
			}
#endif	

			GyroFilter.PushBack(avg);

			if (GyroFilter.GetCapacity() > 2)
			{
				float fRate = GyroFilter.GetSize();
				fRate /= (GyroFilter.GetCapacity() / 2);
				SetCalibrationRate(fRate);
			}
			else
			{
				SetCalibrationRate(0);
			}
			// if had a reasonable number of samples already use it for the current offset
			if (GyroFilter.GetSize() > GyroFilter.GetCapacity() / 2)
			{
				// MOJING_TRACE(g_APIlogger, "GyroFilter.GetSize = " << GyroFilter.GetSize() << " / " << GyroFilter.GetCapacity());
#ifdef CALIBRATION_STAUES_DEBUG
				if (sCalibration != CALIBRATION_HALF && sCalibration != CALIBRATION_DONE)
				{
					sCalibration = CALIBRATION_HALF;
					MOJING_TRACE(g_Sensorlogger, "CALIBRATION_HALF");
				}
#endif

				GyroAutoOffset = GyroFilter.Mean();
				GyroAutoTemperature = msg.Temperature;

#ifdef ENABLE_SENSOR_LOGGER				
				MOJING_TRACE(g_Sensorlogger, "GyroAutoOffset (" << 
					GyroAutoOffset.x << ", " << GyroAutoOffset.y << ", " << GyroAutoOffset.z << ", " <<
					"at temperature " << GyroAutoTemperature);
#endif
				// 虽然没有存盘，但是认为目前收集的数据已经足够矫正用了
				m_pParameters->GetFactoryCalibrationParameters()->SetCalibrated(true);
				// After ~6 seconds of no motion, use the average as the new zero rate offset
				if (GyroFilter.IsFull())
				{
					// 增加发送时间间隔保护，超过10分钟才会发下一次
#ifdef CALIBRATION_STAUES_DEBUG
					if (sCalibration != CALIBRATION_DONE)
					{
						sCalibration = CALIBRATION_DONE;
						MOJING_TRACE(g_Sensorlogger, "CALIBRATION_DONE");
					}
#endif					
					static unsigned int uiLastSendTime = 0;
					if (m_nAutoSaveGap == 0 &&
						(unsigned int)Baofeng::Mojing::Timer::GetSeconds() - uiLastSendTime >= 60 * 15)
					{
						StoreAutoOffset();
						MojingFactoryCalibrationParameters* pFactoryCalibration = m_pParameters->GetFactoryCalibrationParameters();
						pFactoryCalibration->SetGyroOffset(GyroAutoOffset);
						pFactoryCalibration->SetTemperature(GyroAutoTemperature);
						pFactoryCalibration->Save();
						m_nAutoSaveGap++;
						uiLastSendTime = (unsigned int)Baofeng::Mojing::Timer::GetSeconds();
					}
					else
					{
						m_nAutoSaveGap++;
						if (m_nAutoSaveGap > 10)
						{
							m_nAutoSaveGap = 0;
						}
					}
				}
			}
		}

		void SensorCalibration::StoreAutoOffset()
		{
			const double maxDeltaT = 2.5;
			const double minExtraDeltaT = 0.5;
			const UInt32 minDelay = 24 * 3600; // 1 day in seconds

			// find the best bin
			UPInt binIdx = 0;
			for (UPInt i = 1; i < TemperatureReports.GetSize(); i++)
			if (Alg::Abs(GyroAutoTemperature - TemperatureReports[i][0].TargetTemperature) <
				Alg::Abs(GyroAutoTemperature - TemperatureReports[binIdx][0].TargetTemperature))
				binIdx = i;

			// find the oldest and newest samples
			// NB: uninitialized samples have Time == 0, so they will get picked as the oldest
			UPInt newestIdx = 0, oldestIdx = 0;
			for (UPInt i = 1; i < TemperatureReports[binIdx].GetSize(); i++)
			{
				// if the version is newer - do nothing
				if (TemperatureReports[binIdx][i].Version > VERSION)
					return;
				if (TemperatureReports[binIdx][i].Time > TemperatureReports[binIdx][newestIdx].Time)
					newestIdx = i;
				if (TemperatureReports[binIdx][i].Time < TemperatureReports[binIdx][oldestIdx].Time)
					oldestIdx = i;
			}
			TemperatureReport& oldestReport = TemperatureReports[binIdx][oldestIdx];
			TemperatureReport& newestReport = TemperatureReports[binIdx][newestIdx];
			MJ_ASSERT((oldestReport.Sample == 0 && newestReport.Sample == 0 && newestReport.Version == 0) ||
				oldestReport.Sample == (newestReport.Sample + 1) % newestReport.NumSamples);

			UInt32 now = (UInt32)time(0);
//			MOJING_TRACE(g_APIlogger, "StoreAutoOffset - 1");

			if (now - newestReport.Time > minDelay)
			{
//				MOJING_TRACE(g_APIlogger, "StoreAutoOffset - 2");
				// only write a new sample if the temperature is close enough
				if (Alg::Abs(GyroAutoTemperature - oldestReport.TargetTemperature) < maxDeltaT)
				{
//					MOJING_TRACE(g_APIlogger, "StoreAutoOffset - 3");
					oldestReport.Time = now;
					oldestReport.ActualTemperature = GyroAutoTemperature;
					oldestReport.Offset = (Vector3d)GyroAutoOffset;
					oldestReport.Version = VERSION;
					MOJING_TRACE(g_APIlogger, "Save Sensor at Temperature " << GyroAutoTemperature << " , Target " << oldestReport.TargetTemperature);
					GyroCalibration.SetTemperatureReport(oldestReport);
				}
			}
			else
			{
//				MOJING_TRACE(g_APIlogger, "StoreAutoOffset - 4");
				// if the newest sample is too recent - _update_ it if significantly closer to the target temp
				if (Alg::Abs(GyroAutoTemperature - newestReport.TargetTemperature) + minExtraDeltaT
					< Alg::Abs(newestReport.ActualTemperature - newestReport.TargetTemperature))
				{
//					MOJING_TRACE(g_APIlogger, "StoreAutoOffset - 5");
					// (do not update the time!)
					newestReport.ActualTemperature = GyroAutoTemperature;
					newestReport.Offset = (Vector3d)GyroAutoOffset;
					newestReport.Version = VERSION;
					MOJING_TRACE(g_APIlogger, "Save Sensor at Temperature " << GyroAutoTemperature << " , Target " << newestReport.TargetTemperature);
					GyroCalibration.SetTemperatureReport(newestReport);
				}
			}

			// update the interpolators with the new data
			// this is not particularly expensive call and would only happen rarely
			// but if performance is a problem, it's possible to only recompute the data that has changed
			for (int i = 0; i < 3; i++)
				Interpolators[i].Initialize(TemperatureReports, i);
#ifdef ENABLE_SENSOR_LOGGER
			MOJING_TRACE(g_Sensorlogger, "GyroAutoOffset has new data," << GyroAutoOffset.x << "," << GyroAutoOffset.y << "," <<
				GyroAutoOffset.z << "," << "at temp," << GyroAutoTemperature << ", ");
#endif
		}

		const TemperatureReport& median(const Array<TemperatureReport>& temperatureReportsBin, int coord)
		{
			Array<double> values;
			values.Reserve(temperatureReportsBin.GetSize());
			for (unsigned i = 0; i < temperatureReportsBin.GetSize(); i++)
			if (temperatureReportsBin[i].ActualTemperature != 0)
				values.PushBack(temperatureReportsBin[i].Offset[coord]);
			if (values.GetSize() > 0)
			{
				double med = Alg::Median(values);
				// this is kind of a hack
				for (unsigned i = 0; i < temperatureReportsBin.GetSize(); i++)
				if (temperatureReportsBin[i].Offset[coord] == med)
					return temperatureReportsBin[i];
				// if we haven't found the median in the original array, something is wrong
				MJ_DEBUG_BREAK;
			}
			return temperatureReportsBin[0];
		}

		void OffsetInterpolator::Initialize(Array<Array<TemperatureReport> > const& temperatureReports, int coord)
		{
			int bins = (int)temperatureReports.GetSize();
			Temperatures.Clear();
			Temperatures.Reserve(bins);
			Values.Clear();
			Values.Reserve(bins);

			for (int bin = 0; bin < bins; bin++)
			{
				MJ_ASSERT(temperatureReports[bin].GetSize() == temperatureReports[0].GetSize());
				const TemperatureReport& report = median(temperatureReports[bin], coord);
				if (report.Version > 0 && report.Version <= MAX_COMPAT_VERSION)
				{
					Temperatures.PushBack(report.ActualTemperature);
					Values.PushBack(report.Offset[coord]);
				}
			}
		}

		double OffsetInterpolator::GetOffset(double targetTemperature, double autoTemperature, double autoValue)
		{

			const double autoRangeExtra = 1.0;
			const double minInterpolationDist = 0.5;

			// difference between current and autocalibrated temperature adjusted for preference over historical data
			const double adjustedDeltaT = Alg::Abs(autoTemperature - targetTemperature) - autoRangeExtra;

			int count = (int)Temperatures.GetSize();
			// handle special cases when we don't have enough data for proper interpolation
			if (count == 0)
				return autoValue;
			if (count == 1)
			{
				if (adjustedDeltaT < Alg::Abs(Temperatures[0] - targetTemperature))
					return autoValue;
				else
					return Values[0];
			}

			// first, find the interval that contains targetTemperature
			// if all points are on the same side of targetTemperature, find the adjacent interval
			int l;
			if (targetTemperature < Temperatures[1])
				l = 0;
			else if (targetTemperature >= Temperatures[count - 2])
				l = count - 2;
			else
			for (l = 1; l < count - 2; l++)
			if (Temperatures[l] <= targetTemperature && targetTemperature < Temperatures[l + 1])
				break;
			int u = l + 1;

			// extend the interval if it's too small and the interpolation is unreliable
			if (Temperatures[u] - Temperatures[l] < minInterpolationDist)
			{
				if (l > 0
					&& (u == count - 1 || Temperatures[u] - Temperatures[l - 1] < Temperatures[u + 1] - Temperatures[l]))
					l--;
				else if (u < count - 1)
					u++;
			}

			// verify correctness
			MJ_ASSERT(l >= 0 && u < count);
			MJ_ASSERT(l == 0 || Temperatures[l] <= targetTemperature);
			MJ_ASSERT(u == count - 1 || targetTemperature < Temperatures[u]);
			MJ_ASSERT((l == 0 && u == count - 1) || Temperatures[u] - Temperatures[l] > minInterpolationDist);
			MJ_ASSERT(Temperatures[l] <= Temperatures[u]);

			// perform the interpolation
			double slope;
			if (Temperatures[u] - Temperatures[l] >= minInterpolationDist)
				slope = (Values[u] - Values[l]) / (Temperatures[u] - Temperatures[l]);
			else
				// avoid a badly conditioned problem
				slope = 0;
			if (adjustedDeltaT < Alg::Abs(Temperatures[u] - targetTemperature))
				// use the autocalibrated value, if it's close
				return autoValue + slope * (targetTemperature - autoTemperature);
			else
				return Values[u] + slope * (targetTemperature - Temperatures[u]);
		}
	}
}
