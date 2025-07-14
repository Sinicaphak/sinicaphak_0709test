cmake_minimum_required(VERSION 3.10.0)


include(GoogleTest)
set(targets)

# # 自动扫描test下的cpp测试文件, 将其文件名添加到这里
# file(GLOB_RECURSE test_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# foreach(src_file IN LISTS test_sources)
#     get_filename_component(target_name "${src_file}" NAME_WE)
#     list(APPEND targets ${target_name})
# endforeach()

# 手动添加测试文件
list(APPEND targets hello_test)

foreach(target IN LISTS targets)
    add_executable(${target} ${target}.cpp)
    # 为测试指定头文件
    target_include_directories(${target} PRIVATE
        ${CMAKE_SOURCE_DIR}/inc
        ${CMAKE_SOURCE_DIR}/inc/test
        ${CMAKE_SOURCE_DIR}/inc/test/doctest
    )
    # 链接项目库
    target_link_libraries(${target} PRIVATE static_lib GTest::gtest_main)
    add_test(NAME ${target} COMMAND ${target})

    gtest_discover_tests(${target})
endforeach()

