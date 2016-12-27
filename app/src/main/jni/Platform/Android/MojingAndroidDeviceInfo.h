#pragma once

#include <sys/system_properties.h>

class AndroidProperties
{
public:
	static int GetSerialNo(char *value)
	{
		return __system_property_get("ro.serialno", value);
	}

	static int GetBrand(char *value)
	{
		return __system_property_get("ro.product.brand", value);
	}

	static int GetID(char *value)
	{
		return __system_property_get("ro.build.display.id", value);
	}

	static int GetModel(char *value)
	{
		return __system_property_get("ro.product.model", value);
	}

	static int GetAndroidVersion(char *value)
	{
		return __system_property_get("ro.build.version.release", value);
	}

	static int GetIMEI(char *value)
	{
		if (__system_property_get("ro.ril.oem.imei", value) <= 0)
		{
			return __system_property_get("persist.radio.imei", value);
		}
		return 0;
	}

	static int GetOperator(int cardID, char *operatorName, int* operatorID)
	{
		char value[PROP_VALUE_MAX + 1];
		switch (cardID)
		{
		case 0:
			if (__system_property_get("gsm.operator.numeric", value) > 0)
				sscanf(value, "%d", operatorID);
			else
				operatorID = 0;
			return __system_property_get("gsm.operator.alpha", operatorName);

		case 1:
			if (__system_property_get("gsm.operator.numeric.1", value) > 0)
				sscanf(value, "%d", operatorID);
			else
				operatorID = 0;
			return __system_property_get("gsm.operator.alpha.1", operatorName);

		case 2:
			if (__system_property_get("gsm.operator.numeric.2", value) > 0)
				sscanf(value, "%d", operatorID);
			else
				operatorID = 0;
			return __system_property_get("gsm.operator.alpha.2", operatorName);
		}
		return 0;
	}
};