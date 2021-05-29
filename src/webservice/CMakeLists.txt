include_directories(./)
include_directories(../../thirdpart/folly/include)
include_directories(../../thirdpart/libevent/include)
include_directories(../../thirdpart/glog/include)
include_directories(../../thirdpart/gmock/include)
include_directories(../../thirdpart/gflags/include)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(Router_sources 
    WebService.cpp
    StatusHandler.cpp
    NotFoundHandler.cpp
    test/RouterTest.cpp)

link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/gmock/lib)
link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/folly/lib)
link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/glog/lib)
link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/gflags/lib)

add_executable(Router_test ${Router_sources})
target_link_libraries(Router_test -pthread)
target_link_libraries(Router_test dl)
target_link_libraries(Router_test libfolly.a)
target_link_libraries(Router_test libglog.a)
target_link_libraries(Router_test libgmock.a)
target_link_libraries(Router_test libgtest.a)
target_link_libraries(Router_test libgflags.a)

add_test(Router_test ${PROJECT_BINARY_DIR}/bin/test/Router_test)