#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <stdio.h>

#ifdef MJ_OS_ANDROID
#include <android/log.h>
#endif
#include "../Base/MojingMath.h"
#include "../Base/MojingTimer.h"
#include "../Base/MojingLog.h"
#include "../Base/MojingThreads.h"
#include "../MojingManager.h"
#include "../MojingSDKStatus.h"
#include "MojingControllerSocket.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#define LOCAL_RECV_PORT 9414 

namespace Baofeng
{
	namespace Mojing
	{
		Mutex *g_pUpdateDataLocker = NULL;
		ControllerInfo::ControllerInfo(int cid)
			: m_Status(0)
			, m_fCurrentSampleTime(0)
			, m_fFixSampleTime(0)
		{
			m_cid = cid;
		}

		ControllerInfo::~ControllerInfo()
		{

		}

		int ControllerInfo::GetKeymask(int *pKeyMask)
		{
			return 0;
		}
        const double LINEAR_ACCEL_SCALE = 0.000488;
        const double ANGULAR_ACCEL_SCALE = 0.153;
		void ControllerInfo::ParseMMapData(MMapedControllerData* pMMapData)
		{
#ifdef MJ_OS_ANDROID
#ifdef ENABLE_SENSOR_LOGGER
            //if (m_cid == 1)
            {
                char msg[1024];
                sprintf(msg, "ParseMMapData Controller ID = %d, Current Orientation (%f / %f / %f / %f), FixData Orientation (%f / %f / %f / %f), AngularAcceleration(%f / %f / %f), LinearAcceleration(%f / %f / %f)", \
                    m_cid, pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_Orientation[0], pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_Orientation[1], pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_Orientation[2], pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_Orientation[3], \
                    pMMapData->m_JoyStick[m_cid - 1].FixData.m_Orientation[0], pMMapData->m_JoyStick[m_cid - 1].FixData.m_Orientation[1], pMMapData->m_JoyStick[m_cid - 1].FixData.m_Orientation[2], pMMapData->m_JoyStick[m_cid - 1].FixData.m_Orientation[3], \
					pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_AngularAccel[0], pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_AngularAccel[1], pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_AngularAccel[2], \
					pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_LinearAccel[0], pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_LinearAccel[1], pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_LinearAccel[2]);

                __android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "%s\n", msg);
                MOJING_TRACE(g_APIlogger, msg);
            }
#endif
#endif
			if (g_pUpdateDataLocker == NULL)
			{
				g_pUpdateDataLocker = new Mutex();
			}
			if (g_pUpdateDataLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "ParseMMapData UpdateData Locker invalid.");
				return;
			}
			g_pUpdateDataLocker->DoLock();

			m_Status = pMMapData->m_Header.ubJoyStickStatus[m_cid-1];
            if ((pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_Orientation[0] == 0) &&
                (pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_Orientation[1] == 0) &&
                (pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_Orientation[2] == 0) &&
                (pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_Orientation[3] == 0))
            {
                MOJING_ERROR(g_APIlogger , "INVALID Orientation DATA！");

                pMMapData->m_JoyStick[m_cid - 1].CurrentData.m_Orientation[3] = 1.0f;
            }

            if ((pMMapData->m_JoyStick[m_cid - 1].FixData.m_Orientation[0] == 0) &&
                (pMMapData->m_JoyStick[m_cid - 1].FixData.m_Orientation[1] == 0) &&
                (pMMapData->m_JoyStick[m_cid - 1].FixData.m_Orientation[2] == 0) &&
                (pMMapData->m_JoyStick[m_cid - 1].FixData.m_Orientation[3] == 0))
            {
                MOJING_ERROR(g_APIlogger, "INVALID FixData DATA！");

                pMMapData->m_JoyStick[m_cid - 1].FixData.m_Orientation[3] = 1.0f;
            }
            
			ControllerSensorData stSensorData = pMMapData->m_JoyStick[m_cid-1];
			m_CurrentDataInfo = stSensorData.CurrentData;
			m_fCurrentSampleTime = stSensorData.CurrentData.m_TimeInMs/1000.0;
			m_FixDataInfo = stSensorData.FixData;
			m_fFixSampleTime =  stSensorData.FixData.m_TimeInMs/1000.0;

            double lx = m_CurrentDataInfo.m_LinearAccel.x * LINEAR_ACCEL_SCALE;
            double ly = m_CurrentDataInfo.m_LinearAccel.y * LINEAR_ACCEL_SCALE;
            double lz = m_CurrentDataInfo.m_LinearAccel.z * LINEAR_ACCEL_SCALE;

            m_CurrentDataInfo.m_LinearAccel.x = lx;
            m_CurrentDataInfo.m_LinearAccel.y = ly;
            m_CurrentDataInfo.m_LinearAccel.z = lz;

            double ax = m_CurrentDataInfo.m_AngularAccel.x * ANGULAR_ACCEL_SCALE;
            double ay = m_CurrentDataInfo.m_AngularAccel.y * ANGULAR_ACCEL_SCALE;
            double az = m_CurrentDataInfo.m_AngularAccel.z * ANGULAR_ACCEL_SCALE;

            m_CurrentDataInfo.m_AngularAccel.x = ax;
            m_CurrentDataInfo.m_AngularAccel.y = ay;
            m_CurrentDataInfo.m_AngularAccel.z = az;


            lx = m_FixDataInfo.m_LinearAccel.x * LINEAR_ACCEL_SCALE;
            ly = m_FixDataInfo.m_LinearAccel.y * LINEAR_ACCEL_SCALE;
            lz = m_FixDataInfo.m_LinearAccel.z * LINEAR_ACCEL_SCALE;

            m_FixDataInfo.m_LinearAccel.x = lx;
            m_FixDataInfo.m_LinearAccel.y = ly;
            m_FixDataInfo.m_LinearAccel.z = lz;

            ax = m_FixDataInfo.m_AngularAccel.x * ANGULAR_ACCEL_SCALE;
            ay = m_FixDataInfo.m_AngularAccel.y * ANGULAR_ACCEL_SCALE;
            az = m_FixDataInfo.m_AngularAccel.z * ANGULAR_ACCEL_SCALE;

            m_FixDataInfo.m_AngularAccel.x = ax;
            m_FixDataInfo.m_AngularAccel.y = ay;
            m_FixDataInfo.m_AngularAccel.z = az;
			g_pUpdateDataLocker->Unlock();

#ifdef MJ_OS_ANDROID
#ifdef ENABLE_SENSOR_LOGGER
			//if (m_cid == 1)
			{
				char msg[1024];
				sprintf(msg, "control pose data from service. m_fCurrentSampleTime: %f, m_fFixSampleTime: %f, Current Orientation (%f / %f / %f / %f), FixData Orientation (%f / %f / %f / %f), AngularAcceleration(%f / %f / %f), LinearAcceleration(%f / %f / %f)", \
					m_fCurrentSampleTime, m_fFixSampleTime, \
					m_CurrentDataInfo.m_Orientation.x, m_CurrentDataInfo.m_Orientation.y, m_CurrentDataInfo.m_Orientation.z, m_CurrentDataInfo.m_Orientation.w, \
					m_FixDataInfo.m_Orientation.x, m_FixDataInfo.m_Orientation.y, m_FixDataInfo.m_Orientation.z, m_FixDataInfo.m_Orientation.w, \
					m_CurrentDataInfo.m_AngularAccel.x, m_CurrentDataInfo.m_AngularAccel.y, m_CurrentDataInfo.m_AngularAccel.z, \
					m_CurrentDataInfo.m_LinearAccel.x, m_CurrentDataInfo.m_LinearAccel.y, m_CurrentDataInfo.m_LinearAccel.z);

				__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "%s\n", msg);
				MOJING_TRACE(g_APIlogger, msg);
			}																																
#endif
#endif
		}
		
		void ControllerInfo::SendControllerData(float* pControllerData, double dTimestamp, bool bRecenter)
		{
			if (pControllerData == NULL)
				return;	
			
			if (g_pUpdateDataLocker == NULL)
			{
				g_pUpdateDataLocker = new Mutex();
			}
			if (g_pUpdateDataLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "SendControllerData UpdateData Locker invalid.");
				return;
			}
			g_pUpdateDataLocker->DoLock();

			if ( (pControllerData[0] == 0) &&
				(pControllerData[1] == 0) &&
				(pControllerData[2] == 0) &&
				(pControllerData[3] == 0))
			{
				MOJING_ERROR(g_APIlogger, "INVALID Orientation DATA！");
				pControllerData[3] = 1.0f;
			}
			
			m_CurrentDataInfo.m_Orientation = Quatf(pControllerData[0], pControllerData[1], pControllerData[2], pControllerData[3]);
			m_CurrentDataInfo.m_Orientation.Normalized();
			m_CurrentDataInfo.m_LinearAccel = Vector3f(pControllerData[4], pControllerData[5], pControllerData[6]);	
			m_CurrentDataInfo.m_AngularAccel = Vector3f(pControllerData[7], pControllerData[8], pControllerData[9]);	
			m_fCurrentSampleTime = dTimestamp;	
			
			if (bRecenter)
			{
#ifdef MJ_OS_ANDROID
				__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "Controller is recenter...\n");
#endif
				//m_FixDataInfo.m_Orientation = Quatf(pControllerData[0], pControllerData[1], pControllerData[2], pControllerData[3]);
				//m_FixDataInfo.m_Orientation.Normalized();
				//m_FixDataInfo.m_LinearAccel = Vector3f(pControllerData[4], pControllerData[5], pControllerData[6]);	
				//m_FixDataInfo.m_AngularAccel = Vector3f(pControllerData[7], pControllerData[8], pControllerData[9]);	
				m_FixDataInfo = m_CurrentDataInfo;
				m_fFixSampleTime = dTimestamp;
			}

			if ( (pControllerData[10] == 0) &&
				(pControllerData[11] == 0) &&
				(pControllerData[12] == 0) &&
				(pControllerData[13] == 0))
			{
				MOJING_ERROR(g_APIlogger, "INVALID Fix Orientation DATA！");
				pControllerData[13] = 1.0f;
			}
			m_FixDataInfo.m_Orientation = Quatf(pControllerData[10], pControllerData[11], pControllerData[12], pControllerData[13]);
			m_FixDataInfo.m_Orientation.Normalized();
			
			g_pUpdateDataLocker->Unlock();
#ifdef MJ_OS_ANDROID
#ifdef ENABLE_SENSOR_LOGGER
			{
				char msg[1024];
				sprintf(msg, "control pose data. m_fCurrentSampleTime: %f, m_fFixSampleTime: %f, Current Orientation (%f / %f / %f / %f), FixData Orientation (%f / %f / %f / %f), AngularAcceleration(%f / %f / %f), LinearAcceleration(%f / %f / %f)", \
					m_fCurrentSampleTime, m_fFixSampleTime, \
					m_CurrentDataInfo.m_Orientation.x, m_CurrentDataInfo.m_Orientation.y, m_CurrentDataInfo.m_Orientation.z, m_CurrentDataInfo.m_Orientation.w, \
					m_FixDataInfo.m_Orientation.x, m_FixDataInfo.m_Orientation.y, m_FixDataInfo.m_Orientation.z, m_FixDataInfo.m_Orientation.w, \
					m_CurrentDataInfo.m_AngularAccel.x, m_CurrentDataInfo.m_AngularAccel.y, m_CurrentDataInfo.m_AngularAccel.z, \
					m_CurrentDataInfo.m_LinearAccel.x, m_CurrentDataInfo.m_LinearAccel.y, m_CurrentDataInfo.m_LinearAccel.z);

				__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "%s\n", msg);
				//MOJING_TRACE(g_APIlogger, msg);
			}
#endif
#endif
		}

		float ControllerInfo::GetControlCurrentPose(float *fOrientation, float *fAngularAccel, float *fLinearAccel)
		{
			//MOJING_FUNC_TRACE(g_APIlogger);

			if (g_pUpdateDataLocker == NULL)
			{
				g_pUpdateDataLocker = new Mutex();
			}
			if (g_pUpdateDataLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "UpdateData Locker invalid.");
				return 0;
			}
			g_pUpdateDataLocker->DoLock();

			if (fOrientation)
			{
				fOrientation[0] = m_CurrentDataInfo.m_Orientation.x;
				fOrientation[1] = m_CurrentDataInfo.m_Orientation.y;
				fOrientation[2] = m_CurrentDataInfo.m_Orientation.z;
				fOrientation[3] = m_CurrentDataInfo.m_Orientation.w;
			}

			if (fAngularAccel)
			{
				fAngularAccel[0] = m_CurrentDataInfo.m_AngularAccel.x;
				fAngularAccel[1] = m_CurrentDataInfo.m_AngularAccel.y;
				fAngularAccel[2] = m_CurrentDataInfo.m_AngularAccel.z;
			}

			if (fLinearAccel)
			{
				fLinearAccel[0] = m_CurrentDataInfo.m_LinearAccel.x;
				fLinearAccel[1] = m_CurrentDataInfo.m_LinearAccel.y;
				fLinearAccel[2] = m_CurrentDataInfo.m_LinearAccel.z;
			}

			g_pUpdateDataLocker->Unlock();

			return m_fCurrentSampleTime;
		}

		float ControllerInfo::GetControlFixPose(float *fOrientation, float *fAngularAccel, float *fLinearAccel)
		{
			//MOJING_FUNC_TRACE(g_APIlogger);

			if (g_pUpdateDataLocker == NULL)
			{
				g_pUpdateDataLocker = new Mutex();
			}
			if (g_pUpdateDataLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "UpdateData Locker invalid.");
				return 0;
			}
			g_pUpdateDataLocker->DoLock();
	
			if (fOrientation)
			{
				fOrientation[0] = m_FixDataInfo.m_Orientation.x;
				fOrientation[1] = m_FixDataInfo.m_Orientation.y;
				fOrientation[2] = m_FixDataInfo.m_Orientation.z;
				fOrientation[3] = m_FixDataInfo.m_Orientation.w;
			}

			if (fAngularAccel)
			{
				fAngularAccel[0] = m_FixDataInfo.m_AngularAccel.x;
				fAngularAccel[1] = m_FixDataInfo.m_AngularAccel.y;
				fAngularAccel[2] = m_FixDataInfo.m_AngularAccel.z;
			}

			if (fLinearAccel)
			{
				fLinearAccel[0] = m_FixDataInfo.m_LinearAccel.x;
				fLinearAccel[1] = m_FixDataInfo.m_LinearAccel.y;
				fLinearAccel[2] = m_FixDataInfo.m_LinearAccel.z;
			}

			g_pUpdateDataLocker->Unlock();

			return m_fFixSampleTime;
		}


        float ControllerInfo::GetControlFixCurrentPose(float *fOrientation, float *fAngularAccel, float *fLinearAccel)
        {
            //MOJING_FUNC_TRACE(g_APIlogger);	

			if (g_pUpdateDataLocker == NULL)
			{
				g_pUpdateDataLocker = new Mutex();
			}
			if (g_pUpdateDataLocker == NULL)
			{
				MOJING_ERROR(g_APIlogger, "UpdateData Locker invalid.");
				return 0;
			}
			g_pUpdateDataLocker->DoLock();

            Quatf fixData = m_FixDataInfo.m_Orientation.Inverted() * m_CurrentDataInfo.m_Orientation;

			if (fOrientation)
			{
				fOrientation[0] = fixData.x;
				fOrientation[1] = fixData.y;
				fOrientation[2] = fixData.z;
				fOrientation[3] = fixData.w;
			}

			if (fAngularAccel)
			{
				fAngularAccel[0] = m_CurrentDataInfo.m_AngularAccel.x;
				fAngularAccel[1] = m_CurrentDataInfo.m_AngularAccel.y;
				fAngularAccel[2] = m_CurrentDataInfo.m_AngularAccel.z;
			}

			if (fLinearAccel)
			{
				fLinearAccel[0] = m_CurrentDataInfo.m_LinearAccel.x;
				fLinearAccel[1] = m_CurrentDataInfo.m_LinearAccel.y;
				fLinearAccel[2] = m_CurrentDataInfo.m_LinearAccel.z;
			}

			g_pUpdateDataLocker->Unlock();

            return m_fCurrentSampleTime;
        }

		ControllerTracker::ControllerTracker()
		{
            m_iFixScore[0] = 255;
            m_iFixScore[1] = 128;
            m_iFixStatus[0] = 255;
            m_iFixStatus[1] = 128;

			for (int i = 0; i < MAX_CONTROLLER_NUM; i++)
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
			for (int i = 0; i < MAX_CONTROLLER_NUM; i++)
			{
				m_pControl[i] = new ControllerInfo(i + 1);
			}

			/*
			if (m_pMMapData == NULL)
			{
				m_pMMapData = new MMapedControllerData();
				if (m_pMMapData == NULL)
					return 0;
			}
			*/

			return 1;
		}

		void ControllerTracker::Destroy()
		{
			for (int i = 0; i < MAX_CONTROLLER_NUM; i++)
			{
				if (m_pControl[i])
				{
					delete m_pControl[i];
					m_pControl[i] = NULL;
				}
			}

			/*
			if (m_pMMapData == NULL)
			{
				delete m_pMMapData;
				m_pMMapData = NULL;
			}
			*/
		}

		/*
		int ControllerTracker::StartTracker()
		{
		
			int m_Recv_Sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (m_Recv_Sockfd < 0){
				MOJING_ERROR(g_APIlogger, "open  socket fail!!!");
				return -1;
			}
			m_ser_addr.sin_family = AF_INET;
			m_ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			m_ser_addr.sin_port = htons(LOCAL_RECV_PORT);
			int res = bind(m_Recv_Sockfd, (struct sockaddr*)&m_ser_addr, sizeof(struct sockaddr_in));
			__android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "bind res[%u] , sin_port[%u]", res, m_ser_addr.sin_port);
			if (res != 0)
			{
				return res;
			}

			//MOJING_FUNC_TRACE(g_APIlogger);	
			return Thread::Start() ? 1 : 0;
		}

		void ControllerTracker::StopTracker()
		{
			Thread::SetExitFlag(true);
		}

		int ControllerTracker::Run()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			if (m_pMMapData == NULL)
				return -1;

			SetThreadName("ControllerTrackerSocket");
			int addr_len = sizeof(struct sockaddr_in);

			while (!GetExitFlag()) {
				int len_read_net = recvfrom(m_Recv_Sockfd, (void *)m_pMMapData, sizeof(MMapedControllerData), 0, (struct sockaddr*)&m_ser_addr, &addr_len);
				if (len_read_net == -1)
				{
					MOJING_WARN(g_APIlogger, "recive data failed!!!");
				}
				else
				{
					bool bRet = ReadMMapData();
				}
			}

			MOJING_TRACE(g_APIlogger, "Exit ControllerTrackerSocket" );

			return 0;
		}
		*/

		void ControllerTracker::SendControllerData(float* pControllerData, double dTimestamp, bool bRecenter)
		{
			if (pControllerData == NULL || m_pControl[0] == NULL)
				return;	
			
			m_pControl[0]->SendControllerData(pControllerData, dTimestamp, bRecenter);
		}
		
		bool ControllerTracker::ReadMMapData(const UByte* pControllerData, int dataLen)
		{
			if (pControllerData == NULL)
				return false;
		
			MMapedControllerData *pMMapData = (MMapedControllerData *)pControllerData;
			//MOJING_TRACE(g_APIlogger, "ReadMMapData, data len: " << dataLen << " szTag: " << pMMapData->m_Header.szTag);
			if (pMMapData->m_Header.szTag[0] != 'M' || pMMapData->m_Header.szTag[1] != 'J')
			{
				MOJING_WARN(g_APIlogger, "data error!!!");
				return false;
			}

			for (int i = 0; i < MAX_CONTROLLER_NUM; i++)
			{
				if (m_pControl[i] != NULL && pMMapData->m_Header.ubJoyStickStatus[i] != 0 )
				{
					m_pControl[i]->ParseMMapData(pMMapData);
				}
			}	
   
            ParseExData((UByte*)pControllerData);
			return true;
		}

        bool ControllerTracker::ParseExData( UByte* pData)
        {
            UInt16 iDatalen = *((UInt16 *)(pData + sizeof(MMapedControllerData)));

            if (iDatalen < 5)
            {
                return false;
            }
            UByte* pDataTemp = pData + sizeof(MMapedControllerData)+sizeof(UInt16);
            int dataPos = 0;
            ExpandPackHeader * p = (ExpandPackHeader*)(pDataTemp + dataPos);

#if 0
            char log[1024] = { 0 };
#endif
            // while (iDatalen)
            while (dataPos < (iDatalen - 1) && p->Length >= 4)
            {
                p = (ExpandPackHeader*)(pDataTemp + dataPos);
            
                //MOJING_TRACE(g_APIlogger, "ParseExData, dataPos: " << dataPos << "       Len = " << p->Length  << "  Type = " << p->Type);
                if (p->Type == 0x02)
                {
                    ParseFixExData((UByte*)p);
                }
                dataPos += p->Length;
                // iDatalen -= p->Length;
            }
            return true;
        }

        void ControllerTracker::ParseFixExData(UByte* pData)
        {
            ExpandFixPack *p = (ExpandFixPack *)pData;
            if (p->Type != 0x02 || p->Version != 1)
            {
                return;
            }
//             __android_log_print(ANDROID_LOG_DEBUG, "JNIMsg", "ParseFixScore flag = %d,  score = %d, status = %d",
//                 p->AppendData.bFlag,
//                 p->AppendData.iCal_Score,
//                 p->AppendData.bCal_status);
            if (p->AppendData.bFlag == 1)
            {
                m_iFixScore[0] = p->AppendData.iCal_Score;
                m_iFixStatus[0] = p->AppendData.bCal_status;
            }

            if (p->AppendData.bFlag == 2)
            {
                m_iFixScore[1] = p->AppendData.iCal_Score;
                m_iFixStatus[1] = p->AppendData.bCal_status;
            }
        }

        int ControllerTracker::GetKeymask(int cid, int *pKeyMask)
		{
			int id = cid - 1;
			if (id < 0 || id >= MAX_CONTROLLER_NUM) return 0;

			if (m_pControl[id])
			{
				return m_pControl[id]->GetKeymask(pKeyMask);
			}
			return 0;
		}

		float ControllerTracker::GetControlCurrentPose(int cid, float *fOrientation, float *fAngularAccel, float *fLinearAccel)
		{
			int id = cid - 1;
			if (id < 0 || id >= MAX_CONTROLLER_NUM) return 0;

			if (m_pControl[id])
			{
				return m_pControl[id]->GetControlCurrentPose(fOrientation, fAngularAccel, fLinearAccel);
			}
			return 0;
		}

		float ControllerTracker::GetControlFixPose(int cid, float *fOrientation, float *fAngularAccel, float *fLinearAccel)
		{
			int id = cid - 1;
			if (id < 0 || id >= MAX_CONTROLLER_NUM) return 0;

			if (m_pControl[id])
			{
				return m_pControl[id]->GetControlFixPose(fOrientation, fAngularAccel, fLinearAccel);
			}
			return 0;
		}

        float ControllerTracker::GetControlFixCurrentPose(int cid, float *fOrientation, float *fAngularAccel, float *fLinearAccel)
        {
            int id = cid - 1;
            if (id < 0 || id >= MAX_CONTROLLER_NUM) return 0;

            if (m_pControl[id])
            {
                return m_pControl[id]->GetControlFixCurrentPose(fOrientation, fAngularAccel, fLinearAccel);
            }
            return 0;
        }

        void ControllerTracker::GetFixScore(int* pStatus, int* pScore)
        {
            if (pScore)
            {
                pScore[0] = m_iFixScore[0];
                pScore[1] = m_iFixScore[1];
            }

            if (pStatus)
            {
                pStatus[0] = m_iFixStatus[0];
                pStatus[1] = m_iFixStatus[1];
            }
        }
	}
}
