add_executable(main main.c)
target_link_libraries(main PRIVATE static_lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output)
