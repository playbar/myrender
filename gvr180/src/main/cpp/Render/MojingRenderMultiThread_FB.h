#pragma once
#include "MojingRenderMultithread.h"
#include "../3rdPart/IntelVSync/IntelVSync.h"
namespace Baofeng
{
	namespace Mojing
	{
		class MojingRenderMultiThread_FB :
			public MojingRenderMultiThread
		{
		protected:
			IntelVSync m_IntelVSync;
			virtual RenderFrame * SelectDistortionFrame();
			virtual void OnModifyed();
		public:
			MojingRenderMultiThread_FB();
			virtual ~MojingRenderMultiThread_FB();
			CLASS_INTERFACE(bool , m_b , SwapBuffer);
			virtual bool InitVSyncParam();
			virtual bool THREAD_DoDistortion();
			virtual bool AfterDisplay(RenderFrame *pDisplayFrame);
			virtual void SetCenterLine(int iWidth, int colR, int colG, int colB, int colA);
		};
	}
}
