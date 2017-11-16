#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <sys/stat.h>
#include <android/log.h>
#include "../Base/MojingMath.h"
#include "../Base/MojingTimer.h"
#include "../Base/MojingLog.h"
#include "../Base/MojingThreads.h"
#include "../MojingManager.h"
#include "../MojingSDKStatus.h"
#include "MojingControlPose.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

namespace Baofeng
{
	namespace Mojing
	{
		static int sem_timedwait_millsecs(sem_t *sem, long msecs)
		{
			struct timespec ts;
			clock_gettime(CLOCK_REALTIME, &ts);
			long secs = msecs / 1000;
			msecs = msecs % 1000;

			long add = 0;
			msecs = msecs * 1000 * 1000 + ts.tv_nsec;
			add = msecs / (1000 * 1000 * 1000);
			ts.tv_sec += (add + secs);
			ts.tv_nsec = msecs % (1000 * 1000 * 1000);

			return sem_timedwait(sem, &ts);
		}

		ControlPose::ControlPose(int cid)
		{
			m_fCurrentSampleTime = m_fFixSampleTime = 0;
			m_cid = cid;
		}

		ControlPose::~ControlPose()
		{

		}

		bool ControlPose::StartTracker()
		{
			//MOJING_FUNC_TRACE(g_APIlogger);	
			return Thread::Start();
		}

		void ControlPose::StopTracker()
		{
			Thread::SetExitFlag(true);
		}

		int ControlPose::Run()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			char buf[64];
			sprintf(buf, "ControlPose %d", m_cid);
			SetThreadName(buf);
	
			// mmap
			int mmap_fd;
			MMapedControlSensorData *mmap_ptr = NULL;

			while (!GetExitFlag()) {

				if (mmap_ptr == NULL)
				{
					mkdir("/sdcard/MojingSDK", 0777);
					mkdir("/sdcard/MojingSDK/mjcontrol", 0777);

					char mmapRefPath[256] = { 0 };
					sprintf(mmapRefPath, "/sdcard/MojingSDK/mjcontrol/control_%d.data", m_cid);

#ifdef _DEBUG
					__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "control pose mmap %s\n", (const char*)&mmapRefPath);
#endif				

					if ((mmap_fd = open((const char*)&mmapRefPath, O_CREAT | O_RDWR)) < 0)
					{
#ifdef _DEBUG
						__android_log_print(ANDROID_LOG_ERROR, "JNIMsg", "Open error(%d). path=%s\n", errno, (const char*)&mmapRefPath);
						MOJING_TRACE(g_APIlogger, "Open error(" << errno << "). path=" << (const char*)&mmapRefPath);
#endif
						sleep(1);
					}
					else
					{
						ftruncate(mmap_fd, sizeof(MMapedControlSensorData));
						if ((mmap_ptr = (MMapedControlSensorData *)mmap(NULL, sizeof(MMapedControlSensorData), PROT_READ | PROT_WRITE, MAP_SHARED, mmap_fd, 0)) == (void *)-1)
						{
#ifdef _DEBUG
							__android_log_print(ANDROID_LOG_WARN, "JNIMsg", "mmap error\n");
#endif
							mmap_ptr = NULL;
							sleep(1);
						}

						close(mmap_fd);
					}
				}

				// replace trywait with timedwait to reduce CPU usage
				if (mmap_ptr != NULL && 0 == sem_timedwait_millsecs(&mmap_ptr->semlock, 100))
				{
					if (!GetExitFlag())
					{
						if ( (mmap_ptr->TimeS >= m_fCurrentSampleTime)
							|| fabs(mmap_ptr->TimeS - m_fCurrentSampleTime) > 0.1f)
						{
							m_CurrentDataInfo = mmap_ptr->CurrentData;
							
							m_fCurrentSampleTime = mmap_ptr->TimeS;
							
							// 当mmap_ptr->CurrentData.m_Orientation == mmap_ptr->FixData.m_Orientation，表示这个采样被Reset过了
							if (0 == memcmp(mmap_ptr->CurrentData.m_Orientation, mmap_ptr->FixData.m_Orientation, sizeof(mmap_ptr->FixData.m_Orientation)))
							{
								m_FixDataInfo = m_CurrentDataInfo;
								m_fFixSampleTime = m_fCurrentSampleTime;
							}
#ifdef ENABLE_SENSOR_LOGGER
						 __android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", \
							"control pose data from service. controlID: %d , TimeS/m_fSampleTime: %f/%f, Current Orientation (%f / %f / %f / %f), FixData Orientation (%f / %f / %f / %f),\
							AngularAcceleration(%f / %f / %f), LinearAcceleration(%f / %f / %f)\n", m_cid, mmap_ptr->TimeS, m_fCurrentSampleTime, \
							m_CurrentDataInfo.m_Orientation.x, m_CurrentDataInfo.m_Orientation.y, m_CurrentDataInfo.m_Orientation.z, m_CurrentDataInfo.m_Orientation.w, \
							m_FixDataInfo.m_Orientation.x, m_FixDataInfo.m_Orientation.y, m_FixDataInfo.m_Orientation.z, m_FixDataInfo.m_Orientation.w, \
							m_CurrentDataInfo.m_AngularAccel.x, m_CurrentDataInfo.m_AngularAccel.y, m_CurrentDataInfo.m_AngularAccel.z, \
							m_CurrentDataInfo.m_LinearAccel.x, m_CurrentDataInfo.m_AngularAccel.y, m_CurrentDataInfo.m_AngularAccel.z);
#endif				
						}
					}
				}
			}

			

			if (mmap_ptr)
			{
				sem_destroy(&mmap_ptr->semlock);
				mmap_ptr = NULL;
			}

			MOJING_TRACE(g_APIlogger, "Exit ControlPose id = " << m_cid);
			
			return 0;
		}

		int ControlPose::GetKeymask(int *pKeyMask)
		{
			return 0;
		}

		float ControlPose::GetControlCurrentPose(float *fOrientation, float *fAngularAccel, float *fLinearAccel)
		{
			//MOJING_FUNC_TRACE(g_APIlogger);
			fOrientation[0] = m_CurrentDataInfo.m_Orientation.x;
			fOrientation[1] = m_CurrentDataInfo.m_Orientation.y;
			fOrientation[2] = m_CurrentDataInfo.m_Orientation.z;
			fOrientation[3] = m_CurrentDataInfo.m_Orientation.w;

			fAngularAccel[0] = m_CurrentDataInfo.m_AngularAccel.x;
			fAngularAccel[1] = m_CurrentDataInfo.m_AngularAccel.y;
			fAngularAccel[2] = m_CurrentDataInfo.m_AngularAccel.z;

			fLinearAccel[0] = m_CurrentDataInfo.m_LinearAccel.x;
			fLinearAccel[1] = m_CurrentDataInfo.m_LinearAccel.y;
			fLinearAccel[2] = m_CurrentDataInfo.m_LinearAccel.z;

			return m_fCurrentSampleTime;
		}

		float ControlPose::GetControlFixPose(float *fOrientation, float *fAngularAccel, float *fLinearAccel)
		{
			//MOJING_FUNC_TRACE(g_APIlogger);		
			fOrientation[0] = m_FixDataInfo.m_Orientation.x;
			fOrientation[1] = m_FixDataInfo.m_Orientation.y;
			fOrientation[2] = m_FixDataInfo.m_Orientation.z;
			fOrientation[3] = m_FixDataInfo.m_Orientation.w;

			fAngularAccel[0] = m_FixDataInfo.m_AngularAccel.x;
			fAngularAccel[1] = m_FixDataInfo.m_AngularAccel.y;
			fAngularAccel[2] = m_FixDataInfo.m_AngularAccel.z;

			fLinearAccel[0] = m_FixDataInfo.m_LinearAccel.x;
			fLinearAccel[1] = m_FixDataInfo.m_LinearAccel.y;
			fLinearAccel[2] = m_FixDataInfo.m_LinearAccel.z;

			return m_fFixSampleTime;
		}


		ControllerTracker::ControllerTracker()
		{
			for (int i = 0; i < MAX_CONTROL_NUM; i++)
			{
				m_pControl[i] = NULL;
			}
		}

		ControllerTracker::~ControllerTracker()
		{
			Destroy();
		}

		int ControllerTracker::Init()
		{
			//MOJING_FUNC_TRACE(g_APIlogger);	
			for (int i = 0; i < MAX_CONTROL_NUM; i++)
			{
				m_pControl[i] = new ControlPose(i + 1);
				if (m_pControl[i]->StartTracker() == false)
				{
					return false;
				}
			}
			return true;
		}

		void ControllerTracker::Destroy()
		{
			for (int i = 0; i < MAX_CONTROL_NUM; i++)
			{
				if (m_pControl[i])
				{
					m_pControl[i]->StopTracker();
					delete m_pControl[i];
					m_pControl[i] = NULL;
				}
			}
		}

		bool ControllerTracker::StartControlTracker(int cid)
		{
			int id = cid - 1;
			if (id < 0 || id >= MAX_CONTROL_NUM) return false;

			if (m_pControl[id])
			{
				return m_pControl[id]->StartTracker();
			}
			return false;
		}

		void ControllerTracker::StopControlTracker(int cid)
		{
			int id = cid - 1;
			if (id < 0 || id >= MAX_CONTROL_NUM) return;

			if (m_pControl[id])
			{
				m_pControl[id]->StopTracker();
			}
		}

		int ControllerTracker::GetKeymask(int cid, int *pKeyMask)
		{
			int id = cid - 1;
			if (id < 0 || id >= MAX_CONTROL_NUM) return 0;

			if (m_pControl[id])
			{
				return m_pControl[id]->GetKeymask(pKeyMask);
			}
			return 0;
		}

		float ControllerTracker::GetControlCurrentPose(int cid, float *fOrientation, float *fAngularAccel, float *fLinearAccel)
		{
			int id = cid - 1;
			if (id < 0 || id >= MAX_CONTROL_NUM) return 0;

			if (m_pControl[id])
			{
				return m_pControl[id]->GetControlCurrentPose(fOrientation, fAngularAccel, fLinearAccel);
			}
			return 0;
		}

		float ControllerTracker::GetControlFixPose(int cid, float *fOrientation, float *fAngularAccel, float *fLinearAccel)
		{
			int id = cid - 1;
			if (id < 0 || id >= MAX_CONTROL_NUM) return 0;

			if (m_pControl[id])
			{
				return m_pControl[id]->GetControlFixPose(fOrientation, fAngularAccel, fLinearAccel);
			}
			return 0;
		}
	}
}
