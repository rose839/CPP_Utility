include_directories(./)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources 
    ProcessUtils.cpp
    ../status/status.cpp
    test/ProcessUtilsTest.cpp)

add_executable(ProcessUtilsTest ${sources})
target_link_libraries(ProcessUtilsTest pthread)
target_link_libraries(ProcessUtilsTest gmock)
target_link_libraries(ProcessUtilsTest gtest)

add_test(ProcessUtilsTest ${PROJECT_BINARY_DIR}/bin/test/ProcessUtilsTest)