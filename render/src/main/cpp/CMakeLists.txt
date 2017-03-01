cmake_minimum_required(VERSION 3.4.1)

#add_library(app_glue STATIC
#            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DUSE_QVR_SERVICE -fexceptions")

add_library(symphony_lib STATIC IMPORTED)
set_target_properties(symphony_lib PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/3rdparty/symphony/lib/libsymphony-cpu.a)

add_library(symphonypower_lib STATIC IMPORTED)
set_target_properties(symphonypower_lib PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/3rdparty/symphony/lib/libsymphonypower-1.1.1.a)

add_library(libqvrservice_lib SHARED IMPORTED)
set_target_properties(libqvrservice_lib  PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/3rdparty/qvr/lib/libqvrservice_client.so)

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


###################################


add_library(apitest SHARED
            test/mtptest.cpp
            android_native_app_glue.c
            framework/svrApplication.cpp
            framework/svrAndroidMain.cpp
            framework/svrInput.cpp
            3rdparty/imgui/imgui.cpp
            )

target_include_directories(apitest PRIVATE
                           ${CMAKE_SOURCE_DIR}/apitest
                           ${CMAKE_SOURCE_DIR}
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
                           ${CMAKE_SOURCE_DIR}/3rdparty/imgui
                           )

# Include libraries needed for hello-jnicallback lib
target_link_libraries(apitest
                        log
                        android
                        EGL
                        GLESv3
                        svrapi)


##############################
add_library(nt SHARED
            main.cpp
             android_native_app_glue.c)

target_include_directories(nt PRIVATE
                            ${CMAKE_SOURCE_DIR})

# add lib dependencies
target_link_libraries(nt
                      android
                      EGL
                      GLESv1_CM
                      log)

