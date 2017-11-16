#pragma once
#include "UnityPluginInterfaceBase.h"
#include "../../../3rdPart/Qualcomm/CSVRApi.h"
#include <jni.h>
namespace Baofeng
{
	namespace Mojing
	{
		class UnityPluginInterfaceQ820 :public UnityPluginInterfaceBase
		{
			friend class UnityPluginInterfaceBase;
		protected:
			UnityPluginInterfaceQ820();
			virtual ~UnityPluginInterfaceQ820();
			/*以下接口在OnEvent_EnterMojingWorld中调用*/
			bool OnEvent_Initialize();
			bool OnEvent_BeginVR();

			CSVRApi m_SVRApi;
			void OnEvent_UpdateWarpmesh_Q820();

			virtual bool Init(__tagUnityInterfaceInitParams *pInitParameters);
		public:
			virtual void OnEvent_EnterMojingWorld();
			virtual void OnEvent_LeaveMojingWorld();
			virtual void OnEvent_ChangeMojingWorld();
			
			virtual void OnEvent_SetTextureID();
			virtual void OnEvent_DistortFrame();
			virtual void OnEvent_DestroyMojingWorld();
			CLASS_INTERFACE(int , m_i , FrameIndex);
			
			CLASS_INTERFACE(int, m_i, LeftOverlayFBO);
			CLASS_INTERFACE(int, m_i, RightOverlayFBO)
				;
			CLASS_INTERFACE(int, m_i, SurfaceValidWaitFramesCounter);
			CLASS_INTERFACE(int, m_i, SurfaceValidWaitFrames);

			CLASS_MEMBER(bool, m_b, FrameIndexChanged);
			CLASS_MEMBER(bool, m_b, Initialized);
			CLASS_MEMBER(bool, m_b, SvrBegin);
			CLASS_MEMBER(svrHeadPoseState, m_, HeadPoseState);

			
			/*以下是对外提供的响应*/
			virtual bool IsInMojingWorld(const char * szGlassesName)
			{ 
				return GetGlassName() == szGlassesName &&
					m_bInitialized && m_bSvrBegin;// 增加对SVR状态的判断，以防止提早进入
			};

			virtual bool IsGlassesNeedDistortion(){ return true; };

			virtual bool IsGlassesNeedDistortionByName(const char * szGlassesName){ return true; };

			virtual bool StartTracker();

			virtual Matrix4f GetLastHeadView();

			virtual Quatf GetLastHeadQuarternion();

			virtual void ResetSensorOrientation();

			virtual void ResetSensorOrientation2();

			virtual void ResetTracker();

			virtual void StopTracker();

			virtual void SetTimeWarpStatus(__tagTimeWarpStatus);
			
			virtual bool CanEnterMojingWorld();
			// 增加对SVR状态的判断，以防止提早进入
			virtual bool GetIsInMojingWorld()const;
		private:
			bool CreateShader();
			JNIEnv * m_pJNIEnv;
			jobject m_Activity;
		};

	}
}

