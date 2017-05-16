#include "GlUtils.h"
#include <string.h>
#include <stdio.h>
#include "MojingTimer.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

// TODO: remove when new glext is available
#ifdef MJ_OS_ANDROID
#ifndef EGL_OPENGL_ES3_BIT_KHR
#define EGL_OPENGL_ES3_BIT_KHR      0x0040
#endif

bool	EXT_discard_framebuffer;
PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXT_ = NULL;

bool	IMG_multisampled_render_to_texture;
#if !defined(__aarch64__)
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMG glRenderbufferStorageMultisampleIMG_ = NULL;
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMG glFramebufferTexture2DMultisampleIMG_ = NULL;
#endif

PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR_ = NULL;
PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR_ = NULL;
PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR_ = NULL;
PFNEGLSIGNALSYNCKHRPROC eglSignalSyncKHR_ = NULL;
PFNEGLGETSYNCATTRIBKHRPROC eglGetSyncAttribKHR_ = NULL;

// EGL_KHR_image_base
PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR_ = NULL;
PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR_ = NULL;

PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES_ = NULL;

// GL_OES_vertex_array_object
bool	OES_vertex_array_object;
PFNGLBINDVERTEXARRAYOESPROC	glBindVertexArrayOES_ = NULL;
PFNGLDELETEVERTEXARRAYSOESPROC	glDeleteVertexArraysOES_ = NULL;
PFNGLGENVERTEXARRAYSOESPROC	glGenVertexArraysOES_ = NULL;
PFNGLISVERTEXARRAYOESPROC	glIsVertexArrayOES_ = NULL;

// QCOM_tiled_rendering
bool	QCOM_tiled_rendering;
PFNGLSTARTTILINGQCOMPROC	glStartTilingQCOM_ = NULL;
PFNGLENDTILINGQCOMPROC		glEndTilingQCOM_ = NULL;

// EXT_disjoint_timer_query
bool	EXT_disjoint_timer_query;

PFNGLGENQUERIESEXTPROC glGenQueriesEXT_ = NULL;
PFNGLDELETEQUERIESEXTPROC glDeleteQueriesEXT_ = NULL;
PFNGLISQUERYEXTPROC glIsQueryEXT_ = NULL;
PFNGLBEGINQUERYEXTPROC glBeginQueryEXT_ = NULL;
PFNGLENDQUERYEXTPROC glEndQueryEXT_ = NULL;
PFNGLQUERYCOUNTEREXTPROC glQueryCounterEXT_ = NULL;
PFNGLGETQUERYIVEXTPROC glGetQueryivEXT_ = NULL;
PFNGLGETQUERYOBJECTIVEXTPROC glGetQueryObjectivEXT_ = NULL;
PFNGLGETQUERYOBJECTUIVEXTPROC glGetQueryObjectuivEXT_ = NULL;
PFNGLGETQUERYOBJECTI64VEXTPROC glGetQueryObjecti64vEXT_ = NULL;
PFNGLGETQUERYOBJECTUI64VEXTPROC glGetQueryObjectui64vEXT_ = NULL;
PFNGLGETINTEGER64VPROC glGetInteger64v_ = NULL;

// EXT_sRGB_decode
bool HasEXT_sRGB_texture_decode = false;

// glFramebufferTextureMultiviewOVR
bool HasEXT_multiview_ovr = false;
PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR_ glFramebufferTextureMultiviewOVR_ = NULL;

// ES3 replacements to allow linking against ES2 libs
void(*glBlitFramebuffer_) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) = NULL;
void(*glRenderbufferStorageMultisample_) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) = NULL;
void(*glInvalidateFramebuffer_) (GLenum target, GLsizei numAttachments, const GLenum* attachments) = NULL;
GLvoid*        (*glMapBufferRange_) (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access) = NULL;
GLboolean(*glUnmapBuffer_) (GLenum target);


/*
* OpenGL convenience functions
*/
namespace Baofeng
{

	namespace Mojing
	{

		/*double TimeInSeconds();*/

		static void LogStringWords(const char * allExtensions)
		{
			const char * start = allExtensions;
			MOJING_TRACE(g_APIlogger, "Get all GLExtensions: ");
			while (1)
			{
				const char * end = strstr(start, " ");
				if (end == NULL)
				{
					break;
				}
				unsigned int nameLen = (unsigned int)(end - start);
				if (nameLen > 256)
				{
					nameLen = 256;
				}
				char * word = new char[nameLen + 1];
				memcpy(word, start, nameLen);
				word[nameLen] = '\0';
				//LOG("%s", word);
				MOJING_TRACE(g_APIlogger, "     " << word);
				delete[] word;

				start = end + 1;
			}


			
		}

		// This requires the isFBO parameter because GL ES 3.0's glInvalidateFramebuffer() uses
		// different attachment values for FBO vs default framebuffer, unlike glDiscardFramebufferEXT()
		void GL_InvalidateFramebuffer(const invalidateTarget_t isFBO, const bool colorBuffer, const bool depthBuffer)
		{
			const int offset = (int)!colorBuffer;
			const int count = (int)colorBuffer + ((int)depthBuffer) * 2;

			const GLenum fboAttachments[3] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
			const GLenum attachments[3] = { GL_COLOR_EXT, GL_DEPTH_EXT, GL_STENCIL_EXT };
			if (glInvalidateFramebuffer_)
				glInvalidateFramebuffer_(GL_FRAMEBUFFER, count, (isFBO == INV_FBO ? fboAttachments : attachments) + offset);
		}

		void GL_FindExtensions()
		{
			// get extension pointers
			const char * extensions = (const char *)glGetString(GL_EXTENSIONS);
			if (NULL == extensions)
			{
				//LOG("glGetString( GL_EXTENSIONS ) returned NULL");
				return;
			}

			// Unfortunately, the Android log truncates strings over 1024 bytes long,
			// even if there are \n inside, so log each word in the string separately.
			//LOG("GL_EXTENSIONS:");
			LogStringWords(extensions);

			const bool es3 = (strncmp((const char *)glGetString(GL_VERSION), "OpenGL ES 3", 11) == 0);
			//LOG("es3 = %s", es3 ? "TRUE" : "FALSE");

			if (ExtensionStringPresent("GL_EXT_discard_framebuffer", extensions))
			{
				EXT_discard_framebuffer = true;
				glDiscardFramebufferEXT_ = (PFNGLDISCARDFRAMEBUFFEREXTPROC)GetExtensionProc("glDiscardFramebufferEXT");
			}

			if (ExtensionStringPresent("GL_IMG_multisampled_render_to_texture", extensions))
			{
				IMG_multisampled_render_to_texture = true;
#if !defined(__aarch64__)
				glRenderbufferStorageMultisampleIMG_ = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMG)GetExtensionProc("glRenderbufferStorageMultisampleIMG");
				glFramebufferTexture2DMultisampleIMG_ = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMG)GetExtensionProc("glFramebufferTexture2DMultisampleIMG");
#endif
			}
			else if (ExtensionStringPresent("GL_EXT_multisampled_render_to_texture", extensions))
			{
				// assign to the same function pointers as the IMG extension
				IMG_multisampled_render_to_texture = true;
#if !defined(__aarch64__)
				glRenderbufferStorageMultisampleIMG_ = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMG)GetExtensionProc("glRenderbufferStorageMultisampleEXT");
				glFramebufferTexture2DMultisampleIMG_ = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMG)GetExtensionProc("glFramebufferTexture2DMultisampleEXT");
#endif
			}

			eglCreateSyncKHR_ = (PFNEGLCREATESYNCKHRPROC)GetExtensionProc("eglCreateSyncKHR");
			eglDestroySyncKHR_ = (PFNEGLDESTROYSYNCKHRPROC)GetExtensionProc("eglDestroySyncKHR");
			eglClientWaitSyncKHR_ = (PFNEGLCLIENTWAITSYNCKHRPROC)GetExtensionProc("eglClientWaitSyncKHR");
			eglSignalSyncKHR_ = (PFNEGLSIGNALSYNCKHRPROC)GetExtensionProc("eglSignalSyncKHR");
			eglGetSyncAttribKHR_ = (PFNEGLGETSYNCATTRIBKHRPROC)GetExtensionProc("eglGetSyncAttribKHR");

			eglCreateImageKHR_ = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
			eglDestroyImageKHR_ = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
			glEGLImageTargetTexture2DOES_ = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress("glEGLImageTargetTexture2DOES");

			if (ExtensionStringPresent("GL_OES_vertex_array_object", extensions))
			{
				OES_vertex_array_object = true;
				glBindVertexArrayOES_ = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
				glDeleteVertexArraysOES_ = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
				glGenVertexArraysOES_ = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
				glIsVertexArrayOES_ = (PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress("glIsVertexArrayOES");
			}

			if (ExtensionStringPresent("GL_QCOM_tiled_rendering", extensions))
			{
				QCOM_tiled_rendering = true;
				glStartTilingQCOM_ = (PFNGLSTARTTILINGQCOMPROC)eglGetProcAddress("glStartTilingQCOM");
				glEndTilingQCOM_ = (PFNGLENDTILINGQCOMPROC)eglGetProcAddress("glEndTilingQCOM");
			}

			// Enabling this seems to cause strange problems in Unity
			if (ExtensionStringPresent("GL_EXT_disjoint_timer_query", extensions))
			{
				EXT_disjoint_timer_query = true;
				glGenQueriesEXT_ = (PFNGLGENQUERIESEXTPROC)eglGetProcAddress("glGenQueriesEXT");
				glDeleteQueriesEXT_ = (PFNGLDELETEQUERIESEXTPROC)eglGetProcAddress("glDeleteQueriesEXT");
				glIsQueryEXT_ = (PFNGLISQUERYEXTPROC)eglGetProcAddress("glIsQueryEXT");
				glBeginQueryEXT_ = (PFNGLBEGINQUERYEXTPROC)eglGetProcAddress("glBeginQueryEXT");
				glEndQueryEXT_ = (PFNGLENDQUERYEXTPROC)eglGetProcAddress("glEndQueryEXT");
				glQueryCounterEXT_ = (PFNGLQUERYCOUNTEREXTPROC)eglGetProcAddress("glQueryCounterEXT");
				glGetQueryivEXT_ = (PFNGLGETQUERYIVEXTPROC)eglGetProcAddress("glGetQueryivEXT");
				glGetQueryObjectivEXT_ = (PFNGLGETQUERYOBJECTIVEXTPROC)eglGetProcAddress("glGetQueryObjectivEXT");
				glGetQueryObjectuivEXT_ = (PFNGLGETQUERYOBJECTUIVEXTPROC)eglGetProcAddress("glGetQueryObjectuivEXT");
				glGetQueryObjecti64vEXT_ = (PFNGLGETQUERYOBJECTI64VEXTPROC)eglGetProcAddress("glGetQueryObjecti64vEXT");
				glGetQueryObjectui64vEXT_ = (PFNGLGETQUERYOBJECTUI64VEXTPROC)eglGetProcAddress("glGetQueryObjectui64vEXT");
				glGetInteger64v_ = (PFNGLGETINTEGER64VPROC)eglGetProcAddress("glGetInteger64v");
			}

			if (ExtensionStringPresent("GL_EXT_texture_sRGB_decode", extensions))
			{
				HasEXT_sRGB_texture_decode = true;
			}

			// Multiview
			if (ExtensionStringPresent("GL_OVR_multiview", extensions))
			{
				HasEXT_multiview_ovr = false;
				glFramebufferTextureMultiviewOVR_ = (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVR_)eglGetProcAddress("glFramebufferTextureMultiviewOVR");
			}

			GLint MaxTextureSize = 0;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MaxTextureSize);
			//LOG("GL_MAX_TEXTURE_SIZE = %d", MaxTextureSize);

			GLint MaxVertexUniformVectors = 0;
			glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &MaxVertexUniformVectors);
			//LOG("GL_MAX_VERTEX_UNIFORM_VECTORS = %d", MaxVertexUniformVectors);

			GLint MaxFragmentUniformVectors = 0;
			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &MaxFragmentUniformVectors);
			//LOG("GL_MAX_FRAGMENT_UNIFORM_VECTORS = %d", MaxFragmentUniformVectors);

			// ES3 functions we need to getprocaddress to allow linking against ES2 lib
			glBlitFramebuffer_ = (PFNGLBLITFRAMEBUFFER_)eglGetProcAddress("glBlitFramebuffer");
			glRenderbufferStorageMultisample_ = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLE_)eglGetProcAddress("glRenderbufferStorageMultisample");
			glInvalidateFramebuffer_ = (PFNGLINVALIDATEFRAMEBUFFER_)eglGetProcAddress("glInvalidateFramebuffer");
			glMapBufferRange_ = (PFNGLMAPBUFFERRANGE_)eglGetProcAddress("glMapBufferRange");
			glUnmapBuffer_ = (PFNGLUNMAPBUFFEROESPROC_)eglGetProcAddress("glUnmapBuffer");
		}

		bool EGL_IsInit()
		{
			return eglClientWaitSyncKHR_ != NULL &&
				eglDestroySyncKHR_ != NULL &&
				eglCreateSyncKHR_ != NULL;
		}

		EGLSyncKHR GL_AddSync()
		{
			// if extension not present, return NO_SYNC
			if (eglCreateSyncKHR_ == NULL)
			{
				return EGL_NO_SYNC_KHR;
			}

			const EGLSyncKHR sync = eglCreateSyncKHR_(eglGetCurrentDisplay(), EGL_SYNC_FENCE_KHR,
				NULL);
			if (sync == EGL_NO_SYNC_KHR)
			{
				//LOG("GL_AddSync():EGL_NO_SYNC_KHR");
			}
			return sync;
		}

		void GL_DestroySync(EGLSyncKHR sync)
		{
			if (sync != EGL_NO_SYNC_KHR)
			{
				eglDestroySyncKHR_(eglGetCurrentDisplay(), sync);
			}
		}

		void GL_WaitSync(EGLSyncKHR sync)
		{
			EGLDisplay eglDisplay = eglGetCurrentDisplay();
			const EGLint wait = eglClientWaitSyncKHR_(eglDisplay, sync,
				EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, 100000000); // 100 milliseconds == 100000000 nanoseconds
			if (wait == EGL_TIMEOUT_EXPIRED_KHR)
			{
				//LOG("EGL_TIMEOUT_EXPIRED_KHR");
			}
			if (wait == EGL_FALSE)
			{
				//LOG("eglClientWaitSyncKHR returned EGL_FALSE");
			}

			GL_DestroySync(sync);
		}

		void GL_Enable(const GLenum feature, GLboolean enabled)
		{
			if (enabled)
			{
				glEnable(feature);
			}
			else
			{
				glDisable(feature);
			}
		}

		void GL_Finish()
		{
			// Given the common driver "optimization" of ignoring glFinish, we
			// can't run reliably while drawing to the front buffer without
			// the Sync extension.
			if (eglCreateSyncKHR_ != NULL) {
				EGLDisplay eglDisplay = eglGetCurrentDisplay();

				const EGLSyncKHR sync = GL_AddSync();

				const EGLint wait = eglClientWaitSyncKHR_(eglDisplay, sync,
					EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, 100000000); // 100 milliseconds == 100000000 nanoseconds
				if (wait == EGL_TIMEOUT_EXPIRED_KHR) {
					//LOG("EGL_TIMEOUT_EXPIRED_KHR");
				}
				if (wait == EGL_FALSE) {
					//LOG("eglClientWaitSyncKHR returned EGL_FALSE");
				}

				GL_DestroySync(sync);
			}
		}

		void GL_Flush()
		{
			if (eglCreateSyncKHR_ != NULL)
			{
				const EGLSyncKHR sync = GL_AddSync();

				const EGLint wait = eglClientWaitSyncKHR_(eglGetCurrentDisplay(), sync,
					EGL_SYNC_FLUSH_COMMANDS_BIT_KHR, 0);
				if (wait == EGL_FALSE)
				{
					//LOG("eglClientWaitSyncKHR returned EGL_FALSE");
				}

				GL_DestroySync(sync);
			}

			// Also do a glFlush() so it shows up in logging tools that
			// don't capture eglClientWaitSyncKHR_ calls.
			//	glFlush();
		}

		void GL_TimedFinish(const char * title)
		{
			const double start = Timer::GetSeconds();// TimeInSeconds(); by 黄欣
			GL_Finish();
			const double after = Timer::GetSeconds();//TimeInSeconds(); by 黄欣
			//LOG("GL_TimedFinish( %s ): %5.3f seconds", title, after - start);
		}

		void GL_TimedFlush(const char * title)
		{
			const double start = Timer::GetSeconds();// TimeInSeconds(); by 黄欣
			GL_Flush();
			const double after = Timer::GetSeconds();// TimeInSeconds(); by 黄欣
			//LOG("GL_TimedFlush( %s ): %5.3f seconds", title, after - start);
		}


		const char * EglEnumString(const EGLint e)
		{
			switch (e)
			{
			case EGL_TIMEOUT_EXPIRED_KHR: return "EGL_TIMEOUT_EXPIRED_KHR";
			case EGL_CONDITION_SATISFIED_KHR: return "EGL_CONDITION_SATISFIED_KHR";
			case EGL_FALSE: return "EGL_FALSE";
			}
			return "<UNKNOWN EGL ENUM>";
		}

		void DumpEglConfigs(const EGLDisplay display)
		{
			static const int MAX_CONFIGS = 1024;
			EGLConfig 	configs[MAX_CONFIGS];
			EGLint  	numConfigs = 0;

			if (EGL_FALSE == eglGetConfigs(display,
				configs, MAX_CONFIGS, &numConfigs))
			{
				//LOG("eglGetConfigs() failed");
				return;
			}

			//LOG("ES2 configs:");
			//LOG("  Config R G B A DP S M W P REND");
			for (int i = 0; i < numConfigs; i++)
			{
				EGLint	red = 0;
				eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &red);
				EGLint	green = 0;
				eglGetConfigAttrib(display, configs[i], EGL_GREEN_SIZE, &green);
				EGLint	blue = 0;
				eglGetConfigAttrib(display, configs[i], EGL_BLUE_SIZE, &blue);
				EGLint	alpha = 0;
				eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &alpha);
				EGLint	depth = 0;
				eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &depth);
				EGLint	stencil = 0;
				eglGetConfigAttrib(display, configs[i], EGL_STENCIL_SIZE, &stencil);
				EGLint	multisamples = 0;
				eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &multisamples);

				// EGL_SURFACE_TYPE is a bit field
				EGLint	surface = 0;
				eglGetConfigAttrib(display, configs[i], EGL_SURFACE_TYPE, &surface);
				EGLint window = (surface & EGL_WINDOW_BIT) != 0;
				EGLint pbuffer = (surface & EGL_PBUFFER_BIT) != 0;

				// EGL_RENDERABLE_TYPE is a bit field
				EGLint	renderable = 0;
				eglGetConfigAttrib(display, configs[i], EGL_RENDERABLE_TYPE, &renderable);
				//		EGLint window = (surface & EGL_WINDOW_BIT) != 0;
				//		EGLint pbuffer = (surface & EGL_PBUFFER_BIT) != 0;

				/*
				LOG("%8i %i %i %i %i %2i %i %i %i %i 0x%02x 0x%02x", (int)configs[i], red, green, blue, alpha,
				depth, stencil, multisamples, window, pbuffer, renderable, surface);
				*/
			}
		}

		EGLConfig EglConfigForConfigID(const EGLDisplay display, const GLint configID)
		{
			static const int MAX_CONFIGS = 1024;
			EGLConfig 	configs[MAX_CONFIGS];
			EGLint  	numConfigs = 0;

			if (EGL_FALSE == eglGetConfigs(display,
				configs, MAX_CONFIGS, &numConfigs))
			{
				//LOG("eglGetConfigs() failed");
				return NULL;
			}

			for (int i = 0; i < numConfigs; i++)
			{
				EGLint	value = 0;

				eglGetConfigAttrib(display, configs[i], EGL_CONFIG_ID, &value);
				if (value == configID)
				{
					return configs[i];
				}
			}

			return NULL;
		}

		// Returns NULL if no config is found.
		EGLConfig ChooseColorConfig(const EGLDisplay display, const int redBits,
			const int greeBits, const int blueBits, const int depthBits, const int samples, const bool pbuffer)
		{

			// DumpEglConfigs( display );

			// We do NOT want to use eglChooseConfig, because the Android EGL code pushes in
			// multisample flags behind our back if the user has selected the "force 4x MSAA"
			// option in settings, and that is completely wasted for our warp target.
			static const int MAX_CONFIGS = 1024;
			EGLConfig 	configs[MAX_CONFIGS];
			EGLint  	numConfigs = 0;

			if (EGL_FALSE == eglGetConfigs(display,
				configs, MAX_CONFIGS, &numConfigs)) {
				//LOG("eglGetConfigs() failed");
				return NULL;
			}
			//LOG("eglGetConfigs() = %i configs", numConfigs);

			// We don't want a depth/stencil buffer
			const EGLint configAttribs[] = {
				EGL_BLUE_SIZE, blueBits,
				EGL_GREEN_SIZE, greeBits,
				EGL_RED_SIZE, redBits,
				EGL_DEPTH_SIZE, depthBits,
				//			EGL_STENCIL_SIZE,  	0,
				EGL_SAMPLES, samples,
				EGL_NONE
			};

			// look for OpenGL ES 3.0 configs first, then fall back to 2.0
			for (int esVersion = 3; esVersion >= 2; esVersion--)
			{
				for (int i = 0; i < numConfigs; i++)
				{
					EGLint	value = 0;

					// EGL_RENDERABLE_TYPE is a bit field
					eglGetConfigAttrib(display, configs[i], EGL_RENDERABLE_TYPE, &value);

					if ((esVersion == 2) && (value & EGL_OPENGL_ES2_BIT) != EGL_OPENGL_ES2_BIT)
					{
						continue;
					}
					if ((esVersion == 3) && (value & EGL_OPENGL_ES3_BIT_KHR) != EGL_OPENGL_ES3_BIT_KHR)
					{
						continue;
					}

					// For our purposes, the pbuffer config also needs to be compatible with
					// normal window rendering so it can share textures with the window context.
					// I am unsure if it would be portable to always request EGL_PBUFFER_BIT, so
					// I only do it on request.
					eglGetConfigAttrib(display, configs[i], EGL_SURFACE_TYPE, &value);
					const int surfs = EGL_WINDOW_BIT | (pbuffer ? EGL_PBUFFER_BIT : 0);
					if ((value & surfs) != surfs)
					{
						continue;
					}
#ifdef _DEBUG
					char szLog[512];
					sprintf(szLog, "SURFACE_TYPE : 0x%04X (Version = ES %d) , ", value, esVersion);
					MOJING_TRACE(g_APIlogger, szLog);
#endif
					int	j = 0;
					for (; configAttribs[j] != EGL_NONE; j += 2)
					{
						EGLint	value = 0;
						eglGetConfigAttrib(display, configs[i], configAttribs[j], &value);
						if (value != configAttribs[j + 1])
						{
							break;
						}
					}
					if (configAttribs[j] == EGL_NONE)
					{
						//LOG("Got an ES %i renderable config: %i", esVersion, (int)configs[i]);
						return configs[i];
					}
				}
			}
			return NULL;
		}

		const char * EglErrorString()
		{
			const EGLint err = eglGetError();
			switch (err)
			{
			case EGL_SUCCESS:			return "EGL_SUCCESS";
			case EGL_NOT_INITIALIZED:	return "EGL_NOT_INITIALIZED";
			case EGL_BAD_ACCESS:		return "EGL_BAD_ACCESS";
			case EGL_BAD_ALLOC:			return "EGL_BAD_ALLOC";
			case EGL_BAD_ATTRIBUTE:		return "EGL_BAD_ATTRIBUTE";
			case EGL_BAD_CONTEXT:		return "EGL_BAD_CONTEXT";
			case EGL_BAD_CONFIG:		return "EGL_BAD_CONFIG";
			case EGL_BAD_CURRENT_SURFACE:return "EGL_BAD_CURRENT_SURFACE";
			case EGL_BAD_DISPLAY:		return "EGL_BAD_DISPLAY";
			case EGL_BAD_SURFACE:		return "EGL_BAD_SURFACE";
			case EGL_BAD_MATCH:			return "EGL_BAD_MATCH";
			case EGL_BAD_PARAMETER:		return "EGL_BAD_PARAMETER";
			case EGL_BAD_NATIVE_PIXMAP:	return "EGL_BAD_NATIVE_PIXMAP";
			case EGL_BAD_NATIVE_WINDOW:	return "EGL_BAD_NATIVE_WINDOW";
			case EGL_CONTEXT_LOST:		return "EGL_CONTEXT_LOST";
			default: return "Unknown egl error code";
			}
		}

		const char * GL_ErrorForEnum(const GLenum e)
		{
			switch (e)
			{
			case GL_NO_ERROR: return "GL_NO_ERROR";
			case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
			case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
			case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
			case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
			case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
			default: return "Unknown gl error code";
			}
		}

		bool GL_CheckErrors(const char * logTitle)
		{
			bool hadError = false;

			// There can be multiple errors that need reporting.
			do
			{
				GLenum err = glGetError();
				if (err == GL_NO_ERROR)
				{
					break;
				}
				hadError = true;
				MOJING_ERROR(g_APIlogger, logTitle << " GL Error:" << GL_ErrorForEnum(err));
				if (err == GL_OUT_OF_MEMORY)
				{
					//FAIL"GL_OUT_OF_MEMORY");
				}
			} while (1);
			return hadError;
		}

		void *GetExtensionProc(const char * name)
		{
			void * ptr = (void *)eglGetProcAddress(name);
			if (!ptr)
			{
				//LOG("NOT FOUND: %s", name);
			}
			return ptr;
		}

		bool ExtensionStringPresent(const char * extension, const char * allExtensions)
		{
			if (strstr(allExtensions, extension))
			{
				//LOG("Found: %s", extension);
				return true;
			}
			else
			{
				//LOG("Not found: %s", extension);
				return false;
			}
		}

		GpuType EglGetGpuTypeLocal()
		{

			GpuType gpuType;
			const char * glRendererString = (const char *)glGetString(GL_RENDERER);
			if (strstr(glRendererString, "Adreno (TM) 420"))
			{
				gpuType = GPU_TYPE_ADRENO_420;
			}
			else if (strstr(glRendererString, "Adreno (TM) 330"))
			{
				gpuType = GPU_TYPE_ADRENO_330;
			}
			else if (strstr(glRendererString, "Adreno"))
			{
				gpuType = GPU_TYPE_ADRENO;
			}
			else if (strstr(glRendererString, "Mali-T760"))
			{
				gpuType = GPU_TYPE_MALI_T760;
			}
			else if (strstr(glRendererString, "Mali"))
			{
				gpuType = GPU_TYPE_MALI;
			}
			else
			{
				gpuType = GPU_TYPE_UNKNOWN;
			}

			//LOG("EglGetGpuType: %d", gpuType);

			return gpuType;
		}

		GpuType EglGetGpuType()
		{
			static GpuType type = EglGetGpuTypeLocal();
			return type;
		}

		eglSetup_t EglSetup(const EGLContext shareContext,
			const int requestedGlEsVersion,
			const int redBits, const int greenBits, const int blueBits,
			const int depthBits, const int multisamples, const GLuint contextPriority ,
			char* outEGLInfo , char * outGLESInfo, char * outGpuName )
		{
			MOJING_FUNC_TRACE(g_APIlogger);
			
			if (outEGLInfo)
				*outEGLInfo = 0;
			if (outGLESInfo)
				*outGLESInfo = 0;
			if (outGpuName)
				*outGpuName = 0;
			/*LOG("EglSetup: requestGlEsVersion(%d), redBits(%d), greenBits(%d), blueBits(%d), depthBits(%d), multisamples(%d), contextPriority(%d)",
			requestedGlEsVersion, redBits, greenBits, blueBits, depthBits, multisamples, contextPriority);*/

			eglSetup_t egl = {};

			// Get the built in display
			// TODO: check for external HDMI displays
			egl.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
			


			if (EGL_NO_DISPLAY != egl.display)
			{
				// Initialize EGL
				EGLint majorVersion;
				EGLint minorVersion;
				eglInitialize(egl.display, &majorVersion, &minorVersion);
				// 			if (outEGLInfo)
				// 			{
				// 				sprintf(outEGLInfo + strlen(outEGLInfo), "\"majorVersion\":%d,\n", majorVersion);
				// 				sprintf(outEGLInfo + strlen(outEGLInfo), "\"minorVersion\":%d,\n", minorVersion);
				// 			}
				//LOG("eglInitialize gives majorVersion %i, minorVersion %i", majorVersion, minorVersion);
#ifdef _DEBUG
				ChooseColorConfig(egl.display, redBits, greenBits, blueBits, depthBits, multisamples , false);
#endif

				const char * eglVendorString = eglQueryString(egl.display, EGL_VENDOR);
				//LOG("EGL_VENDOR: %s", eglVendorString);
				const char * eglClientApisString = eglQueryString(egl.display, EGL_CLIENT_APIS);
				//LOG("EGL_CLIENT_APIS: %s", eglClientApisString);
				const char * eglVersionString = eglQueryString(egl.display, EGL_VERSION);
				//MOJING_TRACE(g_APIlogger, "EGL_VERSION: " << eglVersionString);
				const char * eglExtensionString = eglQueryString(egl.display, EGL_EXTENSIONS);

				if (outEGLInfo)
				{
					strncpy(outEGLInfo, eglVersionString, EGL_INFO_BUFFER_LENGTH - 1);
					outEGLInfo[EGL_INFO_BUFFER_LENGTH - 1] = 0;
				}
			}
			else
			{
				if (outEGLInfo)
				{
					strcpy(outEGLInfo, "UNKNOWN , INITIALIZE EGL FAILD.");
				}
			}
			const char * glVendorString = (const char *)glGetString(GL_VENDOR);
			//LOG("GL_VENDOR: %s", glVendorString);
			const char * glRendererString = (const char *)glGetString(GL_RENDERER);
			//MOJING_TRACE(g_APIlogger, "GL_RENDERER: " << glRendererString);
			if (outGpuName && glRendererString)
			{
				if (glRendererString)
				{
					strncpy(outGpuName, glRendererString, EGL_INFO_BUFFER_LENGTH - 1);
					outGpuName[EGL_INFO_BUFFER_LENGTH - 1] = 0;
				}
				else
					strcpy(outGpuName, "NULL");
			}

			const char * glVersionString = (const char *)glGetString(GL_VERSION);
			//LOGE("GL_VERSION: %s", glVersionString);
			const char * glSlVersionString = (const char *)glGetString(
				GL_SHADING_LANGUAGE_VERSION);
			size_t lenGlVersionStr = strlen(glVersionString);
			size_t lenSlVersionStr = strlen(glSlVersionString);

			//MOJING_TRACE(g_APIlogger, "GL_VERSION: " << glVersionString);
			//MOJING_TRACE(g_APIlogger, "GL_SHADING_LANGUAGE_VERSION: " << glSlVersionString);
			if (outGLESInfo && glVersionString && glSlVersionString && ((lenGlVersionStr + lenSlVersionStr + 1) < EGL_INFO_BUFFER_LENGTH))
			{
				strncpy(outGLESInfo, glVersionString, lenGlVersionStr);
				outGLESInfo[lenGlVersionStr] = '\n';
				strncpy(outGLESInfo+lenGlVersionStr+1, glSlVersionString, lenSlVersionStr);
				outGLESInfo[EGL_INFO_BUFFER_LENGTH - 1] = 0;

				MOJING_TRACE(g_APIlogger, outGLESInfo);
			}
			GL_FindExtensions();
			return egl;
		}

		void EglShutdown(eglSetup_t & eglr)
		{
			if (eglMakeCurrent(eglr.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE)
			{
				//FAIL"eglMakeCurrent: failed: %s", EglErrorString());
			}
			if (eglDestroyContext(eglr.display, eglr.context) == EGL_FALSE)
			{
				//FAIL"eglDestroyContext: failed: %s", EglErrorString());
			}
			if (eglDestroySurface(eglr.display, eglr.pbufferSurface) == EGL_FALSE)
			{
				//FAIL"eglDestroySurface: failed: %s", EglErrorString());
			}

			eglr.glEsVersion = 0;
			eglr.gpuType = GPU_TYPE_UNKNOWN;
			eglr.display = 0;
			eglr.pbufferSurface = 0;
			eglr.config = 0;
			eglr.context = 0;
		}
	}
}
#endif