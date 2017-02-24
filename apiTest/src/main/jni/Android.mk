#=============================================================================
#                  Copyright (c) 2016 QUALCOMM Technologies Inc.
#                              All Rights Reserved.
#
#=============================================================================
LOCAL_PATH := $(call my-dir)
  
include $(CLEAR_VARS)
LOCAL_MODULE := libsvrapi
LOCAL_SRC_FILES := ../../../svrApi/libs/armeabi-v7a/libsvrapi.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

SDK_ROOT_PATH := $(LOCAL_PATH)/../../..

SVR_SVRAPI_PATH = : $(SDK_ROOT_PATH)/svrApi
SVR_FRAMEWORK_PATH = : $(SDK_ROOT_PATH)/framework
GLM_ROOT_PATH := $(SDK_ROOT_PATH)/3rdparty/glm-0.9.7.0
IMGUI_ROOT_PATH := $(SDK_ROOT_PATH)/3rdparty/imgui
TINYOBJ_ROOT_PATH := $(SDK_ROOT_PATH)/3rdparty/tinyobj
SVR_API_ROOT_PATH := $(SDK_ROOT_PATH)/svrApi/public
TELEMETRY_ROOT_PATH		:= $(SDK_ROOT_PATH)/3rdparty/telemetry

LOCAL_MODULE    := svrapitest
LOCAL_ARM_MODE	:= arm

LOCAL_C_INCLUDES  := $(GLM_ROOT_PATH) $(IMGUI_ROOT_PATH) $(SVR_SVRAPI_PATH) $(SVR_FRAMEWORK_PATH) $(TINYOBJ_ROOT_PATH) $(SVR_API_ROOT_PATH) $(TELEMETRY_ROOT_PATH)/include

LOCAL_SRC_FILES += ../../../framework/svrConfig.cpp ../../../framework/svrContainers.cpp
LOCAL_SRC_FILES += ../../../framework/svrApplication.cpp ../../../framework/svrAndroidMain.cpp ../../../framework/svrInput.cpp
LOCAL_SRC_FILES += ../../../framework/svrGeometry.cpp ../../../framework/svrShader.cpp ../../../framework/svrUtil.cpp ../../../framework/svrRenderTarget.cpp
LOCAL_SRC_FILES += ../../../framework/svrCpuTimer.cpp
LOCAL_SRC_FILES += ../../../3rdparty/imgui/imgui.cpp
LOCAL_SRC_FILES += ../../../3rdparty/tinyobj/tiny_obj_loader.cc
LOCAL_SRC_FILES += app.cpp

LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv3
LOCAL_SHARED_LIBRARIES := libsvrapi

LOCAL_WHOLE_STATIC_LIBRARIES += android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
