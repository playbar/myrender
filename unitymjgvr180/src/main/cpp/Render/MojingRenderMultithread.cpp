#include <unistd.h>
#include <ctime>
#include "MojingRenderMultithread.h"
#include "MojingRenderBase.h"
#include "Vsync.h"
#include "../MojingAPI.h"
#include "../MojingManager.h"
#include "../Distortion/MojingDistortion.h"
#include "../Parameters/MojingParameters.h"
#include "../Parameters/MojingDisplayParameters.h"
#include "../Base/MojingTypes.h"
#include "../Base/BuildType.h"
#include "../Base/MojingTimer.h"

#ifdef LOG4CPLUS_IMPORT
#include "../3rdPart/log4cplus/LogInterface.h"
#else
#include "../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

namespace Baofeng
{
	namespace Mojing
	{
#if defined(MJ_OS_MAC)|| defined(MJ_OS_WIN32)
		extern int gettid(void);
#endif
		MultiThreadGLParam MojingRenderMultiThread::m_GLParam;
		MojingRenderMultiThread* MojingRenderMultiThread::m_pCurrentThread = NULL;
		MojingRenderMultiThread::MojingRenderMultiThread() : 
			MojingRenderBase(), 
			m_bInitThreadSucceeded(false),
			m_bExitThread(false)
		{
			if (m_pCurrentThread &&
				m_pCurrentThread->GetInitThread())
			{
				MOJING_TRACE(g_APIlogger , "Find Distion Thread , Exit it....");
				m_pCurrentThread->m_bExitThread = true;
				m_pCurrentThread = NULL;
			}

			m_pInitThread = new Event();
			// 将自己设置为唯一渲染线程
			m_pCurrentThread = this;
			// 优先级提升到最高
			Priority = Thread::CriticalPriority;
			//Start();
		}

		MojingRenderMultiThread::~MojingRenderMultiThread()
		{
			// 确认线程退出
			m_bExitThread = true;
			m_FrameManager.SetExitState(true);
			int iTryCount = 0;
			
			// 等待200毫秒
			while (m_pCurrentThread == this && iTryCount < 200)
			{
				usleep(1000);
			}
			
			if (gettid() == GetRenderThreadID())
			{// 回复模型线程的结构
				m_GLParam.ReleaseModelParam();
			}
		}
		bool MojingRenderMultiThread::GetInitThread()
		{
			m_pInitThread->Wait();
			return m_bInitThreadSucceeded;
		}

		bool MojingRenderMultiThread::InitRenderMultiThreadGLParam()
		{
			if (gettid() == m_GLParam.GetModelThreadID() && m_GLParam.GetInitModelParam())
				return true;
			return  m_GLParam.InitModelParam();
		}
	
		UInt64 MojingRenderMultiThread::GetCurrentModelFrameInfo(double *pTime, float * pMatrix)
		{
			RenderFrame *pFR = m_FrameManager.GetCurrentModleFrame();
			return MojingRenderBase::GetCurrentModelFrameInfo(pFR, pTime, pMatrix);
		}
		UInt64 MojingRenderMultiThread::GetCurrentModelFrameInfo(double *pTime /*= NULL*/, float * x, float * y, float * z, float * w)
		{
			RenderFrame *pFR = m_FrameManager.GetCurrentModleFrame();
			return MojingRenderBase::GetCurrentModelFrameInfo(pFR, pTime, x,y,z,w);
		}
		
		bool MojingRenderMultiThread::WarpToScreen()
		{
			RenderFrame *pFR;
			if (NULL != (pFR = m_FrameManager.GetCurrentModleFrame()))
			{
				// Relase distortion thread to do real work
				//MOJING_TRACE(g_APIlogger, "End of Model Render -- 1: " << pFR->GetFrameIndex());
				m_FrameManager.SetEndOfRenderModle(pFR, RENDER_SUCCEED);
				//MOJING_TRACE(g_APIlogger, "End of Model Render -- 2: " << pFR->GetFrameIndex());
			}
			return true;
		}

		int MojingRenderMultiThread::UnityATW_WarpToScreen()
		{
			int iRet = 0;
			RenderFrame *pFR;
			if (NULL != (pFR = m_FrameManager.GetCurrentModleFrame()))
			{
				// Relase distortion thread to do real work
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "End of Model Render: " << pFR->GetFrameIndex() << " / " << m_FrameManager.GetFrameIndexInCirular(pFR) << 
					"Left = " << pFR->GetLeftEyeTexture().GetTextureID() << 
					"Right= " << pFR->GetRightEyeTexture().GetTextureID());
#endif
				m_FrameManager.SetEndOfRenderModle(pFR, RENDER_SUCCEED);
				//MOJING_TRACE(g_APIlogger, "End of Model Render -- 2: " << pFR->GetFrameIndex());
				RenderFrame *pNextFR = m_FrameManager.GetCurrentModleFrame();
				while (pNextFR == NULL || pNextFR == pFR)
				{
					usleep(10);
					pNextFR = m_FrameManager.GetCurrentModleFrame();
					
				}
				iRet = m_FrameManager.GetFrameIndexInCirular(pNextFR);
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "Next Model Render: " << pNextFR->GetFrameIndex() << " / " << iRet);
#endif
			}
			return iRet;
		}

		GLuint MojingRenderMultiThread::GetEyeTextureId(EyeTextureType ett, unsigned int &iWidth,
			unsigned int &iHeight, GLenum &format)
		{
			RenderFrame *pFR;
			while (NULL != (pFR = m_FrameManager.GetCurrentModleFrame()))
			{
				if (pFR->GetFrameWorkFlowState() == RENDER_MODLE_BOTH ||
					(pFR->GetFrameWorkFlowState() == RENDER_MODLE_LEFT && ett == TEXTURE_LEFT_EYE) ||
					(pFR->GetFrameWorkFlowState() == RENDER_MODLE_RIGHT && ett == TEXTURE_RIGHT_EYE))
				{// 得到了不可用的Frame
					usleep(100);
					MOJING_WARN(g_APIlogger, "Can not using this Frame for GetEyeTextureId, ID = " << pFR->GetFrameIndex() << " , State = " << pFR->GetFrameWorkFlowState() << " , ett = " << (ett == TEXTURE_LEFT_EYE ? "LEFT" : (ett == TEXTURE_RIGHT_EYE ? "RIGHT" : "BOTH")));
					continue;
				}

				// 获取渲染纹理的大小
				GLuint ret = MojingRenderBase::GetEyeTextureId(ett, iWidth, iHeight, format, pFR);
				FrameWorkFlowState NS = RENDER_MODLE_BOTH;
				if (ett == TEXTURE_LEFT_EYE)
					NS = RENDER_MODLE_LEFT;
				else if (ett == TEXTURE_RIGHT_EYE)
					NS = RENDER_MODLE_RIGHT;
				m_FrameManager.SetEndOfRenderModle(pFR, NS);
				// 根据参数返回对应的纹理Id
				return ret;
			}
			return 0;
		}
		
		void MojingRenderMultiThread::SetEyeTexID(int leftEye, int rightEye)
		{
			RenderFrame *pFR;
			if (NULL != (pFR = m_FrameManager.GetCurrentModleFrame()))
			{// 注意：这里要处理来自外部的Eye Texture
				MojingRenderBase::SetEyeTexID(leftEye, rightEye, pFR);
			}
			// 为了适应U3D的调用，在WarpToScreen中做这个调用
			// m_FrameManager.SetEndOfRenderModle(pFR, RENDER_SUCCEED);
		}
	
		void MojingRenderMultiThread::UnityATW_SetEyeTexID(int leftEye, int rightEye, float *pViewMatrix)
		{
			RenderFrame *pFR;
			if (NULL != (pFR = m_FrameManager.GetCurrentModleFrame()))
			{// 注意：这里要处理来自外部的Eye Texture
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger , "MojingRenderMultiThread::UnityATW_SetEyeTexID , Left = " << leftEye << " , Right = " << rightEye);
#endif
				MojingRenderBase::UnityATW_SetEyeTexID(leftEye, rightEye, pViewMatrix, pFR);
			}
			else
			{
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "MojingRenderMultiThread::UnityATW_SetEyeTexID , GetCurrentModleFrame = NULL");
#endif
			}
		}
	}
}


