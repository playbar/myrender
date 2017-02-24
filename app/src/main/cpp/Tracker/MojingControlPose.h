#pragma once
#ifdef MJ_OS_ANDROID
#include <android/sensor.h>
#endif
#include <semaphore.h>
#include "../Base/MojingMath.h"
#include "../Base/MojingString.h"
#include "../Base/MojingThreads.h"

#define MAX_CONTROL_NUM 	2

namespace Baofeng
{
	namespace Mojing
	{
		struct ControlSensorDataTargetInMap
		{
			float m_Orientation[4];
			float m_AngularAccel[3];
			float m_LinearAccel[3];
		};
		struct ControlSensorDataTarget
		{
			Quatf	m_Orientation;
			Vector3f m_AngularAccel;
			Vector3f m_LinearAccel;
			ControlSensorDataTarget & operator= (const ControlSensorDataTargetInMap& MMAP)
			{
				m_Orientation = Quatf(MMAP.m_Orientation[0], MMAP.m_Orientation[1], MMAP.m_Orientation[2], MMAP.m_Orientation[3]);
				m_Orientation.Normalized();
				m_AngularAccel = Vector3f(MMAP.m_AngularAccel[0], MMAP.m_AngularAccel[1], MMAP.m_AngularAccel[2]);
				m_LinearAccel = Vector3f(MMAP.m_LinearAccel[0], MMAP.m_LinearAccel[1], MMAP.m_LinearAccel[2]);
				return *this;
			}
		};

		struct MMapedControlSensorData
		{
			sem_t semlock;
			int cnt;
			double TimeS;
			ControlSensorDataTargetInMap CurrentData;// 实时更新
			ControlSensorDataTargetInMap FixData;// 初始为默认值，当手柄按下Reset按钮时更行
		};

		class ControlPose:public Thread
		{
		public:
			ControlPose(int cid);
			~ControlPose();
			bool StartTracker();
			void StopTracker();
			virtual int Run();
			int GetKeymask(int *pKeyMask);
			float GetControlCurrentPose(float *fOrientation, float *fAngularAccel, float *fLinearAccel);
			float GetControlFixPose(float *fOrientation, float *fAngularAccel, float *fLinearAccel);
		private:
			int m_cid;
			ControlSensorDataTarget m_CurrentDataInfo;
			ControlSensorDataTarget m_FixDataInfo;
			
			//Quatf m_fOrientation;
			//Vector3f m_fAngularAcceleration;
			//Vector3f m_fLinearAcceleration;
			float m_fCurrentSampleTime;
			float m_fFixSampleTime;
		};

		class ControllerTracker
		{
		public:
			ControllerTracker();
			~ControllerTracker();
			int Init();
			void Destroy();
			bool StartControlTracker(int cid);
			void StopControlTracker(int cid);
			int GetKeymask(int iID, int *pKeyMask);
			float GetControlCurrentPose(int cid, float *fOrientation, float *fAngularAccel, float *fLinearAccel);
			float GetControlFixPose(int cid, float *fOrientation, float *fAngularAccel, float *fLinearAccel);
		private:
			ControlPose* m_pControl[MAX_CONTROL_NUM];
		};

	}
}
