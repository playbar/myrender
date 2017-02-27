#pragma once
#include "GlassInfo.h"
namespace Baofeng
{
	namespace Mojing
	{
		class ManufacturerInfo :
			public MojingJsonElement
		{
		public:
			ManufacturerInfo();
			virtual ~ManufacturerInfo();
			virtual JSON* ToJson();
			JSON* ToJson(unsigned short wLanguageCode);
			virtual bool FromJson(JSON*);
			CLASS_INTERFACE_J(unsigned int, m_ui, ID);
			CLASS_MEMBER_STR_J(String, m_sz, Name);
			CLASS_MEMBER_STR_J(String, m_sz, URL);
			vector<unsigned int> m_AllProductID;
			DisplayInfo m_Display;
		};

	}
}