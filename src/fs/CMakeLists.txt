include_directories(./)
include_directories(../../thirdpart/gmock/include)


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources
   FileUtils.cpp
   test/FileUtilsTest.cpp)
   
link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/gmock/lib)

add_executable(FileUtilsTest ${sources})
target_link_libraries(FileUtilsTest pthread)
target_link_libraries(FileUtilsTest libgmock.a)
target_link_libraries(FileUtilsTest libgtest.a)

add_test(FileUtilsTest ${PROJECT_BINARY_DIR}/bin/test/FileUtilsTest)
