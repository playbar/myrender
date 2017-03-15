#pragma once
#include <vector>
#include <map>
#include <list>
#include "../Base/GlUtils.h"
#include "../Base/MojingTypes.h"
#include "../Base/MojingMath.h"
#include "../Base/MojingThreads.h"
#include "LineDraw.h"
#include "GLProgram.h"
#include "GlGeometry.h"
#include "RenderFrameManager.h"

#define MOTION_TO_PHOTON_TEST 0

namespace Baofeng
{
	namespace Mojing
	{
		struct EyeTextureParam
		{
			GLuint m_ID;
			EyeTextureType m_Type;
			GLuint m_Width;
			GLuint m_Heigth;
			GLenum m_Format;
		};

		class MojingRenderBase
		{
		protected:
			GlGeometry* m_warpLayer;
			GlGeometry* m_warpMesh;
			WarpProg m_warpProg;
			WarpProg m_warpProgLayout;
			WarpProg m_warpProgDistortionRange;

			WarpProg m_warpProg_NoDispersion;
			WarpProg m_warpProgLayout_NoDispersion;
			WarpProg m_warpProg_Dispersion;
			WarpProg m_warpProgLayout_Dispersion;
			// Multiview
			WarpProg m_warpProg_Multiview_NoDispersion;
			WarpProg m_warpProg_MultiviewHighRes_NoDispersion;
			// Bad performance
			// WarpProg m_warpProg_Multiview_Dispersion;
			// WarpProg m_warpProg_MultiviewHighRes_Dispersion;

			// WarpProg m_test;
			bool m_HasEXT_sRGB_write_control;

			Matrix4f m_DistortionFixMatrix;

			struct __tagCenterLineParam
			{
				__tagCenterLineParam(){ m_fColR = m_fColG = m_fColB = 1.0; m_fWidth = 0.0; };
				float m_fColR;
				float m_fColG;
				float m_fColB;
				float m_fWidth;// Pixs
			}m_CenterLineParam;
			GLuint  m_CellsImageTexID;
// 			LineDraw m_LineDraw;
// 			QuadDraw m_QuadDraw;
			static GLuint CreateCells(int iImageWidth, int iImageHeight, int iCellWidth, int iLineWidth, int iBKCol, int iLineCol, float fPPI, float fMetersPerTanAngleAtCenter);
			GLuint CreateCellsImage();

			static bool g_bIsModifyed;

			RenderFrame m_RenderFrame;// 单线程模式下的唯一渲染对象

			MojingRenderBase();

			static GLuint GetEyeTextureId(EyeTextureType ett,
				unsigned int &iWidth, unsigned int &iHeight, GLenum &format, RenderFrame *pRF);
			static void SetEyeTexID(int leftEye, int rightEye, RenderFrame *pRF);
			static void UnityATW_SetEyeTexID(int leftEye, int rightEye, float *pViewMatrix, RenderFrame *pRF);
			UInt64 GetCurrentModelFrameInfo(RenderFrame *pRF, double *pTime = NULL, float * pMatrix = NULL);
			UInt64 GetCurrentModelFrameInfo(RenderFrame *pRF, double *pTime /*= NULL*/, float * x, float * y, float * z, float * w);
		public:
			virtual ~MojingRenderBase();
			virtual bool WarpToScreen();
			virtual int UnityATW_WarpToScreen(){ WarpToScreen(); return 0; };
			static void SetModify();

			static void CreateCurrentRender(bool bEnableMultiThread, bool bEnableTimeWarp);
			static MojingRenderBase *GetCurrentRender();
			static void ReleaseCurrentRender();

			static WarpProg BuildWarpProg(const char * vertex, const char * fragment);
			static GlProgram BuildProgram(const char * vertexSrc, const char * fragmentSrc);
			static void	DeleteProgram(GlProgram & prog);



			static MojingRenderBase *GetRender(int iID);
			static void ReleaseRender(int iID);

			virtual GLuint GetEyeTextureId(EyeTextureType ett,
				unsigned int &iWidth, unsigned int &iHeight, GLenum &format);

			virtual void SetCenterLine(int iWidth, int colR, int colG, int colB, int colA);
			virtual void SetEyeTexID(int leftEye, int rightEye);
			virtual void UnityATW_SetEyeTexID(int leftEye, int rightEye, float *pViewMatrix);
			virtual void SetOverlayTextureID(int leftOverlayEye, int rightOverlayEye);

			virtual UInt64 GetCurrentModelFrameInfo(double *pTime = NULL, float * pMatrix = NULL);
			virtual UInt64 GetCurrentModelFrameInfo(double *pTime /*= NULL*/, float * x, float * y, float * z, float * w);

			CLASS_MEMBER(bool, m_b, EnableTimeWarp);

			CLASS_INTERFACE(int, m_i, RenderThreadID);
			CLASS_INTERFACE(int, m_i, DistortionThreadID);


			CLASS_INTERFACE(int, m_i, ScreenWidth);// 来自屏幕参数的数据，为了防止每一DrawCall都去读取，这里把他们记录下来
			CLASS_INTERFACE(int, m_i, ScreenHeight);// 来自屏幕参数的数据，为了防止每一DrawCall都去读取，这里把他们记录下来
			CLASS_INTERFACE(int, m_i, ScreenWidth_Half);// 来自屏幕参数的数据，为了防止每一DrawCall都去读取，这里把他们记录下来
			CLASS_INTERFACE(int, m_i, ScreenHeight_Half);// 来自屏幕参数的数据，为了防止每一DrawCall都去读取，这里把他们记录下来


			CLASS_INTERFACE(int, m_i, LeftEyeTexID);
			CLASS_INTERFACE(int, m_i, RightEyeTexID);
			CLASS_INTERFACE(UInt64, m_ui64, NextDisplayFrameIndex);// 记录帧序号

			CLASS_INTERFACE(double, m_d, NextFrameDistortionTime);
			CLASS_INTERFACE(double, m_d, NextFrameDisplayTime);// 记录下一帧渲染时的时间
			CLASS_MEMBER(float, m_f, CenterLineWidth);

			CLASS_INTERFACE(int, m_i, LeftOverlayTextureID);
			CLASS_INTERFACE(int, m_i, RightOverlayTextureID);

			CLASS_INTERFACE(bool, m_b, Landscape);
			CLASS_MEMBER(Vector4f, m_v4 , LeftOverlayRect);
			CLASS_MEMBER(Vector4f, m_v4, RightOverlayRect);

			CLASS_INTERFACE(bool, m_b, ShowTimeWapRange);
		protected:
			virtual bool LoadMeshFromMemory();
			virtual void SetWarpState(bool bEnableAlpha);
			virtual void InstallShader();
			virtual void OnModifyed();

			virtual void BindWarpProgram(WarpProg warpProg, const Matrix4f timeWarps, const int eye, bool bClamp = false);
		
			virtual void BindEyeTextures(GLuint TexId);
			virtual void BindEyeOverlayTextures(GLuint TexId);
			virtual void UnbindEyeTextures();
			virtual void UnbindEyeOverlayTextures();
			// 根据时间线选用一帧畸变图像，并且测算陀螺仪偏移
			virtual RenderFrame * SelectDistortionFrame();
			virtual void DoMtpTest(EyeTextureType eyeType = TEXTURE_BOTH_EYE);
			virtual bool DrawDistortion(RenderFrame *pFrame , EyeTextureType DrawEyes = TEXTURE_BOTH_EYE);
			virtual bool DrawDistortionRange(EyeTextureType DrawEyes = TEXTURE_BOTH_EYE);
			//jxb ADD
			virtual bool DrawDistortionGUI(EyeTextureType DrawEyes = TEXTURE_BOTH_EYE);
			virtual bool DrawCenterLine();
			//


			static bool CompileShader(const GLuint shader, const char * src);
			virtual bool ExtensionStringPresent(const char * extension, const char * allExtensions);

		protected:
			
			
			std::vector<GLuint> m_shaderIds;
			std::vector<GLuint> m_programIds;
			std::map<unsigned int, EyeTextureParam> m_eyeTextureParams;
		protected:

			void clearShaderIds();
			void clearProgramIds();
			void clearEyeTextures();
			GLuint genTextureWithBlackBorder(GLuint textureId, int width, int height);
			void FramesPerSecond();


			Vector3f m_Point;
			Vector3f m_Offset;
		};
	}
}
