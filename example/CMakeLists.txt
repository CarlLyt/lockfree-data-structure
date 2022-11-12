cmake_minimum_required(VERSION 3.14)

add_executable(reorder_example reorder_example.cc)

target_link_libraries(reorder_example -lrt -lpthread)