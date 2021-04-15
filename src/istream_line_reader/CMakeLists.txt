include_directories(./)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test) 

set(sources
   test/main.cpp
   test/istream_line_reader_test.cpp)
   
add_executable(test ${sources})
target_link_libraries(test pthread)
target_link_libraries(test gmock)
target_link_libraries(test gtest)