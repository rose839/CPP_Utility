cmake_minimum_required(VERSION 2.8)
project(cpp_utils_test)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)

if(COMPILER_SUPPORTS_CXX14)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

enable_testing()

include_directories(src)
add_subdirectory(src/istream_line_reader)
add_subdirectory(src/status)
add_subdirectory(src/process)
add_subdirectory(src/fs)
add_subdirectory(src/thread)