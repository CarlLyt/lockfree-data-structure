cmake_minimum_required(VERSION 3.14)

add_executable(reorder_example reorder_example.cc)
add_executable(reorder_atomic_example reorder_atomic_example.cc)
add_executable(reorder_atomic_thread_fence_example reorder_atomic_thread_fence_example.cc)

message("architecture is " ${CMAKE_HOST_SYSTEM_PROCESSOR})
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64")
    message("arm64")
    target_link_libraries(reorder_example -lpthread)
    target_link_libraries(reorder_atomic_example -lpthread)
    target_link_libraries(reorder_atomic_thread_fence_example -lpthread)
else(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    message("x86")
  target_link_libraries(reorder_example -lrt -lpthread)
  target_link_libraries(reorder_atomic_example -lrt -lpthread)
  target_link_libraries(reorder_atomic_thread_fence_example -lpthread)
endif()


