cmake_minimum_required(VERSION 3.10.0)

set(target_name main)

aux_source_directory(${CMAKE_SOURCE_DIR}/src/tool TOOL_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_SRC)

set(COMMON_SRC ${SRC_SRC} ${TOOL_SRC})

add_executable(${target_name} main.c ${COMMON_SRC})
target_include_directories(${target_name} PRIVATE ${CMAKE_SOURCE_DIR}/inc)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output)