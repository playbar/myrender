#pragma once

namespace Baofeng
{
	namespace Mojing
	{
		float getTemperature(void);
#ifdef MJ_OS_ANDROID
		String GetBatteryCapacity();
		String GetElectricCurrent();
		String GetCPUTemperature();
		String GetCPUFreq();
		String GetGPUTemperature();
		String GetGPUFreq();
#endif
	}
}
