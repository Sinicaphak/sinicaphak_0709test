cmake_minimum_required(VERSION 3.10.0)
project(socket VERSION 0.1.0 LANGUAGES C CXX)

# 指定CC环境变量为gcc
if(NOT DEFINED ENV{CC})
    set(ENV{CC} gcc)
endif()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99") 
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -std=gnu99")

enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_SOURCE_DIR}/app)
if(
    (
        # 作为主项目构建时才编译测试
        CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME 
        # 手动指定编译测试
        OR MYPROJECT_BUILD_TESTING
    ) 
    # 是否启用测试
    AND BUILD_TESTING
)
    # google test
    # GoogleTest requires at least C++17
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    include(FetchContent)
    FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # 测试文件夹
    add_subdirectory(test)
    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)
    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()


