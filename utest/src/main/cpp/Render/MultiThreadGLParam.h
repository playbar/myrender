#pragma once
#include "../Base/BuildType.h"
#include "../Base/GlUtils.h"
namespace Baofeng
{
	namespace Mojing
	{

		class MultiThreadGLParam
		{
		public:
			MultiThreadGLParam();
#if defined(MJ_OS_ANDROID) || defined(MJ_OS_WIN32)
			EGLDisplay		m_eglDefaultDisplay;// 显示环境
			EGLDisplay		m_eglDisplay;		// 显示环境
			EGLSurface		m_eglWindowSurface; // 显示窗口Surface
			EGLSurface		m_eglPBufferSurface;// 替换窗口Surface的显卡位图Surface
			EGLConfig		m_eglConfig;		// 
			EGLint			m_eglClientVersion;	// TimeWarp can work with EGL 2.0 or 3.0
			EGLContext		m_eglShareContext;	//	共享Context，来自模型窗口
			EGLContext		m_eglWarpContext;	//	交换Context，由畸变线程创建，与m_eglShareContext共享数据？？
#endif
			virtual ~MultiThreadGLParam();

			CLASS_INTERFACE(int, m_i, ModelThreadID);
			CLASS_INTERFACE(int, m_i, DistortionThreadID);
			CLASS_INTERFACE(bool, m_b, InitModelParam);
			CLASS_INTERFACE(bool, m_b, InitDistortionParam);
			CLASS_INTERFACE(bool, m_b, EnableFBR);
			bool InitModelParam();
			void ReleaseModelParam();

			bool InitDistortionParam();
			void ReleaseDistortionParam();
		};

	}
}