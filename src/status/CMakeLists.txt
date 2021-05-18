include_directories(./)
include_directories(../../thirdpart/gmock/include)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources 
    status.cpp
    test/status_test.cpp)

link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/gmock/lib)

add_executable(status_test ${sources})
target_link_libraries(status_test pthread)
target_link_libraries(status_test libgmock.a)
target_link_libraries(status_test libgtest.a)

add_test(status_test ${PROJECT_BINARY_DIR}/bin/test/status_test)
