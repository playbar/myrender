#pragma once
#ifdef MJ_OS_ANDROID
#include <sys/socket.h>
#include <arpa/inet.h>
#include <android/sensor.h>
#endif
#include <semaphore.h>
#include "../Base/MojingMath.h"
#include "../Base/MojingString.h"
//#include "../Base/MojingThreads.h"

#define MAX_CONTROLLER_NUM 	2

namespace Baofeng
{
	namespace Mojing
	{
#pragma  pack(push)
#pragma  pack(1)
		struct ControllerDataHeader
		{
			UByte  szTag[2];
			UByte  ubVersion;
			UByte  ubZero;
			UByte  ubJoyStickStatus[MAX_CONTROLLER_NUM];
		};

		struct ControlSensorDataInMap
		{
			float m_Orientation[4];
			float m_AngularAccel[3];
			float m_LinearAccel[3];
			UInt32 m_TimeInMs;
		};

		struct ControllerKeyData
		{
			UByte   Version;
			UByte	KeyMapID;
			UByte   TouchX;
			UByte	TouchY;
			UInt32  KeyStatus;
		};

        struct ExpandPackHeader
        {
            UByte Type;
            UByte Length;
            UByte Version;
            UByte Zero;
//            UByte AppendData[1] ;
        };

        struct FixFieldData
        {
            UByte bFlag;
            UByte bCal_status;
            UInt16 iCal_Score;
        };

        struct ExpandFixPack : public ExpandPackHeader
        {
            FixFieldData AppendData;
        };

		struct ControllerSensorData
		{
			ControllerKeyData	   KeyData;
			ControlSensorDataInMap CurrentData;// 实时更新
			ControlSensorDataInMap FixData;		// 初始为默认值，当手柄按下Reset按钮时更行
		};

		struct MMapedControllerData
		{
			ControllerDataHeader   m_Header;
			ControllerSensorData   m_JoyStick[MAX_CONTROLLER_NUM];
		};
#pragma  pack(pop)

		struct ControlSensorDataTarget
		{
			Quatf	m_Orientation;
			Vector3f m_AngularAccel;
			Vector3f m_LinearAccel;
			ControlSensorDataTarget & operator= (const ControlSensorDataInMap& MMAP)
			{
				m_Orientation = Quatf(MMAP.m_Orientation[0], MMAP.m_Orientation[1], MMAP.m_Orientation[2], MMAP.m_Orientation[3]);
				m_Orientation.Normalized();
				m_AngularAccel = Vector3f(MMAP.m_AngularAccel[0], MMAP.m_AngularAccel[1], MMAP.m_AngularAccel[2]);
				m_LinearAccel = Vector3f(MMAP.m_LinearAccel[0], MMAP.m_LinearAccel[1], MMAP.m_LinearAccel[2]);
				return *this;
			}
		};

		class ControllerInfo
		{
		public:
			ControllerInfo(int cid);
			~ControllerInfo();
			void SendControllerData(float* pControllerData, double dTimestamp, bool bRecenter);	
			void ParseMMapData(MMapedControllerData* pMMapData);
			int GetKeymask(int *pKeyMask);
			float GetControlCurrentPose(float *fOrientation, float *fAngularAccel, float *fLinearAccel);
			float GetControlFixPose(float *fOrientation, float *fAngularAccel, float *fLinearAccel);
            float GetControlFixCurrentPose(float *fOrientation, float *fAngularAccel, float *fLinearAccel);
		private:
			int m_cid;
			ControlSensorDataTarget m_CurrentDataInfo;
			ControlSensorDataTarget m_FixDataInfo;
			
			float m_fCurrentSampleTime;
			float m_fFixSampleTime;
			UByte m_Status;
		};

		class ControllerTracker
		{
		public:
			ControllerTracker();
			~ControllerTracker();
			int Init();
			void Destroy();
			//int StartTracker();
			//void StopTracker();
			//virtual int Run();
			void SendControllerData(float* pControllerData, double dTimestamp, bool bRecenter);
			bool ReadMMapData(const UByte* pMMapData, int dataLen);
            bool ParseExData(UByte* pData);
            void ParseFixExData(UByte* pData);
			int GetKeymask(int iID, int *pKeyMask);
            void GetFixScore(int* pStatus, int* pScore);
			float GetControlCurrentPose(int cid, float *fOrientation, float *fAngularAccel, float *fLinearAccel);
			float GetControlFixPose(int cid, float *fOrientation, float *fAngularAccel, float *fLinearAccel);
            float GetControlFixCurrentPose(int cid, float *fOrientation, float *fAngularAccel, float *fLinearAccel);
		private:
			//MMapedControllerData *m_pMMapData;
			ControllerInfo* m_pControl[MAX_CONTROLLER_NUM];

            int m_iFixScore[2];
            int m_iFixStatus[2];
		};

	}
}
