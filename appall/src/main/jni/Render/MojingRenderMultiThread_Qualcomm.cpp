#include "MojingRenderMultiThread_Qualcomm.h"
#include "../Base/MojingLog.h"

#include "../3rdPart/Qualcomm/svrCpuTimer.h"
#include "../3rdPart/Qualcomm/svrGpuTimer.h"

#include "../MojingManager.h"
#include "../Tracker/MojingTracker.h"

#if !defined( EGL_OPENGL_ES3_BIT_KHR )
#define EGL_OPENGL_ES3_BIT_KHR		0x0040
#endif

namespace Baofeng
{
	namespace Mojing
	{
		///
		Quatf QuatfFromSvrQuat(svrQuaternion svrQuat)
		{
			return Quatf(svrQuat.x, svrQuat.y, svrQuat.z, svrQuat.w);
		}

		Matrix4f CalculateWarpMatrix(Quatf& origPose, Quatf& latestPose)
		{
			Quatf diff = origPose * latestPose.Inverted();
			Quatf invDiff = diff.Inverted();

			invDiff.z *= -1.0f;

			return Matrix4f(invDiff);
		}

		Matrix4f CalculateProjectionMatrix(float fovRad)
		{
			float tanHalfFov = tan(0.5f * fovRad);
			Matrix4f retMtx(
				1.0f / tanHalfFov, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
				0.0f, 0.0f, -1.0f, 0.0f,
				0.0f, 0.0f, -1.0f, 0.0f
			);

			return retMtx.Transposed();
		}

		MojingRenderMultiThread_Qualcomm::MojingRenderMultiThread_Qualcomm()
		{
		}

		MojingRenderMultiThread_Qualcomm::~MojingRenderMultiThread_Qualcomm()
		{
		}

		void MojingRenderMultiThread_Qualcomm::InstallShader()
		{
			m_warpProg = BuildWarpProg(VertexShader, FragmentShader);
		}

		void MojingRenderMultiThread_Qualcomm::BindEyeTextures(GLuint TexId)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TexId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		void MojingRenderMultiThread_Qualcomm::UnbindEyeOverlayTextures()
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		bool MojingRenderMultiThread_Qualcomm::UpdateEyeContextSurface()
		{
			EGLDisplay eyeDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

			//First we need to bind the eye calling threads context to a small pubffer surface so the
			//warp context can take over the window surface
			eyeRenderContext = eglGetCurrentContext();
			LOGI("   eyeRenderContext : %x", (unsigned int)eyeRenderContext);

			eyeRenderWarpSurface = eglGetCurrentSurface(EGL_DRAW);

			EGLint largestPBuffer = -1;
			eglQuerySurface(eyeDisplay, eyeRenderWarpSurface, EGL_LARGEST_PBUFFER, &largestPBuffer);

			//Can't find any other way to directly query if the current surface for the eye buffer context is a pbuffer other
			//than checking the largest pbuffer attribute.  If the value is unmodified then the surface is a window surface
			//otherwise a pbuffer is already current and we don't need to do anything else.
			if (largestPBuffer == -1)
			{
				//Save the original surface the application created for the window and its configuration
				eyeRenderOrigSurface = eglGetCurrentSurface(EGL_DRAW);
				LOGI("   eyeRenderSurface : %x", (unsigned int)eyeRenderOrigSurface);
				eglQuerySurface(eyeDisplay, eyeRenderOrigSurface, EGL_CONFIG_ID,
					&eyeRenderOrigConfigId);


				EGLConfig pbufferEyeSurfaceConfig = 0;

				EGLint configId;
				eglQueryContext(eyeDisplay, eyeRenderContext, EGL_CONFIG_ID, &configId);

				EGLint numConfigs;
				EGLConfig configs[1024];
				eglGetConfigs(eyeDisplay, configs, 1024, &numConfigs);
				int i = 0;
				for (; i < numConfigs; i++)
				{
					EGLint tmpConfigId;
					eglGetConfigAttrib(eyeDisplay, configs[i], EGL_CONFIG_ID, &tmpConfigId);
					if (tmpConfigId == configId)
					{
						pbufferEyeSurfaceConfig = configs[i];
					}
				}
				LOGI("Warp Context Replace Config ID : %d", i);

				const EGLint surfaceAttribs[] =
				{
					EGL_WIDTH, 16,
					EGL_HEIGHT, 16,
					EGL_NONE
				};

				eyeRenderWarpSurface = eglCreatePbufferSurface(eyeDisplay, pbufferEyeSurfaceConfig, surfaceAttribs);
				if (eyeRenderWarpSurface == EGL_NO_SURFACE)
				{
					LOGE("svrUpdateEyeContextSurface: Failed to create eye context pbuffer surface");
					return false;
				}

				if (eglMakeCurrent(eyeDisplay, eyeRenderWarpSurface,
					eyeRenderWarpSurface, eyeRenderContext) == EGL_FALSE)
				{
					LOGE("svrUpdateEyeContextSurface: eglMakeCurrent failed for eye context");
					return false;
				}
			}

			return true;
		}

		bool MojingRenderMultiThread_Qualcomm::BeginTimeWarp()
		{
			LOGI("svrBeginTimeWarp");

			if (!UpdateEyeContextSurface())
			{
				return false;
			}

			int status = pthread_create(&warpThread, NULL, &MojingRenderMultiThread_Qualcomm::WarpThreadMain, NULL);
			if (status != 0)
			{
				LOGE("svrBeginTimeWarp: Failed to create warp thread");
			}
			pthread_setname_np(warpThread, "svrWarp");

			//Wait until the warp thread is created and the context has been created before continuing
			LOGI("Waiting for timewarp context creation...");
			pthread_mutex_lock(&warpThreadContextMutex);
			while (!warpContextCreated)
			{
				pthread_cond_wait(&warpThreadContextCv, &warpThreadContextMutex);
			}
			pthread_mutex_unlock(&warpThreadContextMutex);

			return true;
		}

		void MojingRenderMultiThread_Qualcomm::DestroyWarpContext()
		{
			EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

			eglDestroySurface(display, warpRenderSurface);
			eglDestroyContext(display, warpRenderContext);

		}

		bool MojingRenderMultiThread_Qualcomm::CreateWarpContext()
		{
			const EGLint attribs[] = {
				EGL_BLUE_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_RED_SIZE, 8,
				EGL_DEPTH_SIZE, 0,
				EGL_NONE
			};

			EGLint format;
			EGLSurface surface;
			EGLContext context;

			EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

			//We need to destroy the surface from the eye render thread that was bound to the window
			if (eyeRenderOrigSurface != EGL_NO_SURFACE)
			{
				eglDestroySurface(display, eyeRenderOrigSurface);
				eyeRenderOrigSurface = EGL_NO_SURFACE;
			}

			//Find an appropriate config
			EGLConfig configs[512];
			EGLint numConfigs = 0;

			eglGetConfigs(display, NULL, 0, &numConfigs);
			LOGI("svrCreateWarpContext, %d EGL configs available", numConfigs);

			eglGetConfigs(display, configs, 512, &numConfigs);

			EGLConfig config = 0;
			for (int i = 0; i < numConfigs; i++)
			{
				EGLint value = 0;

				eglGetConfigAttrib(display, configs[i], EGL_RENDERABLE_TYPE, &value);
				if ((value & EGL_OPENGL_ES3_BIT_KHR) != EGL_OPENGL_ES3_BIT_KHR)
				{
					continue;
				}

				eglGetConfigAttrib(display, configs[i], EGL_SURFACE_TYPE, &value);
				if ((value & (EGL_WINDOW_BIT)) != (EGL_WINDOW_BIT))
				{
					continue;
				}

				int	j = 0;
				for (; attribs[j] != EGL_NONE; j += 2)
				{
					eglGetConfigAttrib(display, configs[i], attribs[j], &value);
					if (value != attribs[j + 1])
					{
						break;
					}
				}
				if (attribs[j] == EGL_NONE)
				{
					config = configs[i];
					LOGI("svrCreateWarpContext, found EGL config");
					break;
				}
			}

			if (config == 0)
			{
				LOGI("svrCreateWarpContext, failed to find EGL config");
				return -1;
			}

			/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
			* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
			* As soon as we picked a EGLConfig, we can safely reconfigure the
			* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
			eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
			// TODO:
			//ANativeWindow_setBuffersGeometry(gAppContext->modeContext->nativeWindow, 1920, 1080, format);

			EGLint renderBufferType = EGL_BACK_BUFFER;

			if (gSingleBufferWindow)
			{
				renderBufferType = EGL_SINGLE_BUFFER;
			}

			const EGLint windowAttribs[] = {
				EGL_RENDER_BUFFER, renderBufferType,
				EGL_NONE
			};
			// TODO: we have done it in java
			//surface = eglCreateWindowSurface(display, config, gAppContext->modeContext->nativeWindow, windowAttribs);
			if (surface == EGL_NO_SURFACE)
			{
				LOGE("svrCreateWarpContext: Failed to create EGL surface");
				return false;
			}

			EGLint contextAttribs[] =
			{
				EGL_CONTEXT_CLIENT_VERSION, 3,
				EGL_CONTEXT_PRIORITY_LEVEL_IMG, EGL_CONTEXT_PRIORITY_HIGH_IMG,
				EGL_NONE
			};

			context = eglCreateContext(display, config, eyeRenderContext, contextAttribs);
			if (context == EGL_NO_CONTEXT)
			{
				LOGE("svrCreateWarpContext: Failed to create EGL context");
				return false;
			}

			//Check the context priority that was actually assigned
			EGLint resultPriority;
			eglQueryContext(display, context, EGL_CONTEXT_PRIORITY_LEVEL_IMG, &resultPriority);
			switch (resultPriority)
			{
			case EGL_CONTEXT_PRIORITY_HIGH_IMG:
				LOGI("svrCreateWarpContext: context has high priority");
				break;
			case EGL_CONTEXT_PRIORITY_MEDIUM_IMG:
				LOGE("svrCreateWarpContext: context has medium priority");
				break;
			case EGL_CONTEXT_PRIORITY_LOW_IMG:
				LOGE("svrCreateWarpContext: context has low priority");
				break;
			default:
				LOGE("svrCreateWarpContext: unknown context priority");
				break;
			}

			if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
			{
				LOGE("svrCreateWarpContext: eglMakeCurrent failed");
				return false;
			}

			EGLint acutalRenderBuffer;
			eglQueryContext(display, context, EGL_RENDER_BUFFER, &acutalRenderBuffer);
			if (gSingleBufferWindow)
			{
				LOGI("svrCreateWarpContext: Requested EGL_SINGLE_BUFFER");
			}
			else
			{
				LOGI("svrCreateWarpContext: Requested EGL_BACK_BUFFER");
			}

			if (acutalRenderBuffer == EGL_SINGLE_BUFFER)
			{
				LOGI("svrCreateWarpContext: Got EGL_SINGLE_BUFFER");
			}
			else if (acutalRenderBuffer == EGL_BACK_BUFFER)
			{
				LOGI("svrCreateWarpContext: Got EGL_BACK_BUFFER");
			}
			else if (acutalRenderBuffer == EGL_NONE)
			{
				LOGE("svrCreateWarpContext: Got EGL_NONE");
			}
			else
			{
				LOGE("svrCreateWarpContext: Got Unknown");
			}

			eglQuerySurface(display, surface, EGL_WIDTH, &warpRenderSurfaceWidth);
			eglQuerySurface(display, surface, EGL_HEIGHT, &warpRenderSurfaceHeight);

			LOGI("Warp Surface Dimensions ( %d x %d )", warpRenderSurfaceWidth, warpRenderSurfaceHeight);

			warpRenderContext = context;
			warpRenderSurface = surface;

			LOGI("Timewarp context created");

			pthread_mutex_lock(&warpThreadContextMutex);
			warpContextCreated = true;
			pthread_cond_signal(&warpThreadContextCv);
			pthread_mutex_unlock(&warpThreadContextMutex);

			return 0;
		}

		void* MojingRenderMultiThread_Qualcomm::WarpThreadMain(void* arg)
		{
		}
		
		void WarpSleep(uint64_t sleepTimeNano)
		{
			uint64_t preSleep = Svr::GetTimeNano();
			// TODO:
			bool gBusyWait = false;
			if (!gBusyWait)
			{
				timespec t, rem;
				t.tv_sec = 0;
				t.tv_nsec = sleepTimeNano;
				nanosleep(&t, &rem);
			}
			else
			{
				while (true)
				{
					uint64_t timeNow = Svr::GetTimeNano();
					if ((timeNow - preSleep) > sleepTimeNano)
					{
						break;
					}
				}
			}

			uint64_t postSleep = Svr::GetTimeNano();

			// TODO:
			bool gLogEyeOverSleep = false;
			if (gLogEyeOverSleep)
			{
				uint64_t sleepDelta = postSleep - preSleep;
				float waitDelta = (float)((int64_t)sleepDelta - (int64_t)sleepTimeNano);

				if ((waitDelta * NANOSECONDS_TO_MILLISECONDS) > 1.0f)
				{
					LOGE("WarpSleep: Overslept by %2.3f ms", waitDelta * NANOSECONDS_TO_MILLISECONDS);
				}
			}
		}

		bool MojingRenderMultiThread_Qualcomm::THREAD_InitGLThread()
		{
			if (MojingRenderMultiThread::THREAD_InitGLThread())
			{
				prevWarpFrameCount = 0;

				LOGI("WarpThreadMain Entered");

				display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#if defined(USE_QVR_SERVICE)
				if (gEnableWarpThreadFifo)
				{
					LOGI("Setting TimeWarp Scheduling policy to SCHED_FIFO");
					gAppContext->qvrService->SetThreadPriority(gettid(), SCHED_FIFO, 98);
				}
#endif
				// TODO:
				//if (gTimeWarpThreadCore >= 0)
				//{
				//	LOGI("Setting TimeWarp Affinity to %d", gTimeWarpThreadCore);
				//	svrSetThreadAffinity(gTimeWarpThreadCore);
				//}

				CreateWarpContext();

				// create warpmesh
				if (!m_warpMesh->IsAvailable() || g_bIsModifyed)
				{
					OnModifyed();
				}

				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				glEnable(GL_SCISSOR_TEST);

				if (gDirectModeWarp)
				{
					LOGI("Rendering in direct mode");
					glEnable(GL_BINNING_CONTROL_HINT_QCOM);
					glHint(GL_BINNING_CONTROL_HINT_QCOM, GL_RENDER_DIRECT_TO_FRAMEBUFFER_QCOM);
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				glViewport(0, 0, 1920, 1080);
				glScissor(0, 0, 1920, 1080);
			}
		}

		bool MojingRenderMultiThread_Qualcomm::THREAD_DoDistortion()
		{
			//while (true)
			{
				//Wait until the raster is at the halfway point, find a set of eye buffers and render the left eye
				float displayRefreshRateHz = 60.0f;
				double framePeriodNano = 1e9 / displayRefreshRateHz;
				uint64_t waitTime;

				pthread_mutex_lock(&vsyncMutex);
				uint64_t timestamp = Svr::GetTimeNano();
				double framePct = (double)vsyncCount + ((double)timestamp - vsyncTimeNano) / (framePeriodNano);
				pthread_mutex_unlock(&vsyncMutex);

				uint64_t warpVsyncCount = ceil(framePct);

				double fractFrame = framePct - ((long)framePct);
				if (fractFrame < 0.5)
				{
					//We are currently in the first half of the display so wait until we hit the halfway point
					waitTime = (uint64_t)((0.5 - fractFrame) * framePeriodNano);
				}
				else
				{
					//We are currently past the halfway point in the display so wait until halfway through the next vsync cycle
					waitTime = (uint64_t)((1.5 - fractFrame) * framePeriodNano);
				}

				WarpSleep(waitTime);

				uint64_t postLeftWaitTimeStamp = Svr::GetTimeNano();

				unsigned int curSubmitFrameCount = submitFrameCount;

				svrFrameParamsInternal* pWarpFrame = NULL;

				//Get the frame parameters for the frame we will be warping
				for (int i = 0; i < NUM_SWAP_FRAMES - 1; i++)
				{
					int checkFrameCount = curSubmitFrameCount - i;

					if (checkFrameCount <= 0)
					{
						//This is the first time through and no frames have been submitted yet so bail out
						pWarpFrame = NULL;
						break;
					}

					svrFrameParamsInternal* pCheckFrame = &frameParams[checkFrameCount % NUM_SWAP_FRAMES];

					if (pCheckFrame->minVSyncCount > warpVsyncCount)
					{
						continue;
					}

					//Check to see if the frame has already finished on the GPU
					GLenum syncResult = eglClientWaitSyncKHR_(display, pCheckFrame->frameSync, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
					if (syncResult == GL_TIMEOUT_EXPIRED)
					{
						//The current frame hasn't finished on the GPU so keep looking back for a frame that has
						//LOGI("GPU not finished with frame %d", checkFrameCount);
						continue;
					}

					pWarpFrame = pCheckFrame;
					warpFrameCount = checkFrameCount;

					break;
				}

				LOGI("Warping %d [vc : %llu]", warpFrameCount, vsyncCount);

				//Signal the eye render thread that it can continue on
				if (!pthread_mutex_trylock(&warpBufferConsumedMutex))
				{
					pthread_cond_signal(&warpBufferConsumedCv);
					pthread_mutex_unlock(&warpBufferConsumedMutex);
				}

				if (pWarpFrame == NULL)
				{
					//No frame to warp (just started rendering??) so start over...
					LOGI("No valid frame to warp...");
					return false;
				}

				if (gLogFrameDoubles && pWarpFrame->frameParams.minVsyncs == 1 && warpFrameCount == prevWarpFrameCount)
				{
					LOGI("Frame doubled on %d [%llu,%llu]", warpFrameCount, warpVsyncCount, vsyncCount);
				}
				prevWarpFrameCount = warpFrameCount;

				if (pWarpFrame->warpFrameLeftTimeStamp == 0)
				{
					pWarpFrame->warpFrameLeftTimeStamp = postLeftWaitTimeStamp;
				}
				// TODO:
				//PROFILE_ENTER(GROUP_TIMEWARP, 0, "Warp Submission : %d", gAppContext->modeContext->warpFrameCount);

				Matrix4f texWarpMatrix[2];
				GLuint TexId[2];
				Vector4f scaleoffset(1.0f, 1.0f, 0.0f, 0.0f);
				Vector2f eyeBorder(0.0f, 1.0f);


				bool bTimeWarpEnabled = !((pWarpFrame->frameParams.frameOptions & kDisableReprojection) > 0);

				// We have a predicted time that was used to render each eye.  Look at what we predicted and 
				// where we are now and adjust the warp matrix to correct.
				// This is done by CalculateWarpMatrix() so no special code is needed.
				// Check and see if the predicted time is close to where we are now.
				uint64_t timeNowNano = Svr::GetTimeNano();
				uint64_t diffTimeNano = timeNowNano - pWarpFrame->frameParams.headPoseState.poseTimeStampNs;
				float diffTimeMs = diffTimeNano * NANOSECONDS_TO_MILLISECONDS;

				float adjustedTimeMs = 0.0f;
				if (pWarpFrame->frameParams.headPoseState.predictedTimeMs > 0.0f && !(pWarpFrame->frameParams.frameOptions & kEnableMotionToPhoton))
				{
					//Predict ahead 
					adjustedTimeMs = (0.5 * framePeriodNano) * 1e-6;
				}

				//LOGI("Left Eye Predicted %0.2f ms but it took %0.2f ms [%0.2f]", pWarpFrame->frameParams.headPoseState.predictedTimeMs, diffTimeMs, adjustedTimeMs);
				Quatf origQuat, latestQuat;

				int64_t timeToLeftEye = 0L;
				int64_t timeToRightEye = 0L;

				
				for (int eye = 0; eye < 2; eye++)
				{
					Matrix4f leftWarpMatrix;
					if (bTimeWarpEnabled || gWarpMeshType != 0)
					{
						//Get the updated sensor state
						//LOGI("TimeWarp left eye predicted time: %0.2f ms", adjustedTimeMs);
						
						origQuat = QuatfFromSvrQuat(pWarpFrame->frameParams.headPoseState.pose.rotation);
						svrHeadPoseState latestPoseState; // = svrGetPredictedHeadPose(adjustedTimeMs);
						if (0)
						{
							latestQuat = QuatfFromSvrQuat(latestPoseState.pose.rotation);
						}
						else
						{
							Manager* pManager = Manager::GetMojingManager();
							if (pManager)
							{
								Tracker* pTracker = pManager->GetTracker();
								if (pTracker)
								{
									latestQuat = pTracker->getPredictionPosition(adjustedTimeMs, NULL);
								}
							}
						}

						leftWarpMatrix = CalculateWarpMatrix(origQuat, latestQuat);

						timeToLeftEye = latestPoseState.poseTimeStampNs - pWarpFrame->frameParams.headPoseState.poseTimeStampNs;
					}
					texWarpMatrix[eye] = leftWarpMatrix * (CalculateProjectionMatrix(3.1415926f / 4)); // TODO: gAppContext->deviceInfo.targetFovXRad

					// prepare data

					glUseProgram(m_warpProg.Prog.program);

					if (eye == 1)
					{
						scaleoffset.z = 1.0f - gMeshIpdOffset;
						scaleoffset.w = 0.0f;
						eyeBorder.x = 0.0f;
						eyeBorder.y = 1.0f;
					}
					glUniform4fv(1, 1, &scaleoffset.x);
					glUniformMatrix4fv(2, 1, GL_FALSE, texWarpMatrix[eye].M[0]);
					glUniform2fv(3, 1, &eyeBorder.x);
					
					TexId[0] = GetLeftEyeTexID();
					TexId[1] = GetRightEyeTexID();
					BindEyeTextures(TexId[eye]);

					m_warpMesh->BindVBO();

					// clear
					if (gTimeWarpClearBuffer)
					{
						glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f);
						glScissor(1920 / 2 * eye, 0, 1920 / 2, 1080);
						glClear(GL_COLOR_BUFFER_BIT);
					}

					// draw
					m_warpLayer->DrawElements(eye);

					// TODO: draw overlay

					if (gSingleBufferWindow)
					{
						glFlush();
					}

					// Single buffer: wait to raster line at left edge
					if (gSingleBufferWindow && eye == 0)
					{
						uint64_t rightTimestamp = Svr::GetTimeNano();
						// TODO:
						//PROFILE_ENTER(GROUP_TIMEWARP, 0, "Right EyeBuffer Wait");
						//We started the left eye half way through vsync, figure out how long
						//we have left in the half frame until the raster starts over so we can render the right eye
						uint64_t delta = rightTimestamp - postLeftWaitTimeStamp;
						uint64_t waitTime = 0;
						if (delta < (uint64_t)(framePeriodNano / 2.0))
						{
							waitTime = ((uint64_t)(framePeriodNano / 2.0)) - delta;
							WarpSleep(waitTime);
						}
						else
						{
							//The left eye took longer than 1/2 the refresh so the raster has already wrapped around and is 
							//in the left half of the screen.  Skip the wait and get right on rendering the right eye.
							LOGE("Left Eye took too long!!! ( %2.3f ms )", delta * NANOSECONDS_TO_MILLISECONDS);
							// TODO:
							//PROFILE_MESSAGE(GROUP_TIMEWARP, 0, "Left Eye took too long!!! ( %2.3f ms )", delta * NANOSECONDS_TO_MILLISECONDS);
						}
						// TODO:
						//PROFILE_EXIT(GROUP_TIMEWARP);
					}

					// 
				} // end of left, right Eye rendering

				eglSwapBuffers(display, warpRenderSurface);

			} // draw loop

			LOGI("WarpThreadMain while loop exited");

			UnbindEyeTextures();

			glUseProgram(0);
			m_warpMesh->UnBindVBO();

			DestroyWarpContext();

			LOGI("WarpThreadMain cleaned up.  Exiting");
		}
	}
}