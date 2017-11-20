#include "MojingDeviceParameters.h"
#include <stdio.h>
#include "../Base/MojingTypes.h"
#include "../MojingManager.h"
#include "../Parameters/MojingParameters.h"
#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif


#ifdef MJ_OS_ANDROID
#include "../3rdPart/Qualcomm/CSVRApi.h"
#endif
namespace Baofeng
{
	namespace Mojing
	{
// 		void f()
// 		{
// 			float fFOV;
// 			Vector4f gl_Position;// Position in near
// 			float fr = 0.5 / sin(fFOV / 180 / 2 * PI);
// 			float fT2 = fr * fr / (gl_Position.x * gl_Position.x + gl_Position.y * gl_Position.y + gl_Position.z * gl_Position.z);
// 			float fT = sqrt(fT2);
// 			gl_Position *= fT;
// 			gl_Position.w = 1;
// 		}






		MojingDeviceParameters::MojingDeviceParameters():
			m_iAbility(DEVICE_ABILITY_NONE)
		{
			SetClassName(__FUNCTION__);
			m_bIsMachine = false;
			m_bSensorDataFromJava = false;
			if (1)
			{
#ifdef MJ_OS_ANDROID
				// 注意：因为SVR的代码里面有硬代码exit(1)，所以不能直接调用初始化测试硬件能力
				// 需要先检查是否是存在以下文件
				
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "Check qvrservice....");
#endif
				
				if (-1 != access("/vendor/bin/qvrservice", 0) &&
					0 == (m_iAbility & DEVICE_ABILITY_SVR))
				{// 程序执行到这里表示疑似存在qvrservice，下面检查可靠性
#ifdef _DEBUG
					MOJING_TRACE(g_APIlogger, "Qvrservice exist, try init....");
#endif
					CSVRApi svrApi;
					if (svrApi.Init() && svrApi.CheckServiceIsAvaliable())
					{
						MOJING_TRACE(g_APIlogger, "Qvrservice exist and working , + DEVICE_ABILITY_SVR");
						m_iAbility |= DEVICE_ABILITY_SVR;
					}
					else
					{
						MOJING_TRACE(g_APIlogger, "Qvrservice exist, but not working with current version....");
					}
				}
				else
				{
#ifdef _DEBUG
					MOJING_TRACE(g_APIlogger, "Qvrservice not exist....");
#endif
				}
#endif // MJ_OS_ANDROID
				
			}
		}


		MojingDeviceParameters::~MojingDeviceParameters()
		{
		}
		JSON* MojingDeviceParameters::ToJson()
		{
			JSON* pRet = JSON::CreateObject();
			ClassNameToJson(pRet);
			ClassVersionToJson(pRet);
			BrandToJson(pRet);
			DeviceToJson(pRet);
			DisplayToJson(pRet);
			FingerprintToJson(pRet);
			HardwareToJson(pRet);
			HostToJson(pRet);
			IDToJson(pRet);
			ModelToJson(pRet);
			ProductToJson(pRet);
			SerialToJson(pRet);
			TagsToJson(pRet);
			TypeToJson(pRet);

			return pRet;
		}

		bool MojingDeviceParameters::FromJson(JSON*pJsonItem)
		{
			bool bRet = false;
			JSON *pJson = pJsonItem->GetItemByName("DeviceParameterss");
			if (pJson)
			{
				BrandFromJson(pJson);
				DeviceFromJson(pJson);
				DisplayFromJson(pJson);
				FingerprintFromJson(pJson);
				HardwareFromJson(pJson);
				HostFromJson(pJson);
				IDFromJson(pJson);
				ModelFromJson(pJson);
				ProductFromJson(pJson);
				SerialFromJson(pJson);
				TagsFromJson(pJson);
				TypeFromJson(pJson);
			}
			return bRet;
		}

		void MojingDeviceParameters::CheckIsMachine()
		{
			if (FindIsMachine(m_sBrand, m_sModel) >= 0)
				m_bIsMachine = true;			
		}

		int MojingDeviceParameters::FindIsMachine(const  String szBrand, const String szModel) const
		{
			int iRet = -1;
			for (uint32_t i = 0; i < m_MachineList.size(); i++)
			{
				if (m_MachineList[i].m_szBrand == szBrand && m_MachineList[i].m_szModel == szModel)
				{
					iRet = i;
					break;
				}
			}
			return iRet;
		}
		void MojingDeviceParameters::AppendMachine(int iID, const  String szBrand, const String szModel, int iRender)
		{
			MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
			if (0 > pDeviceParameters->FindIsMachine(szBrand, szModel))
			{
				MachineListNode NewItem;
				NewItem.m_iID = iID;
				NewItem.m_szBrand = szBrand;
				NewItem.m_szModel = szModel;
				NewItem.m_iRender = iRender;
				pDeviceParameters->m_MachineList.push_back(NewItem);

				pDeviceParameters->CheckIsMachine();
			}
		}

		MachineListNode MojingDeviceParameters::GetCurrentMachine()const
		{
			MachineListNode Ret;
			Ret.m_iID = Ret.m_iRender = 0;
			int iIndex = FindIsMachine(m_sBrand, m_sModel);
			if (iIndex >= 0)
				Ret = m_MachineList[iIndex];
			return Ret;
		}
	}//namespace Mojing
}//namespace Baofeng