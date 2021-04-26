include_directories(./)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources
   FileUtils.cpp
   test/FileUtilsTest.cpp)
   
add_executable(FileUtilsTest ${sources})
target_link_libraries(FileUtilsTest pthread)
target_link_libraries(FileUtilsTest gmock)
target_link_libraries(FileUtilsTest gtest)

add_test(FileUtilsTest ${PROJECT_BINARY_DIR}/bin/test/FileUtilsTest)