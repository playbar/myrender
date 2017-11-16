#=============================================================================
#                  Copyright (c) 2016 QUALCOMM Technologies Inc.
#                              All Rights Reserved.
#
#=============================================================================

LOCAL_PATH := $(call my-dir)

# Use this to enable/disable Sensor Service
ENABLE_QVR_SERVICE := true

# Use this to enable/disable Motion To Photon Test
ENABLE_MOTION_PHOTON_TEST := false

# Use this to enable/disable Telemetry profiling support
ENABLE_TELEMETRY := false

# Use this to enable/disable ATrace profiling support
ENABLE_ATRACE := false

include $(CLEAR_VARS)
LOCAL_MODULE    := libSymphony
LOCAL_SRC_FILES := ../../3rdparty/symphony/lib/libsymphony-cpu.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libSymphonyPower
LOCAL_SRC_FILES := ../../3rdparty/symphony/lib/libsymphonypower-1.1.1.a
include $(PREBUILT_STATIC_LIBRARY)

ifeq ($(ENABLE_TELEMETRY),true)
include $(CLEAR_VARS)
LOCAL_MODULE    := libTelemetry
LOCAL_SRC_FILES := ../../3rdparty/telemetry/lib/libTelemetryAndroid.a
include $(PREBUILT_STATIC_LIBRARY)
endif

ifeq ($(ENABLE_QVR_SERVICE),true)
include $(CLEAR_VARS)
QVR_SERVICE_DIR := $(LOCAL_PATH)/../../3rdparty/qvr/lib
LOCAL_MODULE := libqvrservice_client
LOCAL_SRC_FILES := ../../3rdparty/qvr/lib/libqvrservice_client.so
LOCAL_EXPORT_LDLIBS := -lqvrservice_client
LOCAL_EXPORT_LDFLAGS := -L$(QVR_SERVICE_DIR)
include $(PREBUILT_SHARED_LIBRARY)
endif

include $(CLEAR_VARS)
ifeq ($(ENABLE_QVR_SERVICE),true)
	LOCAL_CFLAGS += -DUSE_QVR_SERVICE
endif

ifeq ($(ENABLE_MOTION_PHOTON_TEST),true)
	LOCAL_CFLAGS += -DMOTION_TO_PHOTON_TEST
endif

ifeq ($(ENABLE_TELEMETRY),true)
	LOCAL_CFLAGS += -DSVR_PROFILING_ENABLED
	LOCAL_CFLAGS += -DSVR_PROFILE_TELEMETRY
endif 

ifeq ($(ENABLE_ATRACE),true)
	LOCAL_CFLAGS += -DSVR_PROFILING_ENABLED
	LOCAL_CFLAGS += -DSVR_PROFILE_ATRACE
endif 

SDK_ROOT_PATH := $(LOCAL_PATH)/../..

SVR_FRAMEWORK_PATH		:= $(SDK_ROOT_PATH)/framework
GLM_ROOT_PATH			:= $(SDK_ROOT_PATH)/3rdparty/glm-0.9.7.0
TINYOBJ_ROOT_PATH		:= $(SDK_ROOT_PATH)/3rdparty/tinyobj
CJSON_ROOT_PATH			:= $(SDK_ROOT_PATH)/3rdparty/cJSON
SVR_API_PUBLIC_PATH		:= $(SDK_ROOT_PATH)/svrApi/public
SVR_API_PRIVATE_PATH	:= $(SDK_ROOT_PATH)/svrApi
TELEMETRY_ROOT_PATH		:= $(SDK_ROOT_PATH)/3rdparty/telemetry
SYMPHONY_ROOT_PATH		:= $(SDK_ROOT_PATH)/3rdparty/symphony
QCOM_ADSP_ROOT_PATH		:= $(SDK_ROOT_PATH)/3rdparty/qcom_adsp
QCOM_QVR_SERVICE_PATH	:= $(SDK_ROOT_PATH)/3rdparty/qvr


LOCAL_MODULE := svrapi
LOCAL_ARM_MODE := arm
LOCAL_C_INCLUDES := $(GLM_ROOT_PATH) $(TINYOBJ_ROOT_PATH) $(CJSON_ROOT_PATH) $(SVR_FRAMEWORK_PATH) $(SVR_API_PUBLIC_PATH) $(SVR_API_PRIVATE_PATH) $(TELEMETRY_ROOT_PATH)/include $(SYMPHONY_ROOT_PATH)/include $(QCOM_ADSP_ROOT_PATH)/include $(QCOM_QVR_SERVICE_PATH)/inc

#Make sure svrApiVersion is compiled every time
LOCAL_SRC_FILES += ../private/svrApiVersion.cpp
.PHONY: $(LOCAL_PATH)/../private/svrApiVersion.cpp

LOCAL_SRC_FILES += ../private/svrApiCore.cpp ../private/svrApiTimeWarp.cpp ../private/svrApiTimeWarpShaders.cpp ../private/svrApiSensor.cpp ../private/svrApiHelper.cpp  ../private/svrApiHiddenMesh.cpp 
LOCAL_SRC_FILES += ../private/svrApiDebugServer.cpp

ifeq ($(ENABLE_QVR_SERVICE),true)
LOCAL_SRC_FILES += ../private/svrApiPredictiveSensor.cpp
endif

LOCAL_SRC_FILES += ../../framework/svrConfig.cpp ../../framework/svrContainers.cpp
LOCAL_SRC_FILES += ../../framework/svrGeometry.cpp ../../framework/svrShader.cpp ../../framework/svrUtil.cpp ../../framework/svrRenderTarget.cpp
LOCAL_SRC_FILES += ../../framework/svrKtxLoader.cpp ../../framework/svrProfile.cpp
LOCAL_SRC_FILES += ../../framework/svrCpuTimer.cpp ../../framework/svrGpuTimer.cpp
LOCAL_SRC_FILES += ../../framework/svrRenderExt.cpp

LOCAL_SRC_FILES += ../../3rdparty/tinyobj/tiny_obj_loader.cc
LOCAL_SRC_FILES += ../../3rdparty/cJSON/cJSON.c

LOCAL_CPPFLAGS += -Wall -fno-strict-aliasing -Wno-unused-but-set-variable 

LOCAL_STATIC_LIBRARIES := Symphony SymphonyPower Telemetry cpufeatures qvrservice_client

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)







