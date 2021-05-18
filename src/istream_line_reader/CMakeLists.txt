include_directories(./)
include_directories(../../thirdpart/gmock/include)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources
   test/main.cpp
   test/istream_line_reader_test.cpp)
   
add_executable(istream_line_reader_test ${sources})
target_link_libraries(istream_line_reader_test pthread)
target_link_libraries(GeneralThreadPool_test ../../thirpart/gmock/lib/libgmock.a)
target_link_libraries(GeneralThreadPool_test ../../thirpart/gmock/lib/libtest.a)

add_test(istream_line_reader_test ${PROJECT_BINARY_DIR}/bin/test/istream_line_reader_test)