#include "MultiThreadGLParam.h"
#include "../Base/MojingThreads.h"

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
#if defined(MJ_OS_MAC)|| defined(MJ_OS_WIN32)
		extern int gettid(void);
#endif
		MultiThreadGLParam::MultiThreadGLParam() :
			m_eglDefaultDisplay(0),m_eglDisplay(0), m_eglWindowSurface(0), m_eglPBufferSurface(0), m_eglConfig(0),
			m_eglClientVersion(0), m_eglShareContext(0), m_eglWarpContext(0), m_iModelThreadID(0), m_iDistortionThreadID(0),
			m_bInitModelParam(false),m_bInitDistortionParam(false)
		{															

		}


		MultiThreadGLParam::~MultiThreadGLParam()
		{
		}

		bool MultiThreadGLParam::InitModelParam()
		{
			MOJING_FUNC_TRACE(g_APIlogger);

			m_iModelThreadID = gettid();
			// 这段是从 Oculus 直接移植过来的
			// Get values for the current OpenGL context
			m_eglDefaultDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
			if (m_eglDefaultDisplay == EGL_NO_DISPLAY)
			{
				MOJING_ERROR(g_APIlogger, "EGL_NO_DISPLAY");
				return false;
			}

			m_eglShareContext = eglGetCurrentContext();
			if (m_eglShareContext == EGL_NO_CONTEXT)
			{
				MOJING_ERROR(g_APIlogger, "EGL_NO_CONTEXT");
				return false;
			}

			EGLint configID;
			if (!eglQueryContext(m_eglDefaultDisplay, m_eglShareContext, EGL_CONFIG_ID, &configID))
			{
				MOJING_ERROR(g_APIlogger, "eglQueryContext EGL_CONFIG_ID failed");
				return false;
			}

			m_eglConfig = EglConfigForConfigID(m_eglDefaultDisplay, configID);
			if (m_eglConfig == NULL)
			{
				MOJING_ERROR(g_APIlogger, "EglConfigForConfigID failed");
				return false;
			}

#ifdef MJ_OS_ANDROID
			// 以下这段代码是为了将当前窗口渲染Surface更换成pbufferSurface
			MOJING_TRACE(g_APIlogger, "$$$ Get WindowSurface " << eglGetCurrentSurface(EGL_DRAW));
#endif
			m_eglWindowSurface = eglGetCurrentSurface(EGL_DRAW);
			m_eglDisplay = eglGetCurrentDisplay();

			EGLint eglSurfaceType = 0;
			eglGetConfigAttrib(m_eglDisplay, m_eglConfig, EGL_SURFACE_TYPE, &eglSurfaceType);
			if (eglSurfaceType & EGL_MUTABLE_RENDER_BUFFER_BIT_KHR)
			{
				m_bEnableFBR = true;
			}
			else
			{
				m_bEnableFBR = false;
			}


			if (!eglQueryContext(m_eglDisplay, m_eglShareContext,
				EGL_CONTEXT_CLIENT_VERSION, (EGLint *)&m_eglClientVersion))
			{
				MOJING_ERROR(g_APIlogger, "eglQueryContext EGL_CONTEXT_CLIENT_VERSION failed");
				return false;
			}
			if (m_eglClientVersion < 2)
			{
				MOJING_ERROR(g_APIlogger, "EGL ClientVersion " << m_eglClientVersion << " < 2");
				return false;
			}

			EGLint ST;
			EGLBoolean R;
			// 尝试使用多重采样
			R = eglGetConfigAttrib(m_eglDefaultDisplay, m_eglConfig, EGL_SAMPLES, &ST);
			if (ST == 0)
			{
				int iNewValue = 8;
				do
				{
					EGLint attribList[] =
					{
						EGL_SAMPLES, iNewValue,
						EGL_NONE
					};
					EGLint numConfigs;
					if (eglChooseConfig(m_eglDefaultDisplay, attribList, &m_eglConfig, 1, &numConfigs))
						break;
					iNewValue /= 2;
				} while (iNewValue);
			}

// 			// 尝试使用高质量纹理
// 			R = eglGetConfigAttrib(m_eglDefaultDisplay, m_eglConfig, EGL_CONTEXT_PRIORITY_LEVEL_IMG, &ST);
// 			if (!R || ST != EGL_CONTEXT_PRIORITY_HIGH_IMG)
// 			{
// 				int iNewValue = EGL_CONTEXT_PRIORITY_HIGH_IMG;
// 				do 
// 				{
// 					EGLint attribList[] =
// 					{
// 						EGL_CONTEXT_PRIORITY_LEVEL_IMG, iNewValue,
// 						EGL_NONE
// 					};
// 					EGLint numConfigs;
// 					if (eglChooseConfig(m_eglDefaultDisplay, attribList, &m_eglConfig, 1, &numConfigs))
// 						break;
// 					iNewValue++;
// 				} while (iNewValue <= EGL_CONTEXT_PRIORITY_LOW_IMG);
// 			}
#if 1
			R = eglGetConfigAttrib(m_eglDefaultDisplay, m_eglConfig, EGL_SURFACE_TYPE, &ST);
			// For 魅族4
			if (0 == (ST & EGL_PBUFFER_BIT))
			{
				EGLint attribList[] =
				{
					EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
					EGL_NONE
				};

				EGLint numConfigs;
				if (!eglChooseConfig(m_eglDefaultDisplay, attribList, &m_eglConfig, 1, &numConfigs))
				{
					MOJING_ERROR(g_APIlogger, "eglChooseConfig failed: " << EglErrorString());
					return false;
				}
			}

			// Because EGL_KHR_surfaceless_context is not widespread (Only on Tegra as of
			// September 2013), we need to create a tiny pbuffer surface to make the context
			// current.
			//
			// It is necessary to use a config with the same characteristics that the
			// context was created with, plus the pbuffer flag, or we will get an
			// EGL_BAD_MATCH error on the eglMakeCurrent() call.
			//
			// This is necessary to support 565 framebuffers, which may be important
			// for higher refresh rate displays.
			const EGLint attrib_list[] =
			{
				EGL_WIDTH, 1,
				EGL_HEIGHT, 1,
				EGL_NONE
			};
			m_eglPBufferSurface = eglCreatePbufferSurface(m_eglDefaultDisplay, m_eglConfig, attrib_list);
			if (m_eglPBufferSurface == EGL_NO_SURFACE)
			{
				MOJING_ERROR(g_APIlogger, "eglCreatePbufferSurface failed: " << EglErrorString());
				return false;
			}

			if (eglMakeCurrent(m_eglDefaultDisplay, m_eglPBufferSurface, m_eglPBufferSurface, m_eglShareContext) == EGL_FALSE)
			{
				MOJING_ERROR(g_APIlogger, "eglMakeCurrent FAILD! " << EglErrorString());
				return false;
			}
#endif
			/////////////////////////////////////////////////////////////////////
			SetInitModelParam(true);
			return true;
		}

		bool MultiThreadGLParam::InitDistortionParam()
		{
			MOJING_FUNC_TRACE(g_APIlogger);

			if (GetInitModelParam())
			{
				SetDistortionThreadID(gettid());
				EGLint contextAttribs[] =
				{
					EGL_CONTEXT_CLIENT_VERSION, m_eglClientVersion,
					EGL_NONE, EGL_NONE,
					EGL_NONE
				};

				
				m_eglWarpContext = eglCreateContext(m_eglDisplay,
					m_eglConfig,
					m_eglShareContext,
					contextAttribs);

				if (m_eglWarpContext == EGL_NO_CONTEXT)
				{
					MOJING_ERROR(g_APIlogger, "eglCreateContext failed:" << EglErrorString());
					return false;
				}
				// 使环境绑定到当前线程
				if (eglMakeCurrent(m_eglDisplay, m_eglWindowSurface,
					m_eglWindowSurface, m_eglWarpContext) == EGL_FALSE)
				{
					MOJING_ERROR(g_APIlogger, "eglMakeCurrent failed:" << EglErrorString());
					return false;
				}
				SetInitDistortionParam(true);
				return true;
			}
			return false;
		}
		void MultiThreadGLParam::ReleaseModelParam()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			// 函数执行的必要条件是 处于模型线程 且畸变线程已经退出
			if (gettid() == GetModelThreadID() && !GetInitDistortionParam())
			{
				if (m_eglDefaultDisplay != EGL_NO_DISPLAY)
				{
					if (m_eglPBufferSurface != EGL_NO_SURFACE )
					{
						if (eglGetCurrentSurface(EGL_DRAW) == m_eglPBufferSurface)
						{
							
							if (EGL_FALSE == eglMakeCurrent(m_eglDefaultDisplay, m_eglWindowSurface, m_eglWindowSurface, m_eglShareContext))
							{
								MOJING_TRACE(g_APIlogger, "Reset WindowSurface :" << EglErrorString());
							}
							else
							{
								MOJING_TRACE(g_APIlogger, "Reset WindowSurface OK");
							}
						}
						eglDestroySurface(m_eglDefaultDisplay, m_eglPBufferSurface);
						m_eglPBufferSurface = EGL_NO_SURFACE;
					}
				}
				m_eglDefaultDisplay = EGL_NO_DISPLAY;
				SetInitModelParam(false);					
				SetModelThreadID(0);
			}
		}

		void MultiThreadGLParam::ReleaseDistortionParam()
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			// 处于畸变线程已经退出
			if (gettid() == GetDistortionThreadID())
			{
				if (m_eglDefaultDisplay != EGL_NO_DISPLAY)
				{
					if (m_eglWarpContext != EGL_NO_CONTEXT)
					{
						if (eglGetCurrentSurface(EGL_DRAW) == m_eglWindowSurface)
						{
							eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
						}
						eglDestroyContext(m_eglDisplay, m_eglWarpContext);
						m_eglWarpContext = EGL_NO_CONTEXT;
					}
				}
				SetInitDistortionParam(false);
				SetDistortionThreadID(0);
			}
		}
	}
}
