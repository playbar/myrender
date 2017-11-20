#include "RenderOverlay3288.h"
#include "../../Render/MojingRenderBase.h"
namespace Baofeng
{
	namespace Mojing
	{
		extern Matrix4f GUI_Matrix4f(Vector4f v4fGUIRect);
		C3288Overlay RenderOverlay3288::m_libOverlay;

		RenderOverlay3288::RenderOverlay3288() :
			m_hOverlay(NULL),
			m_bExitThread(false),
			m_bInitSucceeded(false),
			m_iLeftOverlayTextureID(0),
			m_iRightOverlayTextureID(0),
			m_uiDrawFrameCount(0)
		{
			m_libOverlay.Init();
			memset(&m_StartParmat, 0, sizeof(m_StartParmat));
		}

		void RenderOverlay3288::SetStartParmat(const char* szOverlayTag, unsigned int iWidth, unsigned int iHeight, unsigned int iLayer, EGLContext SharedContext, GLuint glVersion)
		{
			m_StartParmat.iHeight = iHeight;
			m_StartParmat.iWidth = iWidth;
			m_StartParmat.iLayer = iLayer;
			m_StartParmat.SharedContext = SharedContext;
			m_StartParmat.glVersion = glVersion;
			strncpy(m_StartParmat.szOverlayTag, szOverlayTag, 31);
			m_StartParmat.szOverlayTag[31] = 0;
		}

		RenderOverlay3288::~RenderOverlay3288()
		{
			DestroyOverlay();
		}

		bool	RenderOverlay3288::CreateOverlay(const char* szOverlayTag, unsigned int iWidth, unsigned int iHeight, unsigned int iLayer, EGLContext SharedContext, EGLint* pContexAttribute)
		{

			m_hOverlay = m_libOverlay.CreateOverlay(szOverlayTag, iWidth, iHeight, iLayer, SharedContext, pContexAttribute);
			if (m_hOverlay && m_hOverlay->m_iErrorCode == 0)
			{
				return true;
			}
			delete m_hOverlay;
			m_hOverlay = NULL;
			return false;
		}
		unsigned int	RenderOverlay3288::Move(float fX, float fY)
		{
			if (m_hOverlay)
			{
				return m_libOverlay.MoveOverlay(m_hOverlay, fX, fY);
			}
			return -1;
		}
		unsigned int	RenderOverlay3288::Resize(unsigned int iWidth, unsigned int iHeight)
		{
			if (m_hOverlay)
			{
				return m_libOverlay.ResizeOverlay(m_hOverlay, iWidth, iHeight);
			}
			return -1;
		}
		unsigned int	RenderOverlay3288::SetVisible(bool bVisible)
		{
			if (m_hOverlay)
			{
				return m_libOverlay.SetOverlayVisible(m_hOverlay, bVisible);
			}
			return -1;
		}
		void RenderOverlay3288::DestroyOverlay()
		{
			if (m_hOverlay)
			{
				m_libOverlay.DestroyOverlay(m_hOverlay);
			}
			m_hOverlay = NULL;
		}

		void RenderOverlay3288::SetMaxDrawFrameCount(const unsigned int uiDrawFrameCount)
		{
			m_uiDrawFrameCount = m_uiDrawFrameCount < uiDrawFrameCount ? uiDrawFrameCount : m_uiDrawFrameCount;
		}
		void RenderOverlay3288::OnModify()
		{
			if (m_hOverlay)
			{
				m_iScreenWidth = m_hOverlay->m_iWidth;
				m_iScreenHeight = m_hOverlay->m_iHeight;
				m_iScreenWidth_Half = m_iScreenWidth / 2;
				m_iScreenHeight_Half = m_iScreenHeight / 2;
				m_Mesh.BuildGeometry(m_iScreenWidth, m_iScreenHeight);
			}
			else
			{
				m_iScreenWidth = 0;
				m_iScreenHeight = 0;
				m_iScreenWidth_Half = 0;
				m_iScreenHeight_Half = 0;
			}
		}

		bool RenderOverlay3288::InitProgram()
		{
			m_warpProgLayout = MojingRenderBase::BuildWarpProg(VertexShaderLayout, FragmentShaderLayoutNoDispersion);
			return glIsProgram(m_warpProgLayout.Prog.program);
		}
		void RenderOverlay3288::DeleteProgram()
		{
			MojingRenderBase::DeleteProgram(m_warpProgLayout.Prog);
		}


		int   RenderOverlay3288::Run()
		{
			EGLint contextAttribs[] =
			{
				EGL_CONTEXT_CLIENT_VERSION, m_StartParmat.glVersion,
				EGL_NONE, EGL_NONE,
				EGL_NONE
			};
			bool bInitOK = CreateOverlay(m_StartParmat.szOverlayTag, m_StartParmat.iWidth, m_StartParmat.iHeight, m_StartParmat.iLayer, m_StartParmat.SharedContext, contextAttribs);
			SetThreadName(m_StartParmat.szOverlayTag);
			if (bInitOK && m_hOverlay->m_iErrorCode == 0)
			{
				EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
				EGLBoolean b = eglMakeCurrent(dpy, m_hOverlay->m_Surface, m_hOverlay->m_Surface, m_hOverlay->m_Context);
				m_bInitSucceeded = b && InitProgram();
				m_StartEvent.SetEvent();
				/////////////////////////////////
				if (b == EGL_TRUE)
				{

					const char* pErrorString = EglErrorString();
					glDepthMask(GL_FALSE);	// don't write to depth, even if Unity has depth on window
					glDisable(GL_DEPTH_TEST);
					glDisable(GL_CULL_FACE);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glClearColor(0, 0, 0, 0);
					glClear(GL_COLOR_BUFFER_BIT);
					eglSwapBuffers(dpy, m_hOverlay->m_Surface);

					while (!GetExitFlag())
					{
						if (m_uiDrawFrameCount == 0)
						{
							usleep(50 * 1000);// sleep 50ms
							continue;
						}
						
						if (m_iScreenHeight != m_hOverlay->m_iWidth || m_iScreenWidth != m_hOverlay->m_iHeight)
						{
							OnModify();
						}
						while (!m_bVsync)
						{
							usleep(500);// sleep 0.5ms
						}
						
						--m_uiDrawFrameCount;
						bool bLeftOverlay = (GetLeftOverlayTextureID() != 0) && glIsTexture(GetLeftOverlayTextureID()) && m_v4LeftOverlayRect.w > 1e-4 && m_v4LeftOverlayRect.z > 1e-4;
						bool bRightOverlay = (GetRightOverlayTextureID() != 0) && glIsTexture(GetRightOverlayTextureID()) && m_v4RightOverlayRect.w > 1e-4 && m_v4RightOverlayRect.z > 1e-4;
						if (!bLeftOverlay && !bRightOverlay)
						{
							m_bVsync = false;
							continue;// 不需要绘制 
						}
						glClearColor(0, 0, 0, 0);
						glClear(GL_COLOR_BUFFER_BIT);
						// for test
// 						if (m_hOverlay->m_iWidth != 1920)
// 						{
// 							static int iStepX = 5;
// 							static int iStepY = 5;
// 							static int iPosX = (1920 - m_hOverlay->m_iWidth) / 2;
// 							static int iPosY = (1080 - m_hOverlay->m_iHeight) / 2;
// 							iPosX += iStepX;
// 							if (iPosX + m_hOverlay->m_iWidth > 1920)
// 							{
// 								iPosX = 1920 - m_hOverlay->m_iWidth;
// 								iStepX = -1* (rand() % 5 + 2);
// 							}
// 							else if (iPosX <0)
// 							{
// 								iPosX = 0;
// 								iStepX = (rand() % 5 + 2);
// 							}
// 							iPosY += iStepY;
// 							if (iPosY + m_hOverlay->m_iHeight > 1080)
// 							{
// 								iPosY = 1080 - m_hOverlay->m_iHeight;
// 								iStepY = -1 * (rand() % 5 + 2);
// 							}
// 							else if (iPosY < 0)
// 							{
// 								iPosY = 0;
// 								iStepY = (rand() % 5 + 2);
// 							}
// 							Move(iPosX, iPosY);
// 						}
						//end of test
						struct __tagOverlayTexture
						{
							int	  m_iTextureID;
							Matrix4f m_m4Matrix;
						};
						__tagOverlayTexture OverlayParmat[2];
						OverlayParmat[0].m_iTextureID = GetLeftOverlayTextureID();
						OverlayParmat[0].m_m4Matrix = GUI_Matrix4f(GetLeftOverlayRect());

						if (bRightOverlay)
						{
							OverlayParmat[1].m_iTextureID = GetRightOverlayTextureID();
							OverlayParmat[1].m_m4Matrix = GUI_Matrix4f(GetRightOverlayRect());
						}
						else
						{
							OverlayParmat[1] = OverlayParmat[0];
						}

						// MOJING_FUNC_TRACE(g_APIlogger);
						bool bRet = true;
						const Matrix4f timeWarps[2][2] = {
							{ OverlayParmat[0].m_m4Matrix, OverlayParmat[0].m_m4Matrix },
							{ OverlayParmat[1].m_m4Matrix, OverlayParmat[1].m_m4Matrix }
						};
						// 双眼一起绘制
						for (int iEye = 0; iEye < 2; iEye++)
						{
							static Matrix4f LandscapeOrientationMatrix = Matrix4f(
								1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f
								);
							glUseProgram(m_warpProgLayout.Prog.program);
							glUniformMatrix4fv(m_warpProgLayout.Prog.uMvp, 1, GL_FALSE, LandscapeOrientationMatrix.M[0]);

							glUniformMatrix4fv(m_warpProgLayout.Prog.uTexm, 1, GL_FALSE, OverlayParmat[iEye].m_m4Matrix.M[0]);

							if (m_warpProgLayout.Prog.uTexClipX > 0)
							{
								// split screen clamping for UE4
								Vector2f clampX(0, 1.0);
								if (0)
								{
									clampX.x = 0.5 * iEye + iEye * 0.001f;
									clampX.y = 0.5 * iEye + 0.5 - (1 - iEye) * 0.001f;
								}
								glUniform2fv(m_warpProgLayout.Prog.uTexClipX, 1, &clampX.x);
							}//	if (m_warpProgLayout.Prog.uTexClipX > 0)


							glActiveTexture(GL_TEXTURE0);
							glBindTexture(GL_TEXTURE_2D, OverlayParmat[iEye].m_iTextureID);

							// int texId0 = glGetUniformLocation(prog.program, "Texture0");
							glUniform1i(m_warpProgLayout.Prog.uTexture0, GL_TEXTURE0 - GL_TEXTURE0);

							m_Mesh.BindVBO();
							m_Mesh.DrawElements(iEye);
						}// end of iEye 
						eglSwapBuffers(dpy, m_hOverlay->m_Surface);
						m_bVsync = false;
					}// end of while
					eglMakeCurrent(eglGetCurrentDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
					DestroyOverlay();
					DeleteProgram();
				}//if (b == EGL_TRUE)
			}
			else//if (bInitOK && m_hOverlay->m_iErrorCode == 0)
			{
				m_StartEvent.SetEvent();
			}
			m_bInitSucceeded = false;
			return 1; 
		}// end of function run
		
	}
}