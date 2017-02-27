cmake_minimum_required(VERSION 3.4.1)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                    -std=c++11 \
                    -Wall \
                    -O0 -g \
                    -fexceptions \
                    -DUSE_QVR_SERVICE \
                    -DNDEBUG \
                    -DMJ_OS_ANDROID")

MESSAGE(STATUS "Project: svrapi")

add_library(symphony_lib STATIC IMPORTED)
set_target_properties(symphony_lib PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/3rdparty/symphony/lib/libsymphony-cpu.a)

add_library(symphonypower_lib STATIC IMPORTED)
set_target_properties(symphonypower_lib PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/3rdparty/symphony/lib/libsymphonypower-1.1.1.a)

add_library(libqvrservice_lib SHARED IMPORTED)
set_target_properties(libqvrservice_lib  PROPERTIES IMPORTED_LOCATION
     ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libqvrservice_client.so)

#SET(USE_QVR_SERVICE true)


add_library(svrapi SHARED
            svrApi/private/svrApiVersion.cpp
            svrApi/private/svrApiCore.cpp
            svrApi/private/svrApiTimeWarp.cpp
            svrApi/private/svrApiTimeWarpShaders.cpp
            svrApi/private/svrApiSensor.cpp
            svrApi/private/svrApiHelper.cpp
            svrApi/private/svrApiHiddenMesh.cpp
            svrApi/private/svrApiDebugServer.cpp
            svrApi/private/svrApiPredictiveSensor.cpp
            framework/svrConfig.cpp
            framework/svrContainers.cpp
            framework/svrGeometry.cpp
            framework/svrShader.cpp
            framework/svrUtil.cpp
            framework/svrRenderTarget.cpp
            framework/svrKtxLoader.cpp
             framework/svrProfile.cpp
             framework/svrCpuTimer.cpp
             framework/svrGpuTimer.cpp
             framework/svrRenderExt.cpp
             3rdparty/tinyobj/tiny_obj_loader.cc
             3rdparty/cJSON/cJSON.c
            )

target_include_directories(svrapi PRIVATE
                           ${CMAKE_SOURCE_DIR}/
                           ${CMAKE_SOURCE_DIR}/3rdparty/glm-0.9.7.0
                           ${CMAKE_SOURCE_DIR}/framework
                           ${CMAKE_SOURCE_DIR}/3rdparty/tinyobj
                           ${CMAKE_SOURCE_DIR}/3rdparty/cJSON
                           ${CMAKE_SOURCE_DIR}/svrApi/public
                           ${CMAKE_SOURCE_DIR}/3rdparty/telemetry/include
                           ${CMAKE_SOURCE_DIR}/3rdparty/symphony/include
                           ${CMAKE_SOURCE_DIR}/3rdparty/qvr
                           ${CMAKE_SOURCE_DIR}/svrApi
                           ${CMAKE_SOURCE_DIR}/3rdparty/qvr/inc
                           )

# Include libraries needed for hello-jnicallback lib
target_link_libraries(svrapi
                        log
                        android
                        EGL
                        GLESv3
                        libqvrservice_lib
                        symphony_lib
                        symphonypower_lib)

#add_library(svrapi_lib SHARED IMPORTED)
#set_target_properties(svrapi_lib  PROPERTIES IMPORTED_LOCATION
#    ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libsvrapi.so)

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
            Interface/Android/MojingAndroidAPI.cpp
			Interface/Android/MojingNativeInput.cpp
			Interface/Unreal/MojingUnrealAPI.cpp
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
			Platform/MojingPlatformAndroid.cpp Platform/MojingPlatformBase.cpp
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
			3rdPart/Qualcomm/CSVRApiSrc.cpp

            )

target_include_directories(mojing PRIVATE
                           ${CMAKE_SOURCE_DIR}/
                           ${CMAKE_SOURCE_DIR}/googlebreakpad/src
                           ${CMAKE_SOURCE_DIR}/3rdPart/ktx/include)

target_link_libraries(mojing
                      log
                      android
                      EGL
                      GLESv3
                      sqlite_lib
                      curl_lib
                      breakpad_client
                      svrapi
                      z)
