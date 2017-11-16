cmake_minimum_required(VERSION 3.4.1)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                    -std=c++11 \
                    -Wall \
                    -O0 -g \
                    -fexceptions \
                    -DMJ_OS_ANDROID")

add_library(assimp_lib SHARED IMPORTED )
set_target_properties(assimp_lib PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libassimp.so)

add_library(curl_lib SHARED IMPORTED)
set_target_properties(curl_lib  PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libcurl.so)


#make breakpad_client.a
add_library(breakpad_client STATIC IMPORTED)
set_target_properties(breakpad_client PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libbreakpad_client.a)

#model3d
add_library(model3d SHARED
            model/assimpActivity.cpp
            model/model3d.cpp
            model/TextureUtil.cpp
            model/common/assimpLoader.cpp
            model/common/misc.cpp
            model/common/myGLFunctions.cpp
            model/common/myJNIHelper.cpp
            model/common/myShader.cpp
            model/modelAssimp/modelAssimp
            model/gl3stub.c
            )

target_include_directories(model3d PRIVATE
                           ${CMAKE_SOURCE_DIR}/model
                           ${CMAKE_SOURCE_DIR}/model/common
                           ${CMAKE_SOURCE_DIR}/assimp/include
                           ${CMAKE_SOURCE_DIR}/glm
                           )

target_link_libraries(model3d
                      log
                      android
                      EGL
                      GLESv3
                      GLESv2
                      assimp_lib
                      )

#make viewcore.so

add_library(viewcore SHARED
            BitmapOpJni.cpp
            GenTexTask.cpp
            GLThreadUtilJni.cpp
            ThreadPool.cpp
            viewcore.cpp
            )

target_include_directories(viewcore PRIVATE
                           ${CMAKE_SOURCE_DIR}/
                           ${CMAKE_SOURCE_DIR}/googlebreakpad/src
                           ${CMAKE_SOURCE_DIR}/3rdPart/ktx/include)

target_link_libraries(viewcore
                      log
                      android
                      EGL
                      GLESv3
                      model3d)




