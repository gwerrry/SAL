cmake_minimum_required(VERSION 3.9)

project(SAL VERSION 1.0 DESCRIPTION "Simple Audio Library")
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

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
