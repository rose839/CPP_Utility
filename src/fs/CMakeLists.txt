include_directories(./)
include_directories(../../thirdpart/gmock/include)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources
   FileUtils.cpp
   test/FileUtilsTest.cpp)
   
add_executable(FileUtilsTest ${sources})
target_link_libraries(FileUtilsTest pthread)
target_link_libraries(GeneralThreadPool_test ../../thirpart/gmock/lib/libgmock.a)
target_link_libraries(GeneralThreadPool_test ../../thirpart/gmock/lib/libtest.a)

add_test(FileUtilsTest ${PROJECT_BINARY_DIR}/bin/test/FileUtilsTest)