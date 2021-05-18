include_directories(./)
include_directories(../../thirdpart/gmock/include)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources 
    ProcessUtils.cpp
    ../status/status.cpp
    test/ProcessUtilsTest.cpp)

add_executable(ProcessUtilsTest ${sources})
target_link_libraries(ProcessUtilsTest pthread)
target_link_libraries(GeneralThreadPool_test ../../thirpart/gmock/lib/libgmock.a)
target_link_libraries(GeneralThreadPool_test ../../thirpart/gmock/lib/libtest.a)

add_test(ProcessUtilsTest ${PROJECT_BINARY_DIR}/bin/test/ProcessUtilsTest)