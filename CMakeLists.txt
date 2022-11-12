cmake_minimum_required(VERSION 3.10)
project(lockfree-data-structures)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
option(BUILD_PRODUCE_CONSUMER "build produce-consuemer project" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -std=c++11")

add_subdirectory(src/)
add_subdirectory(example/)