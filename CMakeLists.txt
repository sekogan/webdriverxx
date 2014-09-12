cmake_minimum_required(VERSION 2.8)

project(webdriverxx C CXX)

enable_testing()

include_directories(include/webdriverxx)
add_subdirectory(test)
