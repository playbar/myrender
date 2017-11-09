#pragma once
#include "RenderFrame.h"
// 注意 ：这里暂时保持和RenderFrameManager的接口一致，但是没有派生关系
namespace Baofeng
{
	namespace Mojing
	{
		class RenderFrameCirular
		{
		public:
			RenderFrameCirular();
			virtual ~RenderFrameCirular();
			
			void InitFrame();
			// 为了协同其他的线程，在这个函数里面将释放所有的同步对象
			virtual void Release();
			// 返回当前用于绘制模型的Frame
			virtual RenderFrame* GetCurrentModleFrame();
			// 返回用于当前畸变的Frame
			virtual RenderFrame* GetCurrentDistortionFrame(const UInt64 ui64FrameIndex, const double dEndTime);
			// 返回已经完成绘制并显示到屏幕上以后的最后的帧
			virtual RenderFrame* GetLastDisplayFrame();
			virtual int GetFrameIndexInCirular(const RenderFrame*pFR)const{
				int iRet = pFR - m_pRenderFrames;
				return (iRet >= 0 && iRet < m_iMaxFrameCount) ? iRet : -1;
			};
			// 设置当前帧的绘制模型完成
			virtual void SetEndOfRenderModle(RenderFrame* pFR, FrameWorkFlowState EyeMode);
			// 设置畸变帧的畸变完成
			virtual void SetEndOfDistortion(RenderFrame* pFR);
			// 设置帧的显示完成
			virtual void SetEndOfDisplay();
			CLASS_INTERFACE(int, m_i, MaxFrameCount);
			CLASS_MEMBER(bool, m_b, ExitState);
			// 标记被畸变线程控制的允许进行模型绘制的最大帧号，理论上应该等于正在被显示的帧号+1
			CLASS_MEMBER(UInt64, m_ui64, EnableRenderTxtureID);
		protected:
			int PrevInCirular(int iIndex)const{ return m_iMaxFrameCount ? (iIndex - 1 + m_iMaxFrameCount) % m_iMaxFrameCount : -1; };
			int NextInCirular(int iIndex)const{ return m_iMaxFrameCount ? (iIndex + 1) % m_iMaxFrameCount : -1; };
			RenderFrame* m_pRenderFrames;
			int		m_iCurRenderFrameInCirular;
			int		m_iLastDistortionFrameInCirular;
			UInt64	m_ui64NextFrameID;
		};
	}
}
