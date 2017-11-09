
#include <stdio.h>
#include <math.h>
#include "../Base/MojingTypes.h"
#include "../Base/MojingString.h"
#include "../Base/MojingAllocator.h"
#include "../Base/MojingMath.h"
#include "MojingTemperature.h"

namespace Baofeng
{
	namespace Mojing
	{
#ifdef MJ_OS_ANDROID
		char TemperaturePath[] = "/sys/class/thermal/thermal_zone0/temp";

		float getTemperature(void)
		{
			FILE* fp;
			int temp = 0;

			fp = fopen(TemperaturePath, "r");
			if (fp)
			{
				fscanf(fp, "%d", &temp);
				fclose(fp);

				if (temp > 100)
				{
					// 按照标准，应该是温度*1000
					return temp / 1000.0f;
				}
				else
				{
					// 没按照标准来，没有扩大1000倍
					return (float)temp;
				}
			}
			return -1.0f;
		}
        
        String ReadAllText(const char* path)
        {
            FILE* fp;
            String strRes = "";
            
            fp = fopen(path, "r");
            if (fp)
            {
                char buf[1024];
                fgets(buf, 1024, fp);
                size_t len = strlen(buf);
                if (buf[len - 1] == 0xA)
                    buf[len - 1] = 0;
                strRes = buf;
                fclose(fp);
            }
            
            return strRes;
        }
        
        String GetBatteryCapacity()
        {
            return ReadAllText("/sys/class/power_supply/battery/capacity");
        }
        
        String GetElectricCurrent()
        {
            return ReadAllText("/sys/class/power_supply/battery/current_now");
        }
        
        String GetCPUTemperature()
        {
            String ValueString0 = ReadAllText("/sys/devices/virtual/thermal/thermal_zone3/temp");
            String ValueString1 = ReadAllText("/sys/devices/virtual/thermal/thermal_zone9/temp");
            return ValueString0 + " | " + ValueString1;
        }
        
        String GetCPUFreq()
        {
            String strRet = "";
            
            for (int n = 0; n < 4; n++)
            {
                char path[128];
                sprintf(path, "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq", n);
                String ValueString = ReadAllText(path);
                char temp[64];
                sprintf(temp, "%f", atoi(ValueString.ToCStr()) / 1e6f);
                //double dFreq = round(atoi(ValueString.ToCStr()) / 1e6f);
                if (strRet.GetLength() > 0)
                    strRet += " | ";
                
                strRet += temp;
            }
            
            return strRet;
        }
        
        String GetGPUTemperature()
        {
            return ReadAllText("/sys/devices/virtual/thermal/thermal_zone16/temp");
        }
        
        String GetGPUFreq()
        {
            String strRet = "";
            String ValueString = ReadAllText("/sys/class/kgsl/kgsl-3d0/gpuclk");
            //double dFreq = round(atoi(ValueString.ToCStr()) / 1e6f);
            char temp[64];
            sprintf(temp, "%f", atoi(ValueString.ToCStr()) / 1e6f);
            strRet += temp;
            return strRet;
        }
#else
        float getTemperature(void)
        {
            return 40.0f;
        }
#endif
	}
}
