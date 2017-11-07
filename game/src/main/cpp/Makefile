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


# Define macros for name of compiler
CC = gcc

# args
RELEASE = 1

# flags
CFLAGS = 
LFLAGS =

# [args] 生成模式. 1代表release模式. 0代表debug模式, make RELEASE=0.
ifeq ($(RELEASE),0)
    # debug
    CFLAGS += -Wall -g
else
    # release
    CFLAGS +=  -O3 -DNDEBUG
endif

LFLAGS  += -lstdc++ -shared

LOCAL_SRC_FILES = 3rdPart/AES/AES.cpp 3rdPart/AES/AESPro.cpp 3rdPart/MD5/MD5.cpp 3rdPart/sqlite3/sqlite3.c LogTraker/LogInterface.cpp MojingAPI.cpp MojingManager.cpp MojingSDKStatus.cpp \
									3rdPart/minizip/ioapi.c 3rdPart/minizip/miniunz.c 3rdPart/minizip/minizip.c 3rdPart/minizip/mztools.c 3rdPart/minizip/unzip.c 3rdPart/minizip/zip.c \
									Base/Base32.cpp Base/CRC.cpp Base/MojingAlg.cpp Base/MojingAllocator.cpp Base/MojingAtomic.cpp Base/MojingCompress.cpp Base/MojingFile.cpp Base/MojingFileFILE.cpp \
									Base/MojingJSON.cpp Base/MojingLog.cpp Base/MojingMath.cpp Base/MojingRefCount.cpp Base/MojingStd.cpp Base/MojingString.cpp Base/MojingString_FormatUtil.cpp Base/GlUtils.cpp\
									Base/MojingSysFile.cpp Base/MojingSystem.cpp Base/MojingTimer.cpp Base/MojingUTF8Util.cpp Base/MojingThreadsPthread.cpp Distortion/MojingDistortion.cpp Distortion/GVR/GvrProfile.cpp\
									Parameters/MojingDeviceParameters.cpp Parameters/MojingDisplayParameters.cpp Parameters/MojingFactoryCalibrationParameters.cpp Parameters/MojingGyroOffsetReport.cpp \
									Parameters/MojingGyroOffsetReportSet.cpp Parameters/MojingJsonElement.cpp Parameters/MojingParameters.cpp Parameters/MojingSensorParameters.cpp Platform/MojingPlatformBase.cpp \
									Reporter/ActiveTimeInfoReporter.cpp Reporter/CrashReporter.cpp Reporter/DatabaseInfoReporter.cpp Reporter/EventInfoReporter.cpp Reporter/InfoReporter.cpp Reporter/RunInfoReporter.cpp \
									Reporter/LogInfoReporter.cpp Reporter/MobileConfigUpdate.cpp Reporter/MojingMerchantVerify.cpp Reporter/PageInfoReporter.cpp Reporter/ReporterTools.cpp  Reporter/GyroTempCalibrationReporter.cpp \
									Profile/FileProfile.cpp Profile/GlassesConfigProfile.cpp Profile/GlassesConfigProfileV2.cpp Profile/InternetProfile.cpp Profile/MobileConfigProfile.cpp \
									Profile/MojingProfileKey.cpp Profile/ProfileThreadMGR.cpp Profile/ProfileV2/GlassInfo.cpp Profile/ProfileV2/ManufacturerInfo.cpp Profile/ProfileV2/ProductInfo.cpp \
									Profile/UserSettingProfile.cpp Profile/JoystickProfile.cpp Profile/DebugSettingsProfile.cpp Profile/ProfileV2/DayDreamParameters.cpp \
									Tracker/MojingGyroTempCalibration.cpp Tracker/MojingSensor.cpp Tracker/MojingSensorCalibration.cpp Tracker/MojingSensorFilter.cpp Tracker/MojingSensorFusion.cpp \
									Tracker/MojingTemperature.cpp Tracker/MojingTracker.cpp Render/EyeTextureParam.cpp Render/LineDraw.cpp Render/MojingRenderBase.cpp Tracker/MojingWinSensor.cpp\
									Render/RenderFrame.cpp Render/RenderFrameCirular.cpp Render/RenderFrameManager.cpp Render/Vsync.cpp Render/GlGeometry.cpp Render/GLProgram.cpp \
									Interface/Unreal/MojingUnrealAPI.cpp Interface/Unity/GlStateSave.cpp Interface/Unity/UnityPlugin.cpp \
									TextureBacker/FBO.cpp TextureBacker/Shader.cpp TextureBacker/TextBaker.cpp \
									Interface/Unity/UnityPluginInterface/UnityPluginInterfaceBase.cpp Interface/Unity/UnityPluginInterface/UnityPluginInterfaceWin32.cpp Interface/Unity/UnityPluginInterface/UnityPluginStatus.cpp
									#Interface/Windows/MojingWinAPI.cpp	Render/MultiThreadGLParam.cpp	Render/MojingRenderMultiThread_THREAD.cpp		Render/MojingRenderMultithread.cpp	Interface/Unity/UnityWinAPI.cpp 
									
													
							
LOCAL_OBJ_FILES = AES.o AESPro.o MD5.o ioapi.o miniunz.o minizip.o mztools.o unzip.o zip.o sqlite3.o Base32.o CRC.o GlUtils.o MojingAlg.o MojingAllocator.o MojingAtomic.o MojingCompress.o \
									MojingFile.o MojingFileFILE.o MojingJSON.o MojingLog.o MojingMath.o MojingRefCount.o MojingStd.o MojingString.o MojingString_FormatUtil.o MojingSysFile.o MojingSystem.o \
									MojingThreadsPthread.o MojingTimer.o MojingUTF8Util.o MojingDistortion.o GvrProfile.o MojingAPI.o MojingManager.o MojingSDKStatus.o MojingDeviceParameters.o MojingDisplayParameters.o \
									MojingFactoryCalibrationParameters.o MojingGyroOffsetReport.o MojingGyroOffsetReportSet.o MojingJsonElement.o MojingParameters.o MojingSensorParameters.o LogInterface.o \
									MojingPlatformBase.o FileProfile.o GlassesConfigProfile.o GlassesConfigProfileV2.o InternetProfile.o MobileConfigProfile.o MojingProfileKey.o ProfileThreadMGR.o GlassInfo.o \
									ManufacturerInfo.o ProductInfo.o EyeTextureParam.o LineDraw.o MojingRenderBase.o  RenderFrame.o DayDreamParameters.o \
									RenderFrameCirular.o RenderFrameManager.o Vsync.o ActiveTimeInfoReporter.o CrashReporter.o DatabaseInfoReporter.o EventInfoReporter.o GyroTempCalibrationReporter.o \
									InfoReporter.o MobileConfigUpdate.o MojingMerchantVerify.o PageInfoReporter.o ReporterTools.o RunInfoReporter.o LogInfoReporter.o MojingGyroTempCalibration.o MojingSensor.o \
									MojingWinSensor.o MojingSensorCalibration.o MojingSensorFilter.o MojingSensorFusion.o MojingTemperature.o MojingTracker.o GlStateSave.o GlGeometry.o GLProgram.o \
									UserSettingProfile.o JoystickProfile.o MojingUnrealAPI.o UnityPlugin.o DebugSettingsProfile.o \
									FBO.o Shader.o TextBaker.o \
									UnityPluginInterfaceBase.o UnityPluginInterfaceWin32.o UnityPluginStatus.o
									#MojingWinAPI.o MojingRenderMultithread.o MojingRenderMultiThread_THREAD.o MultiThreadGLParam.o UnityWinAPI.o 
							 

LOCAL_MODULE   = libmojing.dll
LOCAL_CURL_PATH = 3rdPart/Curl/lib/x64/libcurl.lib
LOCAL_ZLIB_PATH = 3rdPart/zlib/x64/libz.a
LOCAL_LOG4CPLUS_PATH = LogTraker/x64/liblog4cplus.dll.a

SYS_LIBS = -lpthread -lwinmm -lglew32 -lopengl32


TAGET:
#ifeq ($(RELEASE),0)
#			cp Base\BuildTypeDebug.h Base\BuildType.h
#else
#			cp Base\BuildTypeRelease.h Base\BuildType.h
#endif

			$(CC) -c $(CFLAGS) $(LOCAL_SRC_FILES)
			g++ -o $(LOCAL_MODULE) $(LOCAL_OBJ_FILES) $(LFLAGS) $(LOCAL_GL_LIBPATH) $(LOCAL_CURL_PATH) $(LOCAL_LOG4CPLUS_PATH) $(LOCAL_ZLIB_PATH) $(SYS_LIBS) 



clean:
		rm -f $(LOCAL_OBJ_FILES) $(LOCAL_MODULE)

