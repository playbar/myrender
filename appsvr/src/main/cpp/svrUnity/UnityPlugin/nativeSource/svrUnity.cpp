#ifndef _SVR_UNITY_H_
#define _SVR_UNITY_H_

#include <jni.h>
#include <sys/syscall.h>
#include <memory.h>
#include <unistd.h>

#include <android/native_window_jni.h>
#include <android/log.h>
#include <framework/svrUtil.h>

#include "IUnityInterface.h"
#include "IUnityGraphics.h"

#include "svrApi.h"

#define SVR_EXPORT

#define DEBUG 0

#if DEBUG
#define LogDebug(...)	__android_log_print(ANDROID_LOG_INFO, "svrUnity", __VA_ARGS__ )
#else
#define LogDebug(...)
#endif

#define Log(...)		__android_log_print(ANDROID_LOG_INFO, "svrUnity", __VA_ARGS__ )
#define LogError(...)	__android_log_print(ANDROID_LOG_ERROR, "svrUnity",__VA_ARGS__ )

struct SvrInitParams
{
    JavaVM*	        javaVm;
    JNIEnv*	        javaEnv;
    jobject		    javaActivityObject;
};

class SvrUnity
{
public:
	bool				isInitialized;
	jobject				activity;
	pid_t				threadTid;
	JavaVM*				javaVm;
	bool				isInitializeDataSet;
	svrFrameParams		frameParams;
	bool				isFrameParamsDataSet;
	svrHeadPoseState	headPoseState;
	int 				trackingPoseMode;
	bool 				isSetTrackingPoseDataSet;
	int 				cpuPerfLevel;
	int					gpuPerfLevel;
	bool				isSetPerformanceLevelsDataSet;
	int 				surfaceValidWaitFramesCounter;
	const int 			kSurfaceValidWaitFrames;

public:
	SvrUnity() :
		isInitialized(false),
		activity(0),
		threadTid(0),
		javaVm(0),
		isInitializeDataSet(false),
		isFrameParamsDataSet(false),
		isSetTrackingPoseDataSet(false),
		trackingPoseMode(1),
		cpuPerfLevel(0),
		gpuPerfLevel(0),
		isSetPerformanceLevelsDataSet(false),
		surfaceValidWaitFramesCounter(0),
		kSurfaceValidWaitFrames(30)
	{
		memset(&frameParams, 0, sizeof(frameParams));
		memset(&headPoseState, 0, sizeof(headPoseState));
	}
};

SvrUnity plugin;

JNIEnv* GetJNIEnv(JavaVM* javaVM)
{
    JNIEnv* jniEnv;
    javaVM->AttachCurrentThread(&jniEnv, NULL);
    return jniEnv;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	LogDebug("JNI_OnLoad() called!");
	JNIEnv* javaEnv;
    if (vm->GetEnv(reinterpret_cast<void**>(&javaEnv), JNI_VERSION_1_2) != JNI_OK)
	{
        return -1;
    }

    // Get jclass with env->FindClass.
    // Register methods with env->RegisterNatives.
	plugin.javaVm = vm;
	return JNI_VERSION_1_2;
}

jobject GetViewSurface(JNIEnv * javaEnv, jobject activity)
{
	jclass activityClass = javaEnv->GetObjectClass(activity);
	if (activityClass == NULL)
	{
		LogError("activityClass == NULL!");
		return NULL;
	}

	jfieldID fid = javaEnv->GetFieldID(activityClass, "mUnityPlayer", "Lcom/unity3d/player/UnityPlayer;");
	if (fid == NULL)
	{
		LogError("mUnityPlayer not found!");
		return NULL;
	}

	jobject unityPlayerObj = javaEnv->GetObjectField(activity, fid);
	if(unityPlayerObj == NULL)
	{
		LogError("unityPlayer object not found!");
		return NULL;
	}

	jclass unityPlayerClass = javaEnv->GetObjectClass(unityPlayerObj);
	if (unityPlayerClass == NULL)
	{
		LogError("unityPlayer class not found!");
		return NULL;
	}

	jmethodID mid = javaEnv->GetMethodID(unityPlayerClass, "getChildAt", "(I)Landroid/view/View;");
	if (mid == NULL)
	{
		LogError("getChildAt methodID not found!");
		return NULL;
	}

	jboolean param = 0;
	jobject surfaceViewObj = javaEnv->CallObjectMethod( unityPlayerObj, mid, param);
	if (surfaceViewObj == NULL)
	{
		LogError("surfaceView object not found!");
		return NULL;
	}

	jclass surfaceViewClass = javaEnv->GetObjectClass(surfaceViewObj);
	mid = javaEnv->GetMethodID(surfaceViewClass, "getHolder", "()Landroid/view/SurfaceHolder;");
	if (mid == NULL)
	{
		LogError("getHolder methodID not found!");
		return NULL;
	}

	jobject surfaceHolderObj = javaEnv->CallObjectMethod( surfaceViewObj, mid);
	if (surfaceHolderObj == NULL)
	{
		LogError("surfaceHolder object not found!");
		return NULL;
	}

	jclass surfaceHolderClass = javaEnv->GetObjectClass(surfaceHolderObj);
	mid = javaEnv->GetMethodID(surfaceHolderClass, "getSurface", "()Landroid/view/Surface;");
	if (mid == NULL)
	{
		LogError("getSurface methodID not found!");
		return NULL;
	}

	return javaEnv->CallObjectMethod( surfaceHolderObj, mid);	
}

void SvrInitializeEvent()
{
	LogDebug("SvrInitializeEvent() called!");

	if(!plugin.isInitializeDataSet)
	{
		LogError("SvrInitializeEventData() must be called first nefore calling SvrInitializeEvent!");
		return;
	}

	svrInitParams params;
	memset(&params, 0, sizeof(params));
	params.javaActivityObject = plugin.activity;
	params.javaVm = plugin.javaVm;
	params.javaEnv = GetJNIEnv(plugin.javaVm);

	if(params.javaActivityObject == NULL)
	{
		LogError("params.javaActivityObject is NULL");	
	}

	if(params.javaVm == NULL)
	{
		LogError("params.javaVm is NULL");	
	}

	if(params.javaEnv == NULL)
	{
		LogError("params.javaEnv is NULL");		
	}
		
	if(!svrInitialize(&params))
	{
		LogError("svr initialization failed!");
		return;
	}
	
	plugin.isInitialized = true;
}

void SvrBeginVrEvent()
{
	LogDebug("SvrBeginVrEvent!");

	plugin.surfaceValidWaitFramesCounter = 0;
	
	if(!plugin.isInitialized)
	{
		LogError("Initialize svrApi with SvrInitializeEvent before calling SvrBeginVrEvent!");
		return;
	}
		
	JNIEnv* javaEnv = GetJNIEnv(plugin.javaVm);
	jobject surface = GetViewSurface(javaEnv, plugin.activity);
	ANativeWindow * nativeWindow = ANativeWindow_fromSurface(javaEnv, surface);

	svrBeginParams params;
	memset(&params, 0, sizeof(params));
    params.cpuPerfLevel = kPerfSystem;
    params.gpuPerfLevel = kPerfSystem;
	params.mainThreadId = plugin.threadTid;
	params.nativeWindow = nativeWindow;

	int ire = svrBeginVr(&params);
	if( ire != 0 ){
		LOGI("svrBeginVr return error, val:%d", ire);
	}
	
	ANativeWindow_release(nativeWindow);
}

void SvrEndVrEvent()
{
	LogDebug("SvrEndVrEvent!");
	svrEndVr();
}

void SvrEndEyeEvent()
{
	LogDebug("SvrEndEyeEvent!");
}

void SvrSubmitFrameEvent()
{
	LogDebug("SvrSubmitFrameEvent!");

	if(!plugin.isInitialized || !plugin.isFrameParamsDataSet)
		return;

	svrFrameParams params = plugin.frameParams;

	params.frameIndex = plugin.frameParams.frameIndex;
	params.headPoseState = plugin.headPoseState;
	params.eyeBufferArray[0] = plugin.frameParams.eyeBufferArray[0];
	params.eyeBufferArray[1] = plugin.frameParams.eyeBufferArray[1];
    params.eyeBufferType = kEyeBufferStereoSeparate;
    params.frameOptions = 0;
	params.minVsyncs = 1;
	params.warpType = kSimple;
	svrSubmitFrame(&params);

	plugin.isFrameParamsDataSet = false;	// reset for this frame
}

void SvrShutdownEvent()
{
	LogDebug("SvrShutdownEvent!");

	svrShutdown();	
	
	JNIEnv* javaEnv = GetJNIEnv(plugin.javaVm);
	javaEnv->DeleteGlobalRef(plugin.activity);
	plugin.activity = NULL;
}

void SvrRecenterTrackingEvent()
{
 	LogDebug("SvrRecenterTrackingEvent!");
    svrRecenterPose();
}

void SvrSetTrackingModeEvent()
{	
 	LogDebug("SvrSetTrackingModeEvent!");
	
	if(!plugin.isSetTrackingPoseDataSet)
	{
		LogError("SvrSetTrackingModeEventData() must be called first before calling SvrSetTrackingModeEvent!");
		return;
	}
	
    if(plugin.trackingPoseMode == 1)
    {
        //Orientation only
        svrSetTrackingMode( kTrackingRotation );
    }
    else if(plugin.trackingPoseMode == 2)
    {
        //Orientation and Position
        svrSetTrackingMode( kTrackingRotation | kTrackingPosition );
    }
	
	plugin.isSetTrackingPoseDataSet = false;
}

 void SvrSetPerformanceLevelsEvent()
{
	LogDebug("SvrSetPerformanceLevelsEvent!");
	
	if(!plugin.isSetPerformanceLevelsDataSet)
	{
		LogError("SvrSetPerformanceLevelsEventData() must be called first before calling SvrSetPerformanceLevelsEvent!");
		return;
	}	
	
    svrPerfLevel cpuPerfLevel = kPerfSystem;
    svrPerfLevel gpuPerfLevel = kPerfSystem;

    // SVR version of perf level
    switch (plugin.cpuPerfLevel)
    {
    case 0:
    default:
        cpuPerfLevel = kPerfSystem;
        break;

    case 1:
        cpuPerfLevel = kPerfMinimum;
        break;

    case 2:
        cpuPerfLevel = kPerfMedium;
        break;

    case 3:
        cpuPerfLevel = kPerfMaximum;
        break;
    }

    // SVR version of perf level
    switch (plugin.gpuPerfLevel)
    {
    case 0:
    default:
        gpuPerfLevel = kPerfSystem;
        break;

    case 1:
        gpuPerfLevel = kPerfMinimum;
        break;

    case 2:
        gpuPerfLevel = kPerfMedium;
        break;

    case 3:
        gpuPerfLevel = kPerfMaximum;
        break;
    }

    // Set SVR values
    svrSetPerformanceLevels(cpuPerfLevel, gpuPerfLevel);
	
	plugin.isSetPerformanceLevelsDataSet = false;
}


extern "C"
{

SVR_EXPORT bool SvrIsInitialized()
{
	return plugin.isInitialized;
}

SVR_EXPORT bool SvrCanBeginVR()
{	 	
	JNIEnv* javaEnv = GetJNIEnv(plugin.javaVm);
	jobject surface = GetViewSurface(javaEnv, plugin.activity);
	
	ANativeWindow * nativeWindow = ANativeWindow_fromSurface(javaEnv, surface);

	int currentSurfaceHeight = ANativeWindow_getHeight(nativeWindow);
	svrDeviceInfo info = svrGetDeviceInfo();
	
	if(currentSurfaceHeight != info.displayHeightPixels)
	{	
		LogDebug("Current Surface Height: %d  - DeviceInfoHeight: %d", (int)currentSurfaceHeight, info.displayHeightPixels);

		ANativeWindow_release(nativeWindow);
		return false;
	}
	
	
	LogDebug("Current Surface Height: %d  - DeviceInfoHeight: %d", (int)currentSurfaceHeight, info.displayHeightPixels);

	ANativeWindow_release(nativeWindow);
	plugin.surfaceValidWaitFramesCounter++;
	
	if(plugin.surfaceValidWaitFramesCounter >= plugin.kSurfaceValidWaitFrames)
	{
		return true;
	}
	
	return false;	
}

SVR_EXPORT void SvrInitializeEventData(jobject activity)
{
	LogDebug("SvrSetInitParams() called!");
    JNIEnv* jniEnv = GetJNIEnv(plugin.javaVm);
	plugin.threadTid = gettid();
	plugin.activity = jniEnv->NewGlobalRef(activity);
	plugin.isInitializeDataSet = true;
}

SVR_EXPORT void SvrSubmitFrameEventData(int frameIndex, int leftEyeTextureId, int rightEyeTextureId)
{
	LogDebug("SvrSubmitFrameEventData() called!");

	plugin.frameParams.frameIndex = frameIndex;
	plugin.frameParams.eyeBufferArray[0] = leftEyeTextureId;
	plugin.frameParams.eyeBufferArray[1] = rightEyeTextureId;
	plugin.isFrameParamsDataSet = true;
}

SVR_EXPORT void SvrSetTrackingModeEventData(int mode)
{
	LogDebug("SvrSetTrackingModeEventData() called!");
	
	plugin.trackingPoseMode = mode;
	plugin.isSetTrackingPoseDataSet = true;
}

SVR_EXPORT void SvrSetPerformanceLevelsEventData(int newCpuPerfLevel, int newGpuPerfLevel)
{
	LogDebug("SvrSetPerformanceLevelsEventData() called!");
	
	plugin.cpuPerfLevel = newCpuPerfLevel;
	plugin.gpuPerfLevel = newGpuPerfLevel;
	plugin.isSetPerformanceLevelsDataSet = true;
}

SVR_EXPORT void SvrGetPredictedPose(float &rotX, float &rotY, float &rotZ, float &rotW,
                                    float &posX, float &posY, float &posZ)
{
	LogDebug("SvrGetPredictedPose() called!");	
	
	float predictedTimeMs = svrGetPredictedDisplayTime();
	plugin.headPoseState = svrGetPredictedHeadPose(predictedTimeMs);
	svrHeadPose pose = plugin.headPoseState.pose;

	rotX = pose.rotation.x;
	rotY = pose.rotation.y;
	rotZ = pose.rotation.z;
	rotW = pose.rotation.w;

    posX = pose.position.x;
    posY = pose.position.y;
    posZ = pose.position.z;
}

SVR_EXPORT void SvrGetDeviceInfo(int &displayWidthPixels, int &displayHeightPixels, float &displayRefreshRateHz,
								int &targetEyeWidthPixels, int &targetEyeHeightPixels, float  &targetFovXRad,
								float &targetFovYRad)
{
	LogDebug("SvrGetDeviceInfo() called!");	
		
	svrDeviceInfo info = svrGetDeviceInfo();

	displayWidthPixels = info.displayWidthPixels;
	displayHeightPixels = info.displayHeightPixels;
	displayRefreshRateHz = info.displayRefreshRateHz;
	targetEyeWidthPixels = info.targetEyeWidthPixels;
	targetEyeHeightPixels = info.targetEyeHeightPixels;
	targetFovXRad = info.targetFovXRad;
	targetFovYRad = info.targetFovYRad;
}

enum RenderEventType
{
	InitializeEvent,
	BeginVrEvent,
	EndVrEvent,
	EndEyeEvent,
	SubmitFrameEvent,
	ShutdownEvent,
	RecenterTrackingEvent,
	SetTrackingModeEvent,
	SetPerformanceLevelsEvent
};

// Plugin function to handle a specific rendering event
static void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
    switch(eventID)
	{
	case InitializeEvent:
		SvrInitializeEvent();
		break;
	case BeginVrEvent:
		SvrBeginVrEvent();
		break;
	case EndVrEvent:
		SvrEndVrEvent();
		break;
	case EndEyeEvent:
		SvrEndEyeEvent();
		break;
	case SubmitFrameEvent:
		SvrSubmitFrameEvent();
		break;
	case ShutdownEvent:
		SvrShutdownEvent();
		break;
	case RecenterTrackingEvent:
		SvrRecenterTrackingEvent();
		break;
	case SetTrackingModeEvent:
		SvrSetTrackingModeEvent();
		break;
	case SetPerformanceLevelsEvent:
		SvrSetPerformanceLevelsEvent();
		break;		
	default:
		LogError("Event with id:%i not supported!", eventID);
		break;
	}
}

// Freely defined function to pass a callback to plugin-specific scripts
UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
    return OnRenderEvent;
}

}


#endif //_SVR_UNITY_H_
