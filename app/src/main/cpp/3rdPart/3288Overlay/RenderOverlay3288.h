#pragma once
#include "C3288Overlay.h"
#include "../../Base/MojingThreads.h"
#include "../../Render/GlGeometry.h"
#include "../../Render/GLProgram.h"


namespace Baofeng
{
	namespace Mojing
	{
		struct StartParmeat
		{
			char szOverlayTag[32];
			unsigned int iWidth;
			unsigned int iHeight;
			unsigned int iLayer;
			GLint glVersion;
			EGLContext SharedContext;
		};


		class RenderOverlay3288 : public Thread
		{
			static C3288Overlay m_libOverlay;
			OVERLAY_HANDLE m_hOverlay;
			StartParmeat m_StartParmat;
			Event m_StartEvent;
		public:
			RenderOverlay3288();
			virtual ~RenderOverlay3288();
			void SetStartParmat(const char* szOverlayTag, unsigned int iWidth, unsigned int iHeight, unsigned int iLayer, EGLContext SharedContext, GLuint glVersion);
			unsigned int Move(float fX, float fY);
			unsigned int Resize(unsigned int iWidth, unsigned int iHeight);
			unsigned int SetVisible(bool bVisible);
			void DestroyOverlay();
			virtual int   Run();
			void SetMaxDrawFrameCount(const unsigned int uiDrawFrameCount);
			void WaitInitFinsh(){ m_StartEvent.Wait(); };
		private:
			void OnModify();
			bool InitProgram();
			void DeleteProgram();
			bool CreateOverlay(const char* szOverlayTag, unsigned int iWidth, unsigned int iHeight, unsigned int iLayer, EGLContext SharedContext, EGLint* pContexAttribute);

			CLASS_INTERFACE(unsigned int, m_ui, DrawFrameCount);
			GlGeometryQuad m_Mesh;
			CLASS_MEMBER(bool, m_b, InitSucceeded);
			CLASS_MEMBER(bool, m_b, ExitThread);
			CLASS_MEMBER(bool, m_b, Vsync);

			CLASS_INTERFACE(int, m_i, ScreenWidth);// 来自屏幕参数的数据，为了防止每一DrawCall都去读取，这里把他们记录下来
			CLASS_INTERFACE(int, m_i, ScreenHeight);// 来自屏幕参数的数据，为了防止每一DrawCall都去读取，这里把他们记录下来
			CLASS_INTERFACE(int, m_i, ScreenWidth_Half);// 来自屏幕参数的数据，为了防止每一DrawCall都去读取，这里把他们记录下来
			CLASS_INTERFACE(int, m_i, ScreenHeight_Half);// 来自屏幕参数的数据，为了防止每一DrawCall都去读取，这里把他们记录下来

			//CLASS_MEMBER(WarpProg, m_ , warpProgLayout);
			WarpProg  m_warpProgLayout;
			// 双眼纹理
			CLASS_MEMBER(int, m_i, LeftOverlayTextureID);
			CLASS_MEMBER(int, m_i, RightOverlayTextureID);
			CLASS_MEMBER(Vector4f, m_v4, LeftOverlayRect);
			CLASS_MEMBER(Vector4f, m_v4, RightOverlayRect);


		};


	}
}
