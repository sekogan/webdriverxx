cmake_minimum_required(VERSION 2.8)
project(webdriverxx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

enable_testing()

include_directories(include)
add_subdirectory(test)
