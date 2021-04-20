include_directories(./)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources 
    status.cpp
    test/status_test.cpp)

add_executable(status_test ${sources})
target_link_libraries(status_test pthread)
target_link_libraries(status_test gmock)
target_link_libraries(status_test gtest)

add_test(status_test ${PROJECT_BINARY_DIR}/bin/test/status_test)