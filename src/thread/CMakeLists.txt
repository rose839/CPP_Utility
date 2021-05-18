include_directories(./)
include_directories(../../thirdpart/folly/include)
include_directories(../../thirdpart/libevent/include)
include_directories(../../thirdpart/glog/include)
include_directories(../../thirdpart/double-conversion/include)
include_directories(../../thirdpart/boost/include)
include_directories(../../thirdpart/gmock/include)
include_directories(../../thirdpart/gflags/include)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources 
    GeneralThreadPool.cpp
    GeneralWorker.cpp
    test/GeneralThreadPoolTest.cpp)

link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/gmock/lib)
link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/folly/lib)
link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/glog/lib)
link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/double-conversion/lib)
link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/libevent/lib)
link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/gflags/lib)
link_directories(${cpp_utils_test_SOURCE_DIR}/thirdpart/boost/lib)

add_executable(GeneralThreadPool_test ${sources})
target_link_libraries(GeneralThreadPool_test -pthread)
target_link_libraries(GeneralThreadPool_test dl)
target_link_libraries(GeneralThreadPool_test libevent.a)
target_link_libraries(GeneralThreadPool_test libfolly.a)
target_link_libraries(GeneralThreadPool_test libglog.a)
target_link_libraries(GeneralThreadPool_test libdouble-conversion.a)
target_link_libraries(GeneralThreadPool_test libgmock.a)
target_link_libraries(GeneralThreadPool_test libgtest.a)
target_link_libraries(GeneralThreadPool_test libgflags.a)
target_link_libraries(GeneralThreadPool_test libboost_context.a)

add_test(GeneralThreadPool_test ${PROJECT_BINARY_DIR}/bin/test/GeneralThreadPool_test)
