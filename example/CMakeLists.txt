cmake_minimum_required(VERSION 3.14)

add_executable(reorder_example reorder_example.cc)
add_executable(reorder_atomic_seq_cst reorder_atomic_seq_cst.cc)
add_executable(reorder_atomic_thread_fence_example reorder_atomic_thread_fence_example.cc)
add_executable(reorder_atomic_acq_rel reorder_atomic_acq_rel.cc)
add_executable(reorder_atomic_acq_cum reorder_atomic_acq_cum.cc)

message("architecture is " ${CMAKE_HOST_SYSTEM_PROCESSOR})
if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64")
    message("Program Run On arm64 Arch")
    target_link_libraries(reorder_example -lpthread)
    target_link_libraries(reorder_atomic_seq_cst -lpthread)
    target_link_libraries(reorder_atomic_thread_fence_example -lpthread)
    target_link_libraries(reorder_atomic_acq_rel -lpthread)
    target_link_libraries(reorder_atomic_acq_cum -lpthread)

else(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    message("Program Run On x86 Arch")
  target_link_libraries(reorder_example -lrt -lpthread)
  target_link_libraries(reorder_atomic_seq_cst -lrt -lpthread)
  target_link_libraries(reorder_atomic_thread_fence_example  -lrt -lpthread)
  target_link_libraries(reorder_atomic_acq_rel  -lrt -lpthread)
  target_link_libraries(reorder_atomic_acq_cum -lpthread)

endif()


