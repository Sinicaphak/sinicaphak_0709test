cmake_minimum_required(VERSION 3.10.0)
project(socket VERSION 0.1.0 LANGUAGES C)

# 指定CC环境变量为gcc
if(NOT DEFINED ENV{CC})
    set(ENV{CC} gcc)
endif()


set(CMAKE_BUILD_TYPE Debug)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99") 
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -std=gnu99")

enable_testing()
