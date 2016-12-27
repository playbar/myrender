#ifndef MOJING_RENDER_MULTITHREAD_QUALCOMM
#define MOJING_RENDER_MULTITHREAD_QUALCOMM

#include "MojingRenderMultithread.h"
#include "../Base/GlUtils.h"
#include <pthread.h>
#include "../3rdPart/Qualcomm/svrApi.h"

#define NUM_SWAP_FRAMES 5
#define NANOSECONDS_TO_MILLISECONDS 0.000001f

namespace Baofeng
{
	namespace Mojing
	{
		class MojingRenderMultiThread_Qualcomm : public MojingRenderMultiThread
		{
		protected:
			//	virtual RenderFrame * SelectDistortionFrame();
			//	virtual void OnModifyed();
			void BindEyeTextures(GLuint TexId) override;
			void UnbindEyeOverlayTextures() override;
			//	bool DrawDistortion_Multiview(RenderFrame *pDistortionFrame, EyeTextureType DrawEyes);
			//	void SetEyeTexID(int leftEye, int rightEye) override;

			void InstallShader() override;
		public:
			MojingRenderMultiThread_Qualcomm();
			virtual ~MojingRenderMultiThread_Qualcomm();
			//CLASS_INTERFACE(bool, m_b, SwapBuffer);
			//virtual bool InitVSyncParam();
			bool THREAD_InitGLThread() override;
			bool THREAD_DoDistortion() override;
			//virtual bool AfterDisplay(RenderFrame *pDisplayFrame);
			//virtual void SetCenterLine(int iWidth, int colR, int colG, int colB, int colA);

		private:
			// global setting
			bool gSingleBufferWindow = false;
			int gWarpMeshType = 0;
			float gMeshIpdOffset = 0.0f;
			bool gDirectModeWarp = true;
			bool gLogFrameDoubles = true;
			bool gTimeWarpClearBuffer = false;

			// predraw
			unsigned int prevWarpFrameCount = 0;
			EGLDisplay display;

			float m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

			bool UpdateEyeContextSurface();
			static void* WarpThreadMain(void* arg);
			bool BeginTimeWarp();
			bool CreateWarpContext();
			void DestroyWarpContext();
			// Vsync
			uint64_t        vsyncCount;
			uint64_t        vsyncTimeNano;
			pthread_mutex_t vsyncMutex;

			//Warp Thread/Context data
			EGLSurface      eyeRenderWarpSurface;
			EGLSurface      eyeRenderOrigSurface;
			EGLint          eyeRenderOrigConfigId;
			EGLContext      eyeRenderContext;

			EGLContext      warpRenderContext;
			EGLSurface      warpRenderSurface;
			int             warpRenderSurfaceWidth;
			int             warpRenderSurfaceHeight;

			pthread_cond_t  warpThreadContextCv;
			pthread_mutex_t warpThreadContextMutex;
			bool            warpContextCreated;

			pthread_t       warpThread;
			bool            warpThreadExit;

			pthread_cond_t  warpBufferConsumedCv;
			pthread_mutex_t warpBufferConsumedMutex;

			//
			struct svrFrameParamsInternal
			{
				svrFrameParams      frameParams;
				GLsync              frameSync;
				uint64_t            frameSubmitTimeStamp;
				uint64_t            warpFrameLeftTimeStamp;
				uint64_t            warpFrameRightTimeStamp;
				uint64_t            minVSyncCount;
			};

			svrFrameParamsInternal frameParams[NUM_SWAP_FRAMES];
			unsigned int           submitFrameCount;
			unsigned int           warpFrameCount;
			uint64_t               prevSubmitVsyncCount;
		};
	}
}


#endif // MOJING_RENDER_MULTITHREAD_QUALCOMM