cmake_minimum_required(VERSION 3.10.0)

# 将头文件与源文件加入到静态库
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/inc/*.h")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")
# Make an automatic library - will be static or dynamic based on user setting
add_library(static_lib ${SOURCE_LIST} ${HEADER_LIST})
# We need this directory, and users of our library will need it too
target_include_directories(static_lib PUBLIC ${CMAKE_SOURCE_DIR}/inc)

# IDEs should put the headers in a nice place
source_group(
  TREE "${CMAKE_SOURCE_DIR}/inc"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
