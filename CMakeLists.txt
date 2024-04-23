cmake_minimum_required(VERSION 3.9)

project(SAL VERSION 3.1.1 DESCRIPTION "Simple Audio Library")

add_compile_definitions(SAL_VERSION="${PROJECT_VERSION}")

set(CMAKE_C_STANDARD 99)

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

find_package(OpenAL CONFIG REQUIRED) # comment out if you dont want to try with openal

add_executable(${PROJECT_NAME}
        sal.h
        "test.c"
        "test2.c"
)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenAL::OpenAL) # comment out if you dont want to try with openal
