#pragma once
#include "EyeTextureParam.h"
#include "../Base/MojingMath.h"




namespace Baofeng
{
	namespace Mojing
	{
		enum FrameWorkFlowState{
			UNINIT,				//	0-尚未初始化
			INIT,				//	1-初始化完成
			RENDER_MODLE,		//	2-开始绘制模型
			RENDER_MODLE_LEFT,	//	3-左眼绘制
			RENDER_MODLE_RIGHT,	//	4-右眼绘制
			RENDER_MODLE_BOTH,	//  5-双眼绘制
			RENDER_LAYERS,		//  6-模型绘制完成，开始绘制图层
			RENDER_SUCCEED,		//	7-完成绘制开始进入畸变流程时为此状态
			DISTION_BEGIN,		//	8-畸变完成，并且等待下一轮模型绘制时为此状态
			DISTION_END,		//	9-畸变完成，并且等待显示
			DISPLAY_END,		//	10显示完成
		};

		class RenderFrame
		{
#ifdef MJ_OS_ANDROID
			EGLDisplay m_ModleDisplay;
			EGLSyncKHR m_ModleGpuSync; 
			EGLDisplay m_DistortionDisplay;
			EGLSyncKHR m_DistortionGpuSync;
#endif
			static float m_fModelDrawTime[5];// 记录历史5个帧的模型绘制时间开销，作为预测的采样基准
			static int	 m_uiCurrentDrawTimeIndex;// 此对象仅用于对m_fModelDrawTime数组的索引，记录应该写入到哪一个采样中
			float m_fStartDrawTime;
			void  SetDrawTime(float time);
			float GetDrawTime();
					
			static UInt64 m_ui64CurrentFrameIndex;
			static UInt16 m_uiNextFrameKey;
			static bool m_bMultiThread;
		public:
			RenderFrame();
			virtual ~RenderFrame();
			static void InitDrawTime();
			CLASS_MEMBER_REF(CEyeTextureParam, m_, LeftEyeTexture);
			CLASS_MEMBER_REF(CEyeTextureParam, m_, RightEyeTexture);
			// Multiview texture
			CLASS_MEMBER(GLuint, m_, MultiviewTexture);
			// 目前工作流状态记录在Frame上，将来如果需要更加细节的工作流状态可以考虑设置到Texture上
// #ifdef _DEBUG
// 		private:
// 			FrameWorkFlowState m_FrameWorkFlowState;
// 		public:
// 			virtual void SetFrameWorkFlowState(FrameWorkFlowState V); 
// 			virtual FrameWorkFlowState GetFrameWorkFlowState()const{ return m_FrameWorkFlowState; };
// #else
			CLASS_MEMBER(FrameWorkFlowState, m_, FrameWorkFlowState);// 状态机，表示当前这个Frame的状态
/*#endif*/
			// 帧序号，这个序号是从给出同步信号就开始计算，从初始化第一帧的时候开始使用.0表示尚未初始化
			CLASS_INTERFACE(UInt64, m_ui64, FrameIndex);
			// 帧预计显示时间，小于0表示尚未初始化
			CLASS_INTERFACE(double, m_d, PredictDisplayTime);
			// 帧预计陀螺仪位置，还没有翻过行/列的时候。单位矩阵表示尚未初始化
			// 注意：这个数据只会被Unity ATW的时候才会调用Set接口，否则原则上不允许外部调用Set接口
			CLASS_MEMBER(Quatf, m_, PredictView);
			// 帧唯一标识
			CLASS_INTERFACE(UInt16, m_, FrameKey);

			// 输入以时钟TickCount为单位的帧序号，内部填写预计显示时间和预计陀螺仪位置
			bool BeforModle(UInt64 ui64FrameIndex);
			// 结束模型绘制后调用
			bool AfterModle();
			

			void UpdatePredictView();
			// 开始执行畸变前调用，会等待模型绘制调用完成，
			// 如果成功等到了，则更新陀螺仪修正信息
			bool BeforDistortion(double dWaitEndTime);
			
			// 结束畸变绘制后调用
			bool AfterDistortion();

			// 等待畸变并显示完成。理论上这个函数应该立刻返回
			void WaitForDistortionFinish();
			// 测算是否完成了模型绘制，立即返回
			bool TestModleFinish();
			// 通过传入真实的显示时间，计算显示误差矩阵。默认的话给出单位矩阵
			Matrix4f CalculateTimeWarpMatrix(double dDisplayTime, double * pdOutSensotTime = NULL)const;

			static void SetMultiThread(bool bMultiThread);
			static bool GetMultiThread();
		};


	}
}
