include_directories(./)
include_directories(../../thirdpart/folly/include)
include_directories(../../thirdpart/libevent/include)
include_directories(../../thirdpart/glog/include)
include_directories(../../thirdpart/double-conversion/include)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test) 

set(sources 
    GeneralThreadPool.cpp
    GeneralWorker.cpp
    test/GeneralThreadPoolTest.cpp)

add_executable(GeneralThreadPool_test ${sources})
target_link_libraries(GeneralThreadPool_test pthread)
target_link_libraries(GeneralThreadPool_test gmock)
target_link_libraries(GeneralThreadPool_test gtest)
target_link_libraries(GeneralThreadPool_test ../../thirpart/libevent/lib/libevent.so)
target_link_libraries(GeneralThreadPool_test ../../thirpart/folly/lib/libfolly.a)
target_link_libraries(GeneralThreadPool_test ../../thirpart/glog/lib/libglog.a)
target_link_libraries(GeneralThreadPool_test ../../thirpart/double-conversion/lib/libdouble-conversion.a)

add_test(GeneralThreadPool_test ${PROJECT_BINARY_DIR}/bin/test/GeneralThreadPool_test)