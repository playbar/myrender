#include "MojingJsonElement.h"
#include "../Base/MojingTypes.h"
#include "../Profile/ProfileThreadMGR.h"

namespace Baofeng
{
	namespace Mojing
	{
		MojingJsonElement::MojingJsonElement()
		{
			SetClassName(__FUNCTION__);
			SetClassVersion(1);
		}


		MojingJsonElement::~MojingJsonElement()
		{
		}

		bool MojingJsonElement::Save()
		{
			return ProfileThreadMGR::UpdateProfile(GetClassName(), this, Profile_SAVE);
		}
		bool MojingJsonElement::Load()
		{
			return ProfileThreadMGR::UpdateProfile(GetClassName(), this, Profile_LOAD);
		}
	}//namespace Mojing
}//namespace Baofeng