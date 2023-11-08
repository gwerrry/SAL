cmake_minimum_required(VERSION 3.9)

project(SAL VERSION 1.0 DESCRIPTION "Simple Audio Library")

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

if ( NOT CMAKE_BUILD_TYPE )
    message( "Build Type not set, defaulting to Debug..." )
    set( CMAKE_BUILD_TYPE Debug )
endif()

if (WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("Debug mode detected. Using MSVC MultiThreadedDebug...")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    else()
        message("Debug mode NOT detected. Using MSVC MultiThreaded...")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif()
endif()

# ------------------------------------------------------------------------------------------------
# Set the standard requirements
# ------------------------------------------------------------------------------------------------
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
        include/sal.h
        "tests/play sound/test.c"
)