#pragma once
#include "RenderFrame.h"
#include "../Base/MojingThreads.h"
#include <vector>

using namespace std;
namespace Baofeng
{
	namespace Mojing
	{
		/* 注意：当切换显示对象以后，如何切换帧管理器？？*/
//#define FRAME_COUNT 1 // 注意：这里只允许有1帧,为了在多线程模型下能够调试TimeWarp
		
		class RenderFrameManager
		{
#define FRAME_COUNT 4 // 注意：这里只允许有4帧
			friend class MojingRenderMultiThread;
		public:
			RenderFrameManager();
			virtual ~RenderFrameManager();
			
			void InitFrame();
			CLASS_INTERFACE(int , m_i , MaxFrameCount);
			// 为了协同线程，这个标记很重要
			CLASS_MEMBER(bool , m_b , ExitState);
			// 为了协同其他的线程，在这个函数里面将释放所有的同步对象
			virtual void Release();
			// 返回当前用于绘制模型的Frame
			virtual RenderFrame* GetCurrentModleFrame();
			// 返回用于当前畸变的Frame
			virtual RenderFrame* GetCurrentDistortionFrame(const UInt64 ui64FrameIndex, const double dEndTime);
			// 返回已经完成绘制并显示到屏幕上以后的最后的帧
			virtual RenderFrame* GetLastDisplayFrame();

			// 设置当前帧的绘制模型完成
			virtual void SetEndOfRenderModle(RenderFrame* pFR, FrameWorkFlowState EyeMode);
			// 设置畸变帧的畸变完成
			virtual void SetEndOfDistortion(RenderFrame* pFR);
			// 设置帧的显示完成
			virtual void SetEndOfDisplay();

		protected:
			RenderFrame* m_pRenderFrames;
			// 锁对象，用于锁定队列：等待绘制模型的帧数组
			Mutex *m_pModleLocker;
			vector<int> m_ModelIndex;// 当前java端渲染模型用的目标纹理
			
			// 锁对象，用于锁定队列：等待畸变的帧数组
			Mutex *m_pDistortionLocker;
			vector<int> m_DistortionIndex;	// 下一个反畸变端要显示到屏幕上的纹理
			int m_iDistortionSkip;			// 记录为了等待绘制完成被跳过了多少帧

			Mutex *m_pLastDisplayLocker;
			vector<int> m_LastDisplayIndex;// 历史一个反畸变端要显示到屏幕上的纹理

			// 指示是否已经开始了模型的绘制Event 
			//Event	*m_pEnableDistortoinRender;
			bool m_bEnableDistortoinRender;
			// 指示是否允许畸变模型用于绘制下一帧模型Event 
			// Event	*m_pEnableNextModelRender;
			bool m_bEnableNextModelRender;

			UInt64 m_ui64NextFrameID;
		};

	}
}