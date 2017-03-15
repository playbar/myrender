
#include <stdio.h>
#include "../Base/MojingTypes.h"

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
#else
        float getTemperature(void)
        {
  			return 40.0f;
        }
#endif
	}
}