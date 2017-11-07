#pragma once

#include "../Parameters/MojingJsonElement.h"
#include <map>
#include "../Base/MojingTypes.h"
using namespace std;

namespace Baofeng
{
	namespace Mojing
	{
#ifdef MJ_OS_ANDROID
        class UnrealJoystickProfile
        {
        public:
            UnrealJoystickProfile();
            virtual ~UnrealJoystickProfile();
            bool getDeviceMap(std::map<String, int> & devmap);

            CLASS_INTERFACE_STR(String, m_sz, ProfilePathName);
            CLASS_INTERFACE_STR_J(String, m_sz, ClassName);
            CLASS_INTERFACE_J(unsigned int, m_ui, ReleaseDate);// 最新版本文件的发布日期
        private:
            int GetMojingType(String & name);
        };
#endif

	}
}