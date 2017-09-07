#pragma once
#include "../Base/MojingTypes.h"
#include "../Base/MojingString.h"

namespace Baofeng
{
	namespace Mojing
	{
		class MojingProfileKey
		{
			void ToBuffer(unsigned char[16])const;
			void FromBuffer(unsigned char[16]);
		public:
			MojingProfileKey();
			virtual ~MojingProfileKey();
			void Reset();
			String GetString()const;
			bool SetString(String strKey);
			void UnionKey(const MojingProfileKey OtherKey);
			const char* GetGlassKeyIDString()const;
			static const char* GetGlassKeyIDString(const char* szKey);
			static const char* GetStringWithID(unsigned int uiManufacturerID, unsigned int uiProductID, unsigned int uiGlassID);

			CLASS_MEMBER(unsigned int, m_ui, ManufacturerID);
			CLASS_MEMBER(unsigned int, m_ui, ProductID);
			CLASS_MEMBER(unsigned int, m_ui, GlassID);
			CLASS_MEMBER(unsigned char, m_ui, PlatformID);
			CLASS_MEMBER(unsigned int, m_ui, AppID);
		};
	}
}

