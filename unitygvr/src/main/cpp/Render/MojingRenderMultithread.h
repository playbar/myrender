#ifndef _MOJING_RENDER_MULTI_THREAD_H_
#define _MOJING_RENDER_MULTI_THREAD_H_
#include "MojingRenderBase.h"
#include "MultiThreadGLParam.h"

#include "RenderFrameCirular.h"

namespace Baofeng
{
	namespace Mojing
	{
		// 这是一个调度类，用于与窗口进行交互。真正绘制畸变图像在BasicRendThread类里面实现
		class MojingRenderMultiThread : public MojingRenderBase , public Thread
		{
		protected:
			// 这是三个渲染缓冲区，在两个线程时要来回轮换切换
			// static const int EYE_TEX_COUNT = 4;// 这里需要4个，因为：1帧交给绘制界面，1用于进行畸变，1帧保存历史帧，1帧防止以上帧有无法完成绘制的情况发生
			// RenderFrameManager m_FrameManager;
			RenderFrameCirular m_FrameManager;
			static MojingRenderMultiThread* m_pCurrentThread;
			static MultiThreadGLParam m_GLParam;
			Event *m_pInitThread;
			bool m_bInitThreadSucceeded;
		protected:
			// static MojingRendThread* m_MojingRendThread;
			// 主线程
			virtual int Run();
			// 初始GL化环境
			virtual bool THREAD_InitGLThread();
			// 释放GL环境
			virtual bool THREAD_ReleaseGLThread();
			// 按时间交换屏幕缓冲,由THREAD_DoDistortion调用
			virtual void THREAD_TimeSwap(RenderFrame * pDisplayFrame);
			// 由线程来调用的渲染到屏幕的函数
			virtual bool THREAD_DoDistortion();

			virtual void OnModifyed();
		public:
			MojingRenderMultiThread();
			virtual ~MojingRenderMultiThread();
			virtual UInt64 GetCurrentModelFrameInfo(double *pTime, float * pMatrix);
			virtual UInt64 GetCurrentModelFrameInfo(double *pTime /*= NULL*/, float * x, float * y, float * z, float * w);
			
			// 继承之后就是为了禁用它，因为多线程下走的 THREAD_DoDistortion/THREAD_TimeSwap 函数，而不是这个函数
			// 这里激活模型绘制完成信号，让畸变线程可以开始干活
			virtual bool WarpToScreen();
			virtual int UnityATW_WarpToScreen();
			// 测试是否可以启用多线程渲染模式
			static bool InitRenderMultiThreadGLParam();
			// 测试是否正常启动并初始化了线程 
			bool GetInitThread();
			bool AfterDisplay(RenderFrame *pDisplayFrame);
			/************************************************************************/
			/* 以下代码是重载的基于RenderBase的单线程的函数                         */
			/************************************************************************/
			virtual GLuint GetEyeTextureId(EyeTextureType ett, unsigned int &iWidth,unsigned int &iHeight, GLenum &format);
			virtual void SetEyeTexID(int leftEye, int rightEye);
			virtual void UnityATW_SetEyeTexID(int leftEye, int rightEye, float *pViewMatrix); 
			virtual RenderFrame * SelectDistortionFrame();
#ifdef _DEBUG
			// 调试模式下为了区分Render的类型，这里把中线设置成蓝色
			virtual void SetCenterLine(int iWidth, int colR, int colG, int colB, int colA) 
			{
				MojingRenderBase::SetCenterLine(iWidth, 0, 0, colB / 255.0, colA);
			}
#endif
			CLASS_MEMBER(bool, m_b, ExitThread);
		};
	}
}

#endif