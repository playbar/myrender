#pragma once
#include "../../Base/MojingTypes.h"


class CDayDreamParameters
{
public:
	CDayDreamParameters();
	virtual ~CDayDreamParameters();

	static int UpdateDayDreamURL(const char* szDayDreamURL, char * szNewDayDreamURL , float fPPI_Scale);
};

