#pragma once
#include "MojingRenderMultithread.h"
#include "../3rdPart/3288VSync/C3288VSync.h"
#include "../3rdPart/3288Overlay/RenderOverlay3288.h"

#define  USING_OVERLAY 1
namespace Baofeng
{
	namespace Mojing
	{
		struct OverlayParmeat
		{
			OverlayParmeat():
			m_bChanged(false),
			m_iDrawCount(0),
			m_iLeftOverlayTextureID(0),
			m_iRightOverlayTextureID(0)
			{};
			CLASS_MEMBER(bool, m_b ,Changed);
			CLASS_MEMBER(int, m_i, DrawCount);
			CLASS_MEMBER(int, m_i, LeftOverlayTextureID);
			CLASS_MEMBER(int, m_i, RightOverlayTextureID);
			CLASS_MEMBER(Vector4f, m_v4, LeftOverlayRect);
			CLASS_MEMBER(Vector4f, m_v4, RightOverlayRect);
		};

		class MojingRenderMultiThread_3288 :
			public MojingRenderMultiThread
		{
		protected:
			C3288VSync m_3288VSync;
			virtual RenderFrame * SelectDistortionFrame();
			virtual void OnModifyed();
			void BindEyeTextures(GLuint TexId) override;
			bool DrawDistortion_Multiview(RenderFrame *pDistortionFrame, EyeTextureType DrawEyes);
			void SetEyeTexID(int leftEye, int rightEye) override;

			WarpProg m_warpProgMultiview;
			void InstallShader() override;
		public:
			MojingRenderMultiThread_3288();
			virtual ~MojingRenderMultiThread_3288();
			CLASS_INTERFACE(bool , m_b , SwapBuffer);
			virtual bool InitVSyncParam();
			virtual bool THREAD_InitGLThread();
			virtual bool THREAD_DoDistortion();
			virtual bool AfterDisplay(RenderFrame *pDisplayFrame);
			virtual void SetCenterLine(int iWidth, int colR, int colG, int colB, int colA);
#if USING_OVERLAY
			CLASS_INTERFACE(RenderOverlay3288 *, m_p, RedrawAIM);
			CLASS_INTERFACE(RenderOverlay3288 *, m_p, RedrawOSD);
			CLASS_MEMBER(OverlayParmeat, m_, RedrawAIMParmeat);
			CLASS_MEMBER(OverlayParmeat, m_, RedrawOSDParmeat);
#endif 
		private:
			GLuint texturedQuadProgram;
			GLuint texturedQuadVertexLocation;
			GLuint texturedQuadLowResTexCoordLocation;
			GLuint texturedQuadHighResTexCoordLocation;
			GLuint texturedQuadSamplerLocation;
			GLuint texturedQuadLayerIndexLocation;
		};
	}
}
