#ifndef MJ_SensorCalibration_h
#define MJ_SensorCalibration_h

#include "MojingSensor.h"
#include "MojingSensorFilter.h"
#include "MojingGyroTempCalibration.h"

namespace Baofeng
{
	namespace Mojing
	{


		class OffsetInterpolator
		{
		public:
			void Initialize(Array<Array<TemperatureReport> > const& temperatureReports, int coord);
			double GetOffset(double targetTemperature, double autoTemperature, double autoValue);

			Array<double> Temperatures;
			Array<double> Values;
		};

		class Sensor;
		class SensorCalibration : public NewOverrideBase
		{
		public:
			SensorCalibration(Sensor* pSensor, const char *lpszModel = NULL);
			virtual ~SensorCalibration(){};
			// Load data from the HW and perform the necessary preprocessing
			void Initialize(Parameters* pParameters);
			// Apply the calibration
			void Apply(MessageBodyFrame& msg);
			CLASS_MEMBER(float, m_f, CalibrationRate);
			
			//CLASS_MEMBER(int, m_i, CalibrationResetCount);
		protected:
			int m_nAutoSaveGap;
			float alpha/* = 0.4f*/;
			// 1.25f is a scaling factor related to conversion from per-axis comparison to length comparison
			float absLimit/* = 1.25 * 0.349066f*/;
			//  const float noiseLimit = 1.25f * 0.03f;		// This was the original threshold based on reported device noise characteristics.
			// 0.0175是弧度对应的角度
			//const float noiseLimit = 0.0175f;			// This is the updated threshold based on analyzing ten GearVR devices and determining a value that best
			float noiseLimit/* = 0.0175f*/;

			void StoreAutoOffset();
			void AutocalibrateGyro(MessageBodyFrame const& msg);

			void DebugPrintLocalTemperatureTable();
			void DebugClearHeadsetTemperatureReports();

			Sensor* pSensor;

			// Factory calibration data
			Matrix4f AccelMatrix, GyroMatrix;
			Vector3f AccelOffset;

			// Temperature based data
			Array<Array<TemperatureReport> > TemperatureReports;
			OffsetInterpolator Interpolators[3];

			// Autocalibration data
			SensorFilterf GyroFilter;
			Vector3f GyroAutoOffset;
			float GyroAutoTemperature;

			Parameters* m_pParameters;
			GyroTempCalibration GyroCalibration;
		};
	}
} // namespace OVR
#endif //MJ_SensorCalibration_h
