#pragma once
#include <string>
#include "../../../Base/MojingTypes.h"
#include "../../../Base/MojingMath.h"
#include "UnityPluginStatus.h"
#define USING_PLUGIN_EVENT

using namespace std;


#ifdef MJ_OS_IOS
#include "../Unity/UnityInterface.h"
#endif

#if USING_LOCKER
#define CLASS_MEMBER_L(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	protected:\
	_TYPE_ _PREV_NAME_##_NAME_; \
	Mutex _PREV_NAME_##_NAME_##Locker;\
	public:\
	virtual void Set##_NAME_(_TYPE_ V){ Mutex::Locker _Lock(&_PREV_NAME_##_NAME_##Locker); _PREV_NAME_##_NAME_ = V; }; \
	virtual _TYPE_ Get##_NAME_(){ Mutex::Locker _Lock(&_PREV_NAME_##_NAME_##Locker); return _PREV_NAME_##_NAME_; };
#else
#define CLASS_MEMBER_L(_TYPE_ , _PREV_NAME_ , _NAME_ ) CLASS_MEMBER(_TYPE_ , _PREV_NAME_ , _NAME_ )
#endif

namespace Baofeng
{
	namespace Mojing
	{
		class UnityInterface
		{
		private:
			IUnityInterfaces * g_pUnityInterfaces;
			IUnityGraphics * g_pUnityGraphics;
		public:
			void OnEvent_UnityPluginLoad();
		};

		// 用来初始化接口实现类的数据结构
		struct __tagUnityInterfaceInitParams
		{
			__tagUnityInterfaceInitParams() :
			m_pParams(NULL),
			m_szEngineVersion("Unity"),
			m_iWidth(0),
			m_iHeight(0),
			m_fXDPI(0),
			m_fYDPI(0)
			{};
			string m_szEngineVersion;
			string m_szMerchantID;
			string m_szAppID;
			string m_szAppKey;
			string m_szChannelID;
			string m_szProfilePath; // Default = ""
			int m_iWidth;
			int m_iHeight;
			float m_fXDPI;// Default = 0
			float m_fYDPI;// Default = 0

			void * m_pParams;// Default = NULL
		};
		// 用来表示Texture的数据结构
		struct __tagUnityTexture
		{
			__tagUnityTexture():
			m_iLeftTextureID(0), m_iRightTextureID(0)
			{
                m_mtlLeftTextureID = NULL;
                m_mtlRightTextureID = NULL;
            };
			enum VF
			{
				TEXTURE_LEFT = 1,
				TEXTURE_RIGHT= 2,
				TEXTURE_ROTATION = 4,
				TEXTURE_POSITION = 8
			}m_ValueFlag;

			long m_iLeftTextureID;
			long m_iRightTextureID;
			MTLTextureRef m_mtlLeftTextureID;
			MTLTextureRef m_mtlRightTextureID;
			Quatf m_qRotation;
			Vector3f m_v3Position;
		};
		// 用来表示Overlay的数据结构
		struct __tagUnityOverlay
		{
			__tagUnityOverlay():
				m_Rect(0,0,0,0)
			{
				m_iTextureID = 0;
				m_fDistanceInMetre = 0;
                m_mtlTextureID = NULL;
            };
			long m_iTextureID;
			MTLTextureRef m_mtlTextureID;
			Rectf m_Rect;
			float m_fDistanceInMetre;
			// 重载赋值操作符为了自动判定是否有纹理变化和/或 位置变化
		};
		// 用来表示TW状态的数据结构
		struct __tagTimeWarpStatus
		{
			bool m_bTimeWarpTypeChanged;
			bool m_bTimeWarpType;
		};
		struct __tagColor4f
		{
			__tagColor4f(int R, int G, int B, int A = 1.0) :
			m_R(R), m_G(G), m_B(B), m_A(A){};
			int m_R;
			int m_G;
			int m_B;
			int m_A;
		};
		struct __tagCenterLine
		{
			__tagCenterLine(int R = 255, int G = 255, int B = 255, int A = 255, int iWidth = 6) :
			m_Color(R, G, B, A), m_iWidth(iWidth), m_bChanged(false)
			{};
			__tagColor4f m_Color;
			int m_iWidth;
			bool m_bChanged;

			// 重载赋值操作符为了自动判定是否有变化
			__tagCenterLine& operator =(__tagCenterLine Other)
			{
				if (this != &Other)
				{
					m_bChanged = m_iWidth != Other.m_iWidth || 
						m_Color.m_R != Other.m_Color.m_R ||
						m_Color.m_G != Other.m_Color.m_G ||
						m_Color.m_B != Other.m_Color.m_B ||
						m_Color.m_A != Other.m_Color.m_A;
					if (m_bChanged)
					{
						m_Color = Other.m_Color;
					}
				}
					
				return *this;
			}
		};

		class MojingRenderBase;
		class UnityPluginInterfaceBase
		{
			// 静态的成员
			static UnityPluginInterfaceBase* g_pUnityPlugin;
			
			
		protected:
			// 不允许外部构造/析构
			
			UnityPluginInterfaceBase();
			virtual ~UnityPluginInterfaceBase();
			
			// 以下函数在 OnEvent_SetTextureID 中调用
            virtual void OnEvent_CheckTimeWarpState();
            virtual void OnEvent_SetOverlay(MojingRenderBase* pRender);
            virtual void OnEvent_SetCenterLine();

            virtual bool Init(__tagUnityInterfaceInitParams *pInitParameters);
			/********************************************************************/
			/*			静态函数，用于构造和获取当前可用的插件实现类			*/
			/********************************************************************/
			static bool MojingInit(__tagUnityInterfaceInitParams *pInitParameters);
		public:
			
			static bool CreateUnityPlugin(__tagUnityInterfaceInitParams *pInitParameters);
			static UnityPluginInterfaceBase* GetUnityPluginEventObj();
		
			/*以下是Unity的事件响应*/
		public:
			// 1、OnEnable中，先调用Unity_SetMojingWorld改变GlassName和TimeWarpStatus，然后触发
			virtual void OnEvent_EnterMojingWorld();

			// 2、WaitForEndOfFrame 后调用SetTextureID，随后触发本事件
			virtual void OnEvent_SetTextureID();

			// 3、SetTextureID 后立即调用SetTextureID
			virtual void OnEvent_DistortFrame();
			
			// 4、OnDisable的时候调用
			virtual void OnEvent_LeaveMojingWorld();
			
			// 5、用户修改镜片的时候调用
			virtual void OnEvent_ChangeMojingWorld();


			// 6、Q820需要增加 OnDestroy的调用
			virtual void OnEvent_DestroyMojingWorld();

			/*以下是对外提供的响应*/
			virtual bool IsInMojingWorld(const char * szGlassesName);

			virtual bool IsGlassesNeedDistortion();

			virtual bool IsGlassesNeedDistortionByName(const char * szGlassesName);

			/*Tracker*/
			virtual bool StartTracker();

			virtual Matrix4f GetLastHeadView();

			virtual Quatf GetLastHeadQuarternion();

			virtual void ResetSensorOrientation();

			virtual void ResetSensorOrientation2();

			virtual void ResetTracker();

			virtual void StopTracker();

			virtual bool CanEnterMojingWorld();
			/*报数业务接口 默认的安卓平台不调用这两个接口*/
			virtual void AppResume(){};

			virtual void AppPause(){};

			// 今后将不再提供此接口,引擎版本应该在Init的时候一并设置
			// virtual bool SetEngineVersion(const char* lpszEngine);
		public:
			/************************************************************************/
			/* 警告：因为会运行在多线程环境下，所以成员变量对象必须要用Lock互斥访问 */
			/* 不可以在代码中直接m_XXXX！											*/
			/************************************************************************/
			CLASS_MEMBER(__tagUnityInterfaceInitParams, m_, InitParameters);
			CLASS_MEMBER(__tagUnityTexture, m_, TextureID);
			CLASS_MEMBER(__tagTimeWarpStatus, m_, TimeWarpStatus);
			CLASS_MEMBER(__tagCenterLine, m_, CenterLine);
			CLASS_MEMBER(__tagUnityOverlay, m_, LeftOverlay);
			CLASS_MEMBER(__tagUnityOverlay, m_, RightOverlay);
			CLASS_MEMBER(bool, m_b, IsInMojingWorld);
			// CLASS_INTERFACE(Sizef, m_size, ScreenSize);
			CLASS_MEMBER(int, m_i, SampleFrequence);
			CLASS_MEMBER(string, m_str, GlassName);
			CLASS_INTERFACE(string, m_str, ClassName);
			CLASS_MEMBER(string, m_str, EngineVersion);
		};


	}
}

