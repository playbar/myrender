#pragma once

#ifdef MJ_OS_ANDROID
#include <jni.h>
#endif
#include <vector>
#include "MojingJsonElement.h"

#ifndef MojingDeviceParameters
#define DEVICE_ABILITY_NONE		0
#define DEVICE_ABILITY_SVR		1
#define DEVICE_ABILITY_GVR		2
#endif
namespace Baofeng
{
	namespace Mojing
	{
		struct MachineListNode
		{
			int	   m_iID;
			String m_szBrand;
			String m_szModel;
			int	   m_iRender; // 0 == default
		};
		class MojingDeviceParameters:public MojingJsonElement
		{
			std::vector<MachineListNode> m_MachineList;
			virtual int FindIsMachine(const  String szBrand, const String szModel) const;
		public:
			MojingDeviceParameters();
			virtual ~MojingDeviceParameters();
			
			virtual JSON* ToJson();
			virtual bool FromJson(JSON*);
			virtual void CheckIsMachine();
			static void AppendMachine(int iID, const  String szBrand, const String szModel, int iRender);
			MachineListNode GetCurrentMachine()const;
			CLASS_MEMBER_STR_J(String, m_s, Brand);
			CLASS_MEMBER_STR_J(String, m_s, Device);
			CLASS_MEMBER_STR_J(String, m_s, Display);
			CLASS_MEMBER_STR_J(String, m_s, Fingerprint);
			CLASS_MEMBER_STR_J(String, m_s, Hardware);
			CLASS_MEMBER_STR_J(String, m_s, Host);
			CLASS_MEMBER_STR_J(String, m_s, ID);
			CLASS_MEMBER_STR_J(String, m_s, Model);
			CLASS_MEMBER_STR_J(String, m_s, Product);
			CLASS_MEMBER_STR_J(String, m_s, Serial);
			CLASS_MEMBER_STR_J(String, m_s, Tags);
			CLASS_MEMBER_STR_J(String, m_s, Type);
			CLASS_MEMBER(bool, m_b, IsMachine);
			CLASS_MEMBER(bool, m_b, SensorDataFromJava);
			CLASS_MEMBER(int, m_i, Ability);
			
		};

	}//namespace Mojing
}//namespace Baofeng