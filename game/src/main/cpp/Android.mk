# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)

# 通用 C 编译选项
COMMON_CFLAGS	= -DANDROID_NDK
COMMON_CFLAGS	+= -Wall
COMMON_CFLAGS	+= -Werror=return-type
COMMON_CFLAGS	+= -Wextra
COMMON_CFLAGS	+= -Winvalid-offsetof
#COMMON_CFLAGS	+= -Wlogical-op		# not part of -Wall or -Wextra
#COMMON_CFLAGS	+= -Weffc++			# too many issues to fix for now
COMMON_CFLAGS	+= -Wno-reorder
COMMON_CFLAGS	+= -Wno-unused-variable
COMMON_CFLAGS	+= -Wno-unused-function
COMMON_CFLAGS	+= -Wno-unused-private-field
COMMON_CFLAGS	+= -Wno-strict-aliasing		# TODO: need to rewrite some code
COMMON_CFLAGS	+= -Wno-unused-parameter
COMMON_CFLAGS	+= -Wno-missing-field-initializers	# warns on this: SwipeAction	ret = {}
COMMON_CFLAGS	+= -Wno-multichar	# used in internal Android headers:  DISPLAY_EVENT_VSYNC = 'vsyn',
ifeq ($(NDK_DEBUG),1)
  COMMON_CFLAGS += -O0 -g
else
  COMMON_CFLAGS += -O2
endif
# 通用 C++ 编译选项
COMMON_CPPFLAGS = -Wno-type-limits
#COMMON_CPPFLAGS += -Wno-invalid-offsetof
COMMON_CPPFLAGS	+= -std=c++11

##################################################################################
# 3rd part
##################################################################################

#googlebreakpad
include $(LOCAL_PATH)/googlebreakpad/Android.mk

#curl
include $(CLEAR_VARS)
LOCAL_MODULE := libcurl
LOCAL_SRC_FILES := 3rdPart/Curl/lib/$(TARGET_ARCH_ABI)/libcurl.so
#$(warning $(LOCAL_SRC_FILES))
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
include $(PREBUILT_SHARED_LIBRARY)

#log4cplus
include $(CLEAR_VARS)
LOCAL_MODULE := libmdklog
LOCAL_SRC_FILES := 3rdPart/log4cplus/$(TARGET_ARCH_ABI)/libmdklog.so		   
include $(PREBUILT_SHARED_LIBRARY)

#sqlite3
include $(CLEAR_VARS)
LOCAL_MODULE := libsqlite3
LOCAL_SRC_FILES := 3rdPart/sqlite3/$(TARGET_ARCH_ABI)/libsqlite3.so		   
include $(PREBUILT_SHARED_LIBRARY)

#protobuf
#include $(CLEAR_VARS)
#LOCAL_MODULE := libprotobuf
#LOCAL_SRC_FILES := 3rdPart/Protobuf/lib/$(TARGET_ARCH_ABI)/libprotobuf.so
##include $(PREBUILT_STATIC_LIBRARY)
#include $(PREBUILT_SHARED_LIBRARY)

#libglHelper
include $(CLEAR_VARS)

LOCAL_MODULE := glHelper

LOCAL_ARM_MODE  := arm				# full speed arm instead of thumb
LOCAL_ARM_NEON  := true				# compile with neon support enabled

LOCAL_CFLAGS    := $(COMMON_CFLAGS)
LOCAL_CPPFLAGS  := $(COMMON_CPPFLAGS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/3rdPart/ktx/include \
					$(LOCAL_PATH)/3rdPart/ktx/other_include \

LOCAL_SRC_PATH := 3rdPart/ktx/lib
LOCAL_SRC_FILES :=	$(LOCAL_SRC_PATH)/checkheader.c \
					$(LOCAL_SRC_PATH)/errstr.c \
					$(LOCAL_SRC_PATH)/etcdec.cxx \
					$(LOCAL_SRC_PATH)/etcunpack.cxx \
					$(LOCAL_SRC_PATH)/hashtable.c \
					$(LOCAL_SRC_PATH)/loader.c \
					$(LOCAL_SRC_PATH)/swap.c \
					$(LOCAL_SRC_PATH)/writer.c \
					$(LOCAL_SRC_PATH)/ktxfilestream.c \
					$(LOCAL_SRC_PATH)/ktxmemstream.c \
					$(LOCAL_SRC_PATH)/KtxLoader.cpp \

					LOCAL_LDLIBS    := -lGLESv2 -lEGL

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)

##################################################################################
# libmojing
##################################################################################
include $(CLEAR_VARS)


MOJING_SRC_FILES = MojingAPI.cpp \
					MojingManager.cpp \
					MojingSDKStatus.cpp \
					Tracker/MojingGlassSensor.cpp \
					3rdPart/AES/AES.cpp \
					3rdPart/AES/AESPro.cpp \
					3rdPart/MD5/MD5.cpp 
#					3rdPart/Protobuf/CardboardDevice.pb.cc

# miniZip
MOJING_SRC_FILES += 3rdPart/minizip/ioapi.c \
					3rdPart/minizip/miniunz.c \
					3rdPart/minizip/minizip.c \
					3rdPart/minizip/mztools.c \
					3rdPart/minizip/unzip.c \
					3rdPart/minizip/zip.c 

# log
MOJING_SRC_FILES += 3rdPart/log4cplus/LogInterface.cpp

# Base
MOJING_SRC_FILES += Base/Base32.cpp \
					Base/Base64.cpp \
					Base/CRC.cpp \
					Base/GlUtils.cpp \
					Base/MojingAlg.cpp \
					Base/MojingAllocator.cpp \
					Base/MojingAtomic.cpp \
					Base/MojingCompress.cpp \
					Base/MojingFile.cpp \
					Base/MojingFileFILE.cpp \
					Base/MojingJSON.cpp \
					Base/MojingLog.cpp \
					Base/MojingMath.cpp \
					Base/MojingRefCount.cpp \
					Base/MojingStd.cpp \
					Base/MojingString.cpp \
					Base/MojingString_FormatUtil.cpp \
					Base/MojingSysFile.cpp \
					Base/MojingSystem.cpp \
					Base/MojingThreadsPthread.cpp \
					Base/MojingTimer.cpp \
					Base/MojingUTF8Util.cpp \
					Base/MojingLoadSo.cpp

# MemoryCheck 
# MOJING_SRC_FILES += Base/MojingMemoryCheck.cpp 

# Distortion
MOJING_SRC_FILES += Distortion/MojingDistortion.cpp \
					Distortion/GVR/GvrProfile.cpp

# Hook
MOJING_SRC_FILES += Hook/Global/detour.cpp \
					Hook/Global/instruction.cpp \
					Hook/Global/HookBase.cpp \
					Hook/HookGVR/HookGVRTools.cpp \
					Hook/eglhook/elf_common.cpp \
					Hook/eglhook/elf_hooker.cpp \
					Hook/eglhook/elf_eglhook.cpp \
					Hook/eglhook/elf_module.cpp \
					Hook/waithook.cpp \

# Interface
MOJING_SRC_FILES += Interface/Android/MojingAndroidAPI.cpp \
					Interface/Android/MojingNativeInput.cpp \
					Interface/Unreal/MojingUnrealAPI.cpp \
					Interface/Unity/GlStateSave.cpp \
					Interface/Unity/UnityPlugin.cpp \
					Interface/Unity/UnityPluginInterface/UnityPluginStatus.cpp \
					Interface/Unity/UnityPluginInterface/UnityPluginInterfaceBase.cpp \
					Interface/Unity/UnityPluginInterface/UnityPluginInterfaceQ820.cpp \
					Interface/Gear/MojingGearAPI.cpp


# Parameters
MOJING_SRC_FILES += Parameters/MojingDeviceParameters.cpp \
					Parameters/MojingDisplayParameters.cpp \
					Parameters/MojingFactoryCalibrationParameters.cpp \
					Parameters/MojingGyroOffsetReport.cpp \
					Parameters/MojingGyroOffsetReportSet.cpp \
					Parameters/MojingJsonElement.cpp \
					Parameters/MojingParameters.cpp \
					Parameters/MojingSensorParameters.cpp 

# Platform
MOJING_SRC_FILES += Platform/MojingPlatformBase.cpp 

# Profile
MOJING_SRC_FILES += Profile/FileProfile.cpp \
				   Profile/GlassesConfigProfile.cpp \
				   Profile/GlassesConfigProfileV2.cpp \
				   Profile/InternetProfile.cpp \
				   Profile/JoystickProfile.cpp \
				   Profile/MobileConfigProfile.cpp \
				   Profile/DebugSettingsProfile.cpp \
				   Profile/MojingProfileKey.cpp \
				   Profile/ProfileThreadMGR.cpp \
				   Profile/ProfileV2/DayDreamParameters.cpp \
				   Profile/ProfileV2/GlassInfo.cpp \
				   Profile/ProfileV2/ManufacturerInfo.cpp \
				   Profile/ProfileV2/ProductInfo.cpp \
				   Profile/UserSettingProfile.cpp \
				   Profile/UnrealJoystickProfile.cpp  

# Render
MOJING_SRC_FILES += Render/RenderFrame.cpp \
				   Render/RenderFrameCirular.cpp \
				   Render/RenderFrameManager.cpp \
				   Render/Vsync.cpp \
				   3rdPart/IntelVSync/IntelVSync.cpp \
				   Render/GlGeometry.cpp \
				   Render/GLProgram.cpp \
				   Render/EyeTextureParam.cpp \
				   Render/LineDraw.cpp \
				   Render/MojingRenderBase.cpp \
				   Render/MojingRenderMultithread.cpp \
				   Render/MojingRenderMultiThread_THREAD.cpp \
				   Render/MultiThreadGLParam.cpp \
				   Render/MojingRenderMultiThread_FB.cpp
			   
# Reporter
MOJING_SRC_FILES += Reporter/ActiveTimeInfoReporter.cpp \
					Reporter/CrashReporter.cpp \
					Reporter/DatabaseInfoReporter.cpp \
					Reporter/EventInfoReporter.cpp \
					Reporter/GyroTempCalibrationReporter.cpp \
					Reporter/InfoReporter.cpp \
					Reporter/LogInfoReporter.cpp \
					Reporter/MobileConfigUpdate.cpp \
					Reporter/MojingMerchantVerify.cpp \
					Reporter/PageInfoReporter.cpp \
					Reporter/ReporterTools.cpp \
					Reporter/RunInfoReporter.cpp \
					Reporter/UserActionReporter.cpp

# Tracker
MOJING_SRC_FILES += Tracker/AndroidInternalSensorChecker.cpp \
					Tracker/MojingAndroidSensor.cpp \
					Tracker/MojingGyroTempCalibration.cpp \
					Tracker/MojingSensor.cpp \
					Tracker/MojingSensorCalibration.cpp \
					Tracker/MojingSensorFilter.cpp \
					Tracker/MojingSensorFusion.cpp \
					Tracker/MojingTemperature.cpp \
					Tracker/MojingTracker.cpp \
					Tracker/MojingControllerSocket.cpp 

# TextureBacker
MOJING_SRC_FILES += TextureBacker/FBO.cpp TextureBacker/Shader.cpp TextureBacker/TextBaker.cpp


# libmojing GLES 2
LOCAL_MODULE    := libmojing

LOCAL_ARM_MODE  := arm				# full speed arm instead of thumb
LOCAL_ARM_NEON  := true				# compile with neon support enabled

LOCAL_CFLAGS    := $(COMMON_CFLAGS)
# LOCAL_CFLAGS 	+= -D GOOGLE_PROTOBUF_NO_RTTI=1

LOCAL_CPPFLAGS  := $(COMMON_CPPFLAGS)
LOCAL_CPPFLAGS 	+= -fexceptions

LOCAL_C_INCLUDES := $(LOCAL_PATH)/3rdPart/log4cplus/include 
#					$(LOCAL_PATH)/3rdPart/Protobuf/include
					
LOCAL_SRC_FILES := $(MOJING_SRC_FILES)
LOCAL_SRC_FILES += Render/MojingRenderMultiThread_3288.cpp \
				   	3rdPart/3288VSync/C3288VSync.cpp \
				   	3rdPart/3288Overlay/C3288Overlay.cpp \
				   	3rdPart/3288Overlay/RenderOverlay3288.cpp\
					3rdPart/Qualcomm/CSVRApi.cpp

LOCAL_LDLIBS    := -llog -lGLESv2 -lEGL -landroid -lz
LOCAL_STATIC_LIBRARIES := breakpad_client 
LOCAL_SHARED_LIBRARIES := libcurl libsqlite3 

include $(BUILD_SHARED_LIBRARY)

##################################################################################
# libOverlay
##################################################################################
#include $(CLEAR_VARS)
#LOCAL_MODULE := libOverlay
#LOCAL_SRC_FILES := 3rdPart/3288Overlay/lib/libOverlay.so	   
#include $(PREBUILT_SHARED_LIBRARY)


##################################################################################
# libsvrapi
##################################################################################
# include $(CLEAR_VARS)
# LOCAL_MODULE := libsvrapi
# LOCAL_SRC_FILES := 3rdPart/Qualcomm/lib/libsvrapi.so
# include $(PREBUILT_SHARED_LIBRARY)


##################################################################################
# libqvrservice_client
##################################################################################
# include $(CLEAR_VARS)
# LOCAL_MODULE := libqvrservice_client
# LOCAL_SRC_FILES := 3rdPart/Qualcomm/lib/libqvrservice_client.so
# include $(PREBUILT_SHARED_LIBRARY)

##################################################################################
# libmojingvrrom
##################################################################################
#include $(CLEAR_VARS)
#
#LOCAL_MODULE    := libmojingvrrom
#
#LOCAL_ARM_MODE  := arm				# full speed arm instead of thumb
#LOCAL_ARM_NEON  := true				# compile with neon support enabled
#
#LOCAL_CFLAGS    := $(COMMON_CFLAGS)
#LOCAL_CPPFLAGS  := $(COMMON_CPPFLAGS)
#
#LOCAL_C_INCLUDES := $(LOCAL_PATH)/Render/Model \
#					$(LOCAL_PATH)/googlebreakpad/src
#
#LOCAL_SRC_FILES := Interface/Android/MojingVRRom.cpp \
#					Interface/Android/MojingVRRomAPI.cpp \
#					Render/Models/Model.cpp \
#				   	Render/Models/SkyboxModel.cpp \
#				   	Render/Models/SphereModel.cpp \
#					Render/Models/RectangleModel.cpp
#
#LOCAL_LDLIBS    := -lGLESv2
#LOCAL_SHARED_LIBRARIES := libmojing
#include $(BUILD_SHARED_LIBRARY)

# libmojing GLES 3
#include $(CLEAR_VARS)

#LOCAL_MODULE    := libmojing_gles3

#LOCAL_ARM_MODE  := arm				# full speed arm instead of thumb
#LOCAL_ARM_NEON  := true				# compile with neon support enabled

#LOCAL_CFLAGS    := $(COMMON_CFLAGS)
#LOCAL_CPPFLAGS  := $(COMMON_CPPFLAGS)
#LOCAL_CPPFLAGS	+= -DRK3288=1

#LOCAL_C_INCLUDES := $(LOCAL_PATH)/3rdPart/log4cplus/include
#LOCAL_SRC_FILES := $(MOJING_SRC_FILES)
#LOCAL_SRC_FILES += Render/MojingRenderMultiThread_3288.cpp \
#					Render/JNI.cpp \
#					Render/Models/Model.cpp \
#				   	Render/Models/SkyboxModel.cpp \
#				   	Render/Models/SphereModel.cpp \
#				   	3rdPart/3288VSync/C3288VSync.cpp \
#				   	3rdPart/3288Overlay/C3288Overlay.cpp \
#				   	3rdPart/3288Overlay/RenderOverlay3288.cpp

#LOCAL_LDLIBS    := -llog -lGLESv3 -lEGL -landroid -lz -lsqlite
#LOCAL_STATIC_LIBRARIES := breakpad_client ktx
#LOCAL_SHARED_LIBRARIES := libcurl
#include $(BUILD_SHARED_LIBRARY)
