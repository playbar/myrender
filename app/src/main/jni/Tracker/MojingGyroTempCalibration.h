

#ifndef MJ_GyroTempCalibration_h
#define MJ_GyroTempCalibration_h

//#include "MJ_Device.h"
#include "../Base/MojingArray.h"
#include "../Base/MojingString.h"
#include "../Base/MojingMath.h"
#include "../Parameters/MojingParameters.h"


namespace Baofeng
{
	namespace Mojing
	{
		// Temperature calibration result.
		struct TemperatureReport
		{
			TemperatureReport()
			: CommandId(0), Version(0),
			NumBins(0), Bin(0), NumSamples(0), Sample(0),
			TargetTemperature(0), ActualTemperature(0),
			Time(0), Offset(0)
			{}

			TemperatureReport(UInt16 commandId,
				UByte  version,
				UByte  numBins,
				UByte  bin,
				UByte  numSamples,
				UByte  sample,
				double targetTemperature,
				double actualTemperature,
				UInt32 time,
				Vector3d offset)
				: CommandId(commandId), Version(version),
				NumBins(numBins), Bin(bin), NumSamples(numSamples), Sample(sample),
				TargetTemperature(targetTemperature), ActualTemperature(actualTemperature),
				Time(time), Offset(offset)
			{ }

			UInt16      CommandId;
			UByte	    Version;          // See 'DK2 Firmware Specification' document for a description of
			UByte	    NumBins;          // temperature calibration data.
			UByte	    Bin;
			UByte	    NumSamples;
			UByte	    Sample;
			double	    TargetTemperature;
			double	    ActualTemperature;
			UInt32      Time;             // Better hope nobody tries to use this in 2038
			Vector3d    Offset;
		};

		class GyroTempCalibration
		{
		public:
			GyroTempCalibration();

			void Initialize(Parameters* pParameters);

			void GetAllTemperatureReports(Array<Array<TemperatureReport> >* tempReports);
			void SetTemperatureReport(const TemperatureReport& tempReport);

		private:
			double m_dLastPostTime;
			//enum { GyroCalibrationNumBins = 7 };
			//enum { GyroCalibrationNumSamples = 5 };

			struct GyroCalibrationEntry
			{
				UInt32		Version;
				double	    ActualTemperature;
				UInt32      Time;
				Vector3d    Offset;
			};
			//String GetBaseOVRPath(bool create_dir);
			//String GetCalibrationPath(bool create_dir);
			//void TokenizeString(Array<String>* tokens, const String& str, char separator);
			//void GyroCalibrationFromString(const String& str);
			//String GyroCalibrationToString();
			void GetTemperatureReport(int binIndex, int sampleIndex, TemperatureReport* tempReport);

			//void LoadFile();
			//void SaveFile();

			//String DeviceSerialNumber;
			//GyroCalibrationEntry GyroCalibration[GyroCalibrationNumBins][GyroCalibrationNumSamples];

		private:
			Parameters* m_pParameters;
		};
	}
} // namespace OVR

#endif // MJ_GyroTempCalibration_h
