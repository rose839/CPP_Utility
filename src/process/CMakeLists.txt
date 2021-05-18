include_directories(./)
include_directories(../../thirdpart/gmock/include)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources 
    ProcessUtils.cpp
    ../status/status.cpp
    test/ProcessUtilsTest.cpp)

link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/gmock/lib)

add_executable(ProcessUtilsTest ${sources})
target_link_libraries(ProcessUtilsTest pthread)
target_link_libraries(ProcessUtilsTest libgmock.a)
target_link_libraries(ProcessUtilsTest libgtest.a)

add_test(ProcessUtilsTest ${PROJECT_BINARY_DIR}/bin/test/ProcessUtilsTest)
