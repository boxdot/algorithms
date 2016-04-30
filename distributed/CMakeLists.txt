find_package(Threads)
add_library(distsys distsys.cpp astream.cpp)

add_executable(snapshot snapshot.cpp)
target_link_libraries(snapshot catch_main distsys ${CMAKE_THREAD_LIBS_INIT})
add_test(distributed/snapshot snapshot)
