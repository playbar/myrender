cmake_minimum_required(VERSION 3.4.1)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fPIC  -shared")

add_library(svrapi SHARED
            svrApi/private/svrApiVersion.cpp
            svrApi/private/svrApiCore.cpp
            svrApi/private/svrApiTimeWarp.cpp
            svrApi/private/svrApiTimeWarpShaders.cpp
            svrApi/private/svrApiSensor.cpp
            svrApi/private/svrApiHelper.cpp
            svrApi/private/svrApiHiddenMesh.cpp
            )

# Include libraries needed for hello-jnicallback lib
target_link_libraries(svrapi log android EGL GLEsv3 )
