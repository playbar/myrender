#include "MojingParameterBase.h"
#include "../Base/MojingTypes.h"
#include "../Profile/ProfileThreadMGR.h"
namespace Baofeng
{
	namespace Mojing
	{
		MojingParameterBase::MojingParameterBase()
		{
			SetClassName(__FUNCTION__);
			SetClassVersion(1);
		}


		MojingParameterBase::~MojingParameterBase()
		{
		}

		bool MojingParameterBase::Save()
		{
			return ProfileThreadMGR::UpdateProfile(GetClassName(), this, Profile_SAVE);
		}
		bool MojingParameterBase::Load()
		{
			return ProfileThreadMGR::UpdateProfile(GetClassName(), this, Profile_LOAD);
		}
	}//namespace Mojing
}//namespace Baofeng