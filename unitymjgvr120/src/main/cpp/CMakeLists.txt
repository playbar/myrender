cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                    -std=c++11 \
                    -Wall \
                    -O2 -g \
                    -fexceptions \
                    -DUSE_QVR_SERVICE \
                    -DNDEBUG \
                    -D_DEBUG \
                    -DMJ_OS_ANDROID")

#SET(USE_QVR_SERVICE true)

##################################################
#mojing so
MESSAGE(STATUS "Project: mojing")

add_library(sqlite_lib SHARED IMPORTED )
set_target_properties(sqlite_lib PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libsqlite3.so)

add_library(curl_lib SHARED IMPORTED)
set_target_properties(curl_lib  PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libcurl.so)


#make breakpad_client.a
add_library(breakpad_client STATIC IMPORTED)
set_target_properties(breakpad_client PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libbreakpad_client.a)


#make mojing.so

add_library(mojing SHARED
            MojingAPI.cpp
            MojingManager.cpp
            MojingSDKStatus.cpp
            Tracker/MojingGlassSensor.cpp
            3rdPart/AES/AES.cpp
            3rdPart/AES/AESPro.cpp
            3rdPart/MD5/MD5.cpp
            3rdPart/minizip/ioapi.c
            3rdPart/minizip/miniunz.c
            3rdPart/minizip/minizip.c
            3rdPart/minizip/mztools.c
            3rdPart/minizip/unzip.c
            3rdPart/minizip/zip.c
            3rdPart/log4cplus/LogInterface.cpp
            Base/Base32.cpp
            Base/Base64.cpp
			Base/CRC.cpp
			Base/GlUtils.cpp
			Base/MojingAlg.cpp
			Base/MojingAllocator.cpp
			Base/MojingAtomic.cpp
			Base/MojingCompress.cpp
			Base/MojingFile.cpp
			Base/MojingFileFILE.cpp
			Base/MojingJSON.cpp
			Base/MojingLog.cpp
			Base/MojingMath.cpp
			Base/MojingRefCount.cpp
			Base/MojingStd.cpp
			Base/MojingString.cpp
			Base/MojingString_FormatUtil.cpp
			Base/MojingSysFile.cpp
			Base/MojingSystem.cpp
			Base/MojingThreadsPthread.cpp
			Base/MojingTimer.cpp
			Base/MojingUTF8Util.cpp
			Base/MojingLoadSo.cpp
            Distortion/MojingDistortion.cpp
            Distortion/GVR/GvrProfile.cpp
            Hook/Global/detour.cpp
            Hook/Global/instruction.cpp
            Hook/Global/HookBase.cpp
            Hook/HookGVR/HookGVRTools.cpp
            Hook/eglhook/elf_common.cpp
            Hook/eglhook/elf_hooker.cpp
            Hook/eglhook/elf_eglhook.cpp
            Hook/eglhook/elf_module.cpp
            Hook/drawtex.cpp
            Hook/waithook.cpp
            Interface/Android/MojingAndroidAPI.cpp
			Interface/Android/MojingNativeInput.cpp
			Interface/Unreal/MojingUnrealAPI.cpp
			Interface/Gear/MojingGearAPI.cpp
			Interface/Unity/GlStateSave.cpp
			Interface/Unity/UnityPlugin.cpp
			Interface/Unity/UnityPluginInterface/UnityPluginStatus.cpp
			Interface/Unity/UnityPluginInterface/UnityPluginInterfaceBase.cpp
			Interface/Unity/UnityPluginInterface/UnityPluginInterfaceQ820.cpp
            Parameters/MojingDeviceParameters.cpp
			Parameters/MojingDisplayParameters.cpp
			Parameters/MojingFactoryCalibrationParameters.cpp
			Parameters/MojingGyroOffsetReport.cpp
			Parameters/MojingGyroOffsetReportSet.cpp
			Parameters/MojingJsonElement.cpp
			Parameters/MojingParameters.cpp
			Parameters/MojingSensorParameters.cpp
			Platform/MojingPlatformBase.cpp
			Profile/FileProfile.cpp
		   Profile/GlassesConfigProfile.cpp
		   Profile/GlassesConfigProfileV2.cpp
		   Profile/InternetProfile.cpp
		   Profile/JoystickProfile.cpp
		   Profile/MobileConfigProfile.cpp
		   Profile/DebugSettingsProfile.cpp
		   Profile/MojingProfileKey.cpp
		   Profile/ProfileThreadMGR.cpp
		   Profile/ProfileV2/GlassInfo.cpp
		   Profile/ProfileV2/ManufacturerInfo.cpp
		   Profile/ProfileV2/ProductInfo.cpp
		   Profile/UserSettingProfile.cpp
		   Profile/UnrealJoystickProfile.cpp
		   Profile/ProfileV2/DayDreamParameters.cpp
		   Profile/ProfileV2/GlassInfo.cpp
		   Profile/ProfileV2/ManufacturerInfo.cpp
		   Profile/ProfileV2/ProductInfo.cpp

           Render/RenderFrame.cpp
		   Render/RenderFrameCirular.cpp
		   Render/RenderFrameManager.cpp
		   Render/Vsync.cpp
		   3rdPart/IntelVSync/IntelVSync.cpp
		   Render/GlGeometry.cpp
		   Render/GLProgram.cpp
		   Render/EyeTextureParam.cpp
		   Render/LineDraw.cpp
		   Render/MojingRenderBase.cpp
		   Render/MojingRenderMultithread.cpp
		   Render/MojingRenderMultiThread_THREAD.cpp
		   Render/MultiThreadGLParam.cpp
		   Render/MojingRenderMultiThread_FB.cpp
            Reporter/ActiveTimeInfoReporter.cpp
    		Reporter/CrashReporter.cpp
    		Reporter/DatabaseInfoReporter.cpp
    		Reporter/EventInfoReporter.cpp
    		Reporter/GyroTempCalibrationReporter.cpp
    		Reporter/InfoReporter.cpp
    		Reporter/LogInfoReporter.cpp
    		Reporter/MobileConfigUpdate.cpp
    		Reporter/MojingMerchantVerify.cpp
    		Reporter/PageInfoReporter.cpp
    		Reporter/ReporterTools.cpp
    		Reporter/RunInfoReporter.cpp
    		Reporter/UserActionReporter.cpp
            Tracker/AndroidInternalSensorChecker.cpp
			Tracker/MojingAndroidSensor.cpp
			Tracker/MojingGyroTempCalibration.cpp
			Tracker/MojingSensor.cpp
			Tracker/MojingSensorCalibration.cpp
			Tracker/MojingSensorFilter.cpp
			Tracker/MojingSensorFusion.cpp
			Tracker/MojingTemperature.cpp
			Tracker/MojingTracker.cpp
			Tracker/MojingControllerSocket.cpp
            TextureBacker/FBO.cpp
            TextureBacker/Shader.cpp
            TextureBacker/TextBaker.cpp

            Render/MojingRenderMultiThread_3288.cpp
		   	3rdPart/3288VSync/C3288VSync.cpp
		   	3rdPart/3288Overlay/C3288Overlay.cpp
		   	3rdPart/3288Overlay/RenderOverlay3288.cpp
			3rdPart/Qualcomm/CSVRApi.cpp

            )

target_include_directories(mojing PRIVATE
                           ${CMAKE_SOURCE_DIR}/
                           ${CMAKE_SOURCE_DIR}/googlebreakpad/src
                           ${CMAKE_SOURCE_DIR}/3rdPart/ktx/include)

target_link_libraries(mojing
                      log
                      android
                      EGL
                      GLESv2
                      sqlite_lib
                      curl_lib
                      breakpad_client
                      z)

#make mojingvrrom.so

add_library(mojingvrrom SHARED
            Interface/Android/MojingVRRom.cpp
            Interface/Android/MojingVRRomAPI.cpp
            Render/Models/Model.cpp
            Render/Models/SkyboxModel.cpp
            Render/Models/SphereModel.cpp
            Render/Models/RectangleModel.cpp)

target_include_directories(mojingvrrom PRIVATE
                           ${CMAKE_SOURCE_DIR}/
                           ${CMAKE_SOURCE_DIR}/googlebreakpad/src
                           ${CMAKE_SOURCE_DIR}/Render/Model)

target_link_libraries(mojingvrrom
                      GLESv2
                      mojing)
