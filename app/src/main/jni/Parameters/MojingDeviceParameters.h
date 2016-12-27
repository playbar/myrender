#pragma once

#ifdef MJ_OS_ANDROID
#include <jni.h>
#endif
#include <vector>
#include "MojingJsonElement.h"

namespace Baofeng
{
	namespace Mojing
	{
// 		enum MACHINE_TYPE
// 		{
// 			MACHINE_TYPE_NONE = 0 ,
// 			MACHINE_TYPE_A1 ,
// 			MACHINE_TYPE_NIBIRU_3288
// 		};
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
//			CLASS_MEMBER_J(MACHINE_TYPE, m_, IsMachine);
			
		};

	}//namespace Mojing
}//namespace Baofeng