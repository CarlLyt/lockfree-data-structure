cmake_minimum_required(VERSION 3.10)
project(lockfree-data-structures)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
option(BUILD_PRODUCE_CONSUMER "build produce-consuemer project" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -std=c++17 --save-temps")

add_subdirectory(src/)
add_subdirectory(example/)