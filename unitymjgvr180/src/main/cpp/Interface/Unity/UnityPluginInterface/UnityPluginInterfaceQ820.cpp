#include "UnityPluginInterfaceQ820.h"
#include <android/native_window_jni.h>
#include "../../../Base/GlUtils.h"
#include "../../../Base/MojingTimer.h"
#include "../../../MojingAPI.h"
//#include "../../../Render/GlGeometry.h"
#include "../../../Distortion/MojingDistortion.h"
#include "../../../Distortion/GVR/GvrProfile.h"
#include "../../../MojingManager.h"
#include "../../../Parameters/MojingParameters.h"
#include "../../../Parameters/MojingDeviceParameters.h"
extern JavaVM* gJavaVM;
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif


namespace Baofeng
{
	namespace Mojing
	{
		JNIEnv* GetJNIEnv(JavaVM* javaVM)
		{
			JNIEnv* jniEnv;
			javaVM->AttachCurrentThread(&jniEnv, NULL);
			return jniEnv;
		}
		jobject GetViewSurface(JNIEnv * javaEnv, jobject activity)
		{
			jclass activityClass = javaEnv->GetObjectClass(activity);
			if (activityClass == NULL)
			{
				MOJING_ERROR(g_APIlogger,"activityClass == NULL!");
				return NULL;
			}

			jfieldID fid = javaEnv->GetFieldID(activityClass, "mUnityPlayer", "Lcom/unity3d/player/UnityPlayer;");
			if (fid == NULL)
			{
				MOJING_ERROR(g_APIlogger,"mUnityPlayer not found!");
				return NULL;
			}

			jobject unityPlayerObj = javaEnv->GetObjectField(activity, fid);
			if (unityPlayerObj == NULL)
			{
				MOJING_ERROR(g_APIlogger,"unityPlayer object not found!");
				return NULL;
			}

			jclass unityPlayerClass = javaEnv->GetObjectClass(unityPlayerObj);
			if (unityPlayerClass == NULL)
			{
				MOJING_ERROR(g_APIlogger,"unityPlayer class not found!");
				return NULL;
			}

			jmethodID mid = javaEnv->GetMethodID(unityPlayerClass, "getChildAt", "(I)Landroid/view/View;");
			if (mid == NULL)
			{
				MOJING_ERROR(g_APIlogger,"getChildAt methodID not found!");
				return NULL;
			}

			jboolean param = 0;
			jobject surfaceViewObj = javaEnv->CallObjectMethod(unityPlayerObj, mid, param);
			if (surfaceViewObj == NULL)
			{
				MOJING_ERROR(g_APIlogger,"surfaceView object not found!");
				return NULL;
			}

			jclass surfaceViewClass = javaEnv->GetObjectClass(surfaceViewObj);
			mid = javaEnv->GetMethodID(surfaceViewClass, "getHolder", "()Landroid/view/SurfaceHolder;");
			if (mid == NULL)
			{
				MOJING_ERROR(g_APIlogger,"getHolder methodID not found!");
				return NULL;
			}

			jobject surfaceHolderObj = javaEnv->CallObjectMethod(surfaceViewObj, mid);
			if (surfaceHolderObj == NULL)
			{
				MOJING_ERROR(g_APIlogger,"surfaceHolder object not found!");
				return NULL;
			}

			jclass surfaceHolderClass = javaEnv->GetObjectClass(surfaceHolderObj);
			mid = javaEnv->GetMethodID(surfaceHolderClass, "getSurface", "()Landroid/view/Surface;");
			if (mid == NULL)
			{
				MOJING_ERROR(g_APIlogger,"getSurface methodID not found!");
				return NULL;
			}

			return javaEnv->CallObjectMethod(surfaceHolderObj, mid);
		}

		UnityPluginInterfaceQ820::UnityPluginInterfaceQ820():
			m_iFrameIndex(0),
			m_bInitialized(false),
			m_bSvrBegin(false),
			m_pJNIEnv(NULL),
			m_Activity(NULL),
			m_iSurfaceValidWaitFramesCounter(0),
			m_iSurfaceValidWaitFrames(30)
		{
			// MOJING_FUNC_TRACE(g_APIlogger);
			SetClassName(__FUNCTION__);
		}


		UnityPluginInterfaceQ820::~UnityPluginInterfaceQ820()
		{
			
		}

		/* Update Warpmesh */
		void UnityPluginInterfaceQ820::OnEvent_UpdateWarpmesh_Q820()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
#ifndef MJ_OVERLAY
			return;
#endif
			// create WarpMesh
			Mesh_820 warpMesh;
			Distortion* pDistortion = Manager::GetMojingManager()->GetDistortion();
#ifdef _DEBUG
			FILE *pDURL = fopen("/sdcard/DURL.txt" , "r");
			if (pDURL)
			{
				char szTemp[128];
				*szTemp = 0;
				fgets(szTemp ,128 , pDURL);
				fclose(pDURL);
				MOJING_TRACE(g_APIlogger , "Find DURL FILE ， DURL = " << szTemp);
				CDayDreamParameters DD = DD.FromDayDreamURL(szTemp);
				if (DD.GetNumK() >= 2)
				{
					MOJING_TRACE(g_APIlogger, "Set DURL to Distortion");
					pDistortion->SetDURL(szTemp);
				}
				else
				{
					MOJING_ERROR(g_APIlogger, "INVALID DURL");
				}
			}
			else
			{
				MOJING_TRACE(g_APIlogger, "Can not Find DURL FILE ");
			}
#endif
			/******************************************************************************/
			/* 注意：因为我们的畸变效果与DayDream游戏还有差异，所以这里使用特殊的参数设置*/
			/******************************************************************************/
//		  CgzmmrTpo47prZTplZwSBk1hdHJpeB2WQws9Jc3MTD0qEAAALEIAADRCAAAsQgAANEJYATUpXA89OgjNzEw9ZmYmP1ABYAA
//		  CgzmmrTpo47prZTplZwSBk1hdHJpeB1wzgg9JS5WVD0qEAAAMEIAADBCAAAsQgAANEJYATUpXA89OgjNzEw9ZmYmP1AAYAA
			//			pDistortion->SetDURL("CgzmmrTpo47prZTplZwSBk1hdHJpeB1wzgg9JS5WVD0qEAAAMEIAADBCAAAsQgAANEJYATUpXA89OgjNzEw9ZmYmP1AAYAA");
			void* vbuffer = pDistortion->BuildDistortionBuffer_V2(warpMesh, 32, 32);
			float* pVertexLeft = &(warpMesh.vertices_left[0]);
			float* pVertexRight = &(warpMesh.vertices_right[0]);

			unsigned int* pIndexLeft = &(warpMesh.indices[0]);

			size_t numVertex = warpMesh.vertices_left.size();
			size_t numIndex = warpMesh.indices.size();
			MOJING_TRACE(g_APIlogger, "VertexCount = " << numVertex << " , IndexCount = " << numIndex);
			m_SVRApi.UpdateWarpmesh(pVertexLeft, pVertexRight, numVertex, pIndexLeft, numIndex);
		}

		bool UnityPluginInterfaceQ820::OnEvent_Initialize()
		{
			/*SvrManager 的 Start函数*/
			MOJING_FUNC_TRACE(g_APIlogger);
			bool bRet = false;
			MOJING_TRACE(g_APIlogger, "OnEvent_Initialize  A");
			if (gJavaVM && !m_bInitialized)
			{
				MOJING_TRACE(g_APIlogger, "OnEvent_Initialize  B");
				/*Initialize*/
				if (NULL == m_pJNIEnv)
					gJavaVM->AttachCurrentThread(&m_pJNIEnv, NULL);
				MOJING_TRACE(g_APIlogger, "OnEvent_Initialize  C");
#ifdef _DEBUG
				char szLog[256];
				sprintf(szLog, "JavaVm = 0x%08X JNIEnv = 0x%08X ActivityObject = 0x%08X",
					(unsigned int)gJavaVM, (unsigned int)m_pJNIEnv, (unsigned int)m_InitParameters.m_pParams);
				MOJING_TRACE(g_APIlogger , szLog);
#endif

				MOJING_TRACE(g_APIlogger, "OnEvent_Initialize  D");
				svrInitParams params;
				memset(&params, 0, sizeof(params));
				params.javaActivityObject = m_Activity = m_pJNIEnv->NewGlobalRef((jobject)m_InitParameters.m_pParams);
				MOJING_TRACE(g_APIlogger, "OnEvent_Initialize  E");
				params.javaVm = gJavaVM;
				params.javaEnv = m_pJNIEnv;
				MOJING_TRACE(g_APIlogger, "Befor svrInitialize");
				m_bInitialized = m_SVRApi.Initialize(&params);
				MOJING_TRACE(g_APIlogger, "After svrInitialize");
				if (!m_bInitialized)
				{
					MOJING_ERROR(g_APIlogger, "svrInitialize FAILD");
				}
				else
				{
					// 注意： 现在要先Initialize
					m_SVRApi.SetTrackingMode(kTrackingRotation);
					MOJING_TRACE(g_APIlogger, "svrInitialize SUCCEED");
				}
				//gJavaVM->DetachCurrentThread();
				/*End of Initialize*/
				return m_bInitialized;
			}
			else
			{
				MOJING_TRACE(g_APIlogger, "OnEvent_Initialize  F");
				MOJING_TRACE(g_APIlogger, "JavaVm = " << gJavaVM << " , m_bInitialized = " << m_bInitialized);
			}
			return m_bInitialized;
		}
		
		bool UnityPluginInterfaceQ820::Init(__tagUnityInterfaceInitParams *pInitParameters)
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			if (m_SVRApi.Init() && UnityPluginInterfaceBase::Init(pInitParameters))
			{
				// 这里增加判定是否是820平台的代码
				if (pInitParameters->m_pParams)
				{
					JNIEnv* jniEnv;
					gJavaVM->AttachCurrentThread(&jniEnv, 0);
					pInitParameters->m_pParams = jniEnv->NewGlobalRef((jobject) pInitParameters->m_pParams);
					m_iSurfaceValidWaitFramesCounter = 0;
				}
				return true;
			}
			else
			{
				MOJING_TRACE(g_APIlogger , "Init ERROR!");
				MojingDeviceParameters* pDeviceParameters = Manager::GetMojingManager()->GetParameters()->GetDeviceParameters();
				int iAbility = pDeviceParameters->GetAbility();
				pDeviceParameters->SetAbility(iAbility ^ DEVICE_ABILITY_SVR);

			}
			return false;
		}
		bool UnityPluginInterfaceQ820::CanEnterMojingWorld()
		{
			if (!m_bInitialized)
				OnEvent_Initialize();

			JNIEnv* javaEnv = GetJNIEnv(gJavaVM);
				
			jobject surface = GetViewSurface(javaEnv, m_Activity);

			ANativeWindow * nativeWindow = ANativeWindow_fromSurface(javaEnv, surface);

			int currentSurfaceHeight = ANativeWindow_getHeight(nativeWindow);
			
			svrDeviceInfo info = m_SVRApi.GetDeviceInfo();

			if (currentSurfaceHeight != info.displayHeightPixels)
			{
				// LogDebug("Current Surface Height: %d  - DeviceInfoHeight: %d", (int)currentSurfaceHeight, info.displayHeightPixels);
				MOJING_ERROR(g_APIlogger , "Current Surface Height: " << (int)currentSurfaceHeight  << " - DeviceInfoHeight: " << info.displayHeightPixels);
				ANativeWindow_release(nativeWindow);
				return false;
			}

			MOJING_TRACE(g_APIlogger, "Current Surface Height: " << (int)currentSurfaceHeight << " - DeviceInfoHeight: " << info.displayHeightPixels << "\nTest : " << m_iSurfaceValidWaitFramesCounter << "/" << m_iSurfaceValidWaitFrames);
			//LogDebug("Current Surface Height: %d  - DeviceInfoHeight: %d", (int)currentSurfaceHeight, info.displayHeightPixels);

			ANativeWindow_release(nativeWindow);
			m_iSurfaceValidWaitFramesCounter++;

			if (m_iSurfaceValidWaitFramesCounter >= m_iSurfaceValidWaitFrames)
			{
				return true;
			}

			return false;
		}

		bool UnityPluginInterfaceQ820::OnEvent_BeginVR()
		{
			 MOJING_FUNC_TRACE(g_APIlogger);
			/*BeginVr*/
	
			jclass activityClass = m_pJNIEnv->GetObjectClass((jobject)m_Activity);
			if (activityClass == NULL)
			{
				MOJING_ERROR(g_APIlogger , "activityClass == NULL!");
				return false;
			}

			jfieldID fid = m_pJNIEnv->GetFieldID(activityClass, "mUnityPlayer", "Lcom/unity3d/player/UnityPlayer;");
			if (fid == NULL)
			{
				MOJING_ERROR(g_APIlogger , "mUnityPlayer not found!");
				return false;
			}

			jobject unityPlayerObj = m_pJNIEnv->GetObjectField((jobject)m_Activity, fid);
			if (unityPlayerObj == NULL)
			{
				MOJING_ERROR(g_APIlogger , "unityPlayer object not found!");
				return false;
			}

			jclass unityPlayerClass = m_pJNIEnv->GetObjectClass(unityPlayerObj);
			if (unityPlayerClass == NULL)
			{
				MOJING_ERROR(g_APIlogger , "unityPlayer class not found!");
				return false;
			}

			jmethodID mid = m_pJNIEnv->GetMethodID(unityPlayerClass, "getChildAt", "(I)Landroid/view/View;");
			if (mid == NULL)
			{
				MOJING_ERROR(g_APIlogger , "getChildAt methodID not found!");
				return false;
			}

			jboolean param = 0;
			jobject surfaceViewObj = m_pJNIEnv->CallObjectMethod(unityPlayerObj, mid, param);
			if (surfaceViewObj == NULL)
			{
				MOJING_ERROR(g_APIlogger , "surfaceView object not found!");
				return false;
			}

			jclass surfaceViewClass = m_pJNIEnv->GetObjectClass(surfaceViewObj);
			mid = m_pJNIEnv->GetMethodID(surfaceViewClass, "getHolder", "()Landroid/view/SurfaceHolder;");
			if (mid == NULL)
			{
				MOJING_ERROR(g_APIlogger , "getHolder methodID not found!");
				return false;
			}

			jobject surfaceHolderObj = m_pJNIEnv->CallObjectMethod(surfaceViewObj, mid);
			if (surfaceHolderObj == NULL)
			{
				MOJING_ERROR(g_APIlogger , "surfaceHolder object not found!");
				return false;
			}

			jclass surfaceHolderClass = m_pJNIEnv->GetObjectClass(surfaceHolderObj);
			mid = m_pJNIEnv->GetMethodID(surfaceHolderClass, "getSurface", "()Landroid/view/Surface;");
			if (mid == NULL)
			{
				MOJING_ERROR(g_APIlogger , "getSurface methodID not found!");
				return false;
			}

			jobject surface = m_pJNIEnv->CallObjectMethod(surfaceHolderObj, mid);

#ifdef _DEBUG
			char szLog[64];
			sprintf(szLog, "ANativeWindow = 0x%08X", (unsigned int)surface);
			// MOJING_TRACE(g_APIlogger, szLog);

#endif
			svrBeginParams params;
			memset(&params, 0, sizeof(params));

			ANativeWindow* nativewindow = ANativeWindow_fromSurface(m_pJNIEnv, surface);

			params.cpuPerfLevel = kPerfSystem;// CPU频率由系统调节
			params.gpuPerfLevel = kPerfSystem; // kPerfMaximum;// GPU频率锁死最高频
			params.mainThreadId = gettid(); // 看不出来这个TID应该是畸变线程的还是主线程的
			params.nativeWindow = nativewindow;
			MOJING_TRACE(g_APIlogger , "Befor call svrBeginVr");
			
			m_SVRApi.BeginVr(&params);

			ANativeWindow_release(nativewindow);

			MOJING_TRACE(g_APIlogger, "After call svrBeginVr");
			//gJavaVM->DetachCurrentThread();
			m_iFrameIndex = 0;
			return true;
		}
		void UnityPluginInterfaceQ820::OnEvent_EnterMojingWorld()
		{	/*注意： 因为实际使用的绘制类不一样，所以这里不能调用基类的函数*/
			MOJING_FUNC_TRACE(g_APIlogger);
			if (GetIsInMojingWorld())
			{
				MOJING_ERROR(g_APIlogger, "Call OnEvent_EnterMojingWorld again!");
				return;
			}
			// 注意：因为Android的代码中会强起Tracker，所以这里要关掉
			// MojingSDK_StopTracker();

			MojingSDK_SetEngineVersion(m_strEngineVersion.c_str());
			bool bRet = MojingSDK_EnterMojingWorld(m_strGlassName.c_str(), false, m_TimeWarpStatus.m_bTimeWarpType);
			if (bRet&&  OnEvent_Initialize())
			{
				m_TimeWarpStatus.m_bTimeWarpTypeChanged = false;
				// 注意：Resume的时候不会额外调用SetCenterLine，所以无论如何这里要重新设置一下CenterLine。
				m_CenterLine.m_bChanged = true;
				SetIsInMojingWorld(true);
				MOJING_TRACE(g_APIlogger, "MojingSDK_EnterMojingWorld = " << bRet);

				/*TODO：这里要向SVR提交Mesh表*/
				if (!m_bSvrBegin)
					m_bSvrBegin = OnEvent_BeginVR();
				if (m_bSvrBegin)
				{
					OnEvent_UpdateWarpmesh_Q820();
				}
				else
				{
					MOJING_ERROR(g_APIlogger , "Call Svr Begin FAILD");
				}
			}
			else
			{
				MOJING_ERROR(g_APIlogger , "UnityPluginInterfaceQ820::OnEvent_EnterMojingWorld - FAILD");
			}
		}

		void UnityPluginInterfaceQ820::OnEvent_LeaveMojingWorld()
		{
			if (GetIsInMojingWorld())
			{
				MOJING_FUNC_TRACE(g_APIlogger);
				UnityPluginInterfaceBase::OnEvent_LeaveMojingWorld();
				// m_bInitialized = false;
				// sleep(2);
				MOJING_TRACE(g_APIlogger, "Befor svrEndVr");
				// 注意 ： 开启下行代码会导致1.1.0不能退出
				m_bSvrBegin = false;
				m_SVRApi.EndVr();
				MOJING_TRACE(g_APIlogger, "After svrEndVr");
				// MOJING_TRACE(g_APIlogger , "delete m_pShader");
				m_iFrameIndex = 0;
			}
			UnityPluginInterfaceBase::OnEvent_LeaveMojingWorld();
		}

		void UnityPluginInterfaceQ820::OnEvent_ChangeMojingWorld()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			UnityPluginInterfaceBase::OnEvent_ChangeMojingWorld();
			/*ToDo 这里要更新Mesh表*/
			OnEvent_UpdateWarpmesh_Q820();
		}

		void UnityPluginInterfaceQ820::OnEvent_DestroyMojingWorld()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			MOJING_TRACE(g_APIlogger, "Befor Shutdown");
#if (SVR_VERSION == 113)
 			m_bInitialized = false;
 			m_SVRApi.Shutdown();

			m_pJNIEnv->DeleteGlobalRef(m_Activity);
			m_Activity = NULL;
#endif
			MOJING_TRACE(g_APIlogger, "After Shutdown");
		}
		void UnityPluginInterfaceQ820::OnEvent_SetTextureID()
		{
			++m_iFrameIndex; 
			m_bFrameIndexChanged = true;
		}

#if (SVR_VERSION == 113)
		void L_CreateLayout(float centerX, float centerY, float radiusX, float radiusY, svrOverlayLayout *pLayout)
		{
			float lowerLeft[4] = { centerX - radiusX, centerY - radiusY, 0.0f, 0.0f };
			float lowerRight[4] = { centerX + radiusX, centerY - radiusY, 1.0f, 0.0f };
			float upperLeft[4] = { centerX - radiusX, centerY + radiusY, 0.0f, 1.0f };
			float upperRight[4] = { centerX + radiusX, centerY + radiusY, 1.0f, 1.0f };

			memcpy(pLayout->LowerLeft, lowerLeft, sizeof(lowerLeft));
			memcpy(pLayout->LowerRight, lowerRight, sizeof(lowerRight));
			memcpy(pLayout->UpperLeft, upperLeft, sizeof(upperLeft));
			memcpy(pLayout->UpperRight, upperRight, sizeof(upperRight));
		}
#endif
		void UnityPluginInterfaceQ820::OnEvent_DistortFrame()
		{
			/*
			struct svrFrameParams
			{
			int                 frameIndex;                             //!< Frame Index
			int                 minVsyncs;                              //!< Minimum number of vysnc events before displaying the frame (1=display refresh, 2=half refresh, etc...)
			int                 eyeBufferArray[SVR_MAX_EYE_BUFFERS];    //!< Array of eye buffer identifiers.  Number and layout dictated by the eyeBufferType
			svrEyeBufferType    eyeBufferType;                          //!< Layout for the supplied eye buffer(s)
			int                 overlayBuffer;                          //!< Overlay buffer identifier.  This buffer will be rendered on top of eye buffer
			unsigned int        frameOptions;                           //!< Options for adjusting the frame warp behavior (bitfield of svrFrameOption)
			svrHeadPoseState    headPoseState;                          //!< Head pose state used to generate the frame
			svrWarpType         warpType;                               //!< Type of warp to be used on the frame
			};
			*/
			if (m_bFrameIndexChanged && m_bInitialized && m_bSvrBegin)
			{
				svrFrameParams params;// = plugin.frameParams;
				memset(&params, 0, sizeof(svrFrameParams));

				params.frameIndex = m_iFrameIndex;
				params.minVsyncs = 1; 
				
				__tagUnityTexture TextureID = GetTextureID();
				params.eyeBufferArray[0] = TextureID.m_iLeftTextureID;
				params.eyeBufferArray[1] = TextureID.m_iRightTextureID;
				params.eyeBufferType = kEyeBufferStereoSeparate;
				// MOJING_TRACE(g_APIlogger, "FrameIndex = " << m_iFrameIndex  << " , Left Texture = " << params.eyeBufferArray[0] << " , Right Texture = " << params.eyeBufferArray[1]);
					
				/// 计算投影后实际的屏幕坐标
#if MJ_OVERLAY
 				params.overlayBuffer[0] = m_LeftOverlay.m_iTextureID;
 				params.overlayBuffer[1] = m_RightOverlay.m_iTextureID;
				__tagUnityOverlay Overlay = m_LeftOverlay;
				
				// left eye
				Rectf rcOverlay = MojingSDK_Math_OverlayPosition3D( 1, Overlay.m_Rect, Overlay.m_fDistanceInMetre);
#if (SVR_VERSION == 113)
				params.overlayType = kOverlayStereo;
				L_CreateLayout(rcOverlay.x + rcOverlay.w / 2.0f - 1.0f, rcOverlay.y - rcOverlay.h / 2.0f - 1.0f, rcOverlay.w, rcOverlay.h, &params.overlayLayout[0]);
#endif
				params.overlayRect[0] = { rcOverlay.x, rcOverlay.y, rcOverlay.w, rcOverlay.h };
				// right eye
				Overlay = m_RightOverlay;
				rcOverlay = MojingSDK_Math_OverlayPosition3D(2, Overlay.m_Rect, Overlay.m_fDistanceInMetre);
#if (SVR_VERSION == 113)
				L_CreateLayout(rcOverlay.x + rcOverlay.w / 2.0f, rcOverlay.y - rcOverlay.h / 2.0f, rcOverlay.w, rcOverlay.h, &params.overlayLayout[1]);
#endif
				params.overlayRect[1] = { rcOverlay.x, rcOverlay.y, rcOverlay.w, rcOverlay.h };
#else

#endif
				params.frameOptions = 0;
				// ????
				params.headPoseState = GetHeadPoseState();
				params.warpType = kSimple;
				m_SVRApi.SubmitFrame(&params);
				m_bFrameIndexChanged = false;
			}
		}


		bool UnityPluginInterfaceQ820::StartTracker()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			
/*			m_SVRApi.SetTrackingMode(kTrackingRotation);*/
			return true;
		}

		Matrix4f UnityPluginInterfaceQ820::GetLastHeadView()
		{
			Matrix4f Ret(GetLastHeadQuarternion());
			return Ret;
		}

		Quatf UnityPluginInterfaceQ820::GetLastHeadQuarternion()
		{
			Quatf Pos(0, 0, 0, 1);
			if (m_bInitialized && m_bSvrBegin)
			{
				float predictedTimeMs = m_SVRApi.GetPredictedDisplayTime();
				m_HeadPoseState = m_SVRApi.GetPredictedHeadPose(predictedTimeMs);
#ifdef _DEBUG
// 				double dTime = m_HeadPoseState.poseTimeStampNs / 1e9;
// 				double dTimeNow = Timer::GetSeconds();
// 				double dTimeOffset = dTimeNow - dTime;
// 				MOJING_TRACE(g_APIlogger, "Sensor time offset = "
// 					<< Timer::FormatDoubleTimeInMS(dTimeOffset));
#endif
				if (m_HeadPoseState.pose.rotation.x == 0 &&
					m_HeadPoseState.pose.rotation.y == 0 &&
					m_HeadPoseState.pose.rotation.z == 0 &&
					m_HeadPoseState.pose.rotation.w == 0)
				{
					MOJING_ERROR(g_APIlogger, "Can not get HeadPose!");
					m_HeadPoseState.pose.rotation.w = 1;
				}
				else
				{
					Pos = Quatf(-m_HeadPoseState.pose.rotation.x,
						-m_HeadPoseState.pose.rotation.y,
						-m_HeadPoseState.pose.rotation.z,
						m_HeadPoseState.pose.rotation.w);
#if 0 //def _DEBUG
				{	
					float x = Pos.x;
					float y = Pos.y;
					float z = Pos.z;
					float w = Pos.w;
#define CLAMP(x , min , max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : x))
					float roll = atan2(2 * (w * z + x * y), 1 - 2 * (z * z + x * x));
					float pitch = asin(CLAMP(2 * (w * x - y * z), -1.0f, 1.0f));
					float yaw = atan2(2 * (w * y + z * x), 1 - 2 * (x * x + y * y));
					char szLog[512];
					sprintf(szLog, "Rotation = ( %1.4f , %1.4f , %1.4f , %1.4f) , EulerAngles = (%3.4f , %3.4f , %3.4f )", 
						x,y,z,w,roll, pitch, yaw);
					MOJING_TRACE(g_APIlogger, szLog);
				}
#endif

				}
			}
			else
			{
				MOJING_ERROR(g_APIlogger, "SVR not init (" << m_bInitialized << ") or not begin(" << m_bSvrBegin << ")");

			}
			return Pos;
		}

		void UnityPluginInterfaceQ820::ResetSensorOrientation()
		{
			m_SVRApi.RecenterPose();
		}

		void UnityPluginInterfaceQ820::ResetSensorOrientation2()
		{
			m_SVRApi.RecenterPose();
		}

		void UnityPluginInterfaceQ820::ResetTracker()
		{
			m_SVRApi.RecenterPose();
		}

		void UnityPluginInterfaceQ820::StopTracker()
		{

		}

		void UnityPluginInterfaceQ820::SetTimeWarpStatus(__tagTimeWarpStatus timewarpStatus)
		{
			m_SVRApi.SetTimewarp(timewarpStatus.m_bTimeWarpType);
		}

		bool UnityPluginInterfaceQ820::GetIsInMojingWorld()const
		{
			bool bA = UnityPluginInterfaceBase::GetIsInMojingWorld();
			bool bB = GetInitialized();
			bool bC = GetSvrBegin();
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger , "Call GetIsInMojingWorld : " << bA << " , " << bB << " , " << bC );
#endif
			return bA && bB && bC;
		}
	}
}
