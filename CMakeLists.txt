cmake_minimum_required(VERSION 3.2)

add_compile_options(-std=c++1y)
add_compile_options(-Wall -Wextra -Wno-missing-braces)
add_compile_options(-O0 -g)

set(EXECUTABLES
    sorting
    next_permutation
    johnson_trotter
    3sum
    ecdh
    cnf
    intersection
    range_search
    dijkstra
)

foreach(EXEC_NAME ${EXECUTABLES})
    add_executable(${EXEC_NAME} ${EXEC_NAME}.cpp)
    target_link_libraries(${EXEC_NAME} catch_main)
    add_test(${EXEC_NAME} ${EXEC_NAME})
endforeach()

# external
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/vendor)

# catch
add_subdirectory(${EXT_PROJECTS_DIR}/catch)
include_directories(${CATCH_INCLUDE_DIR})
enable_testing(true)

add_library(catch_main tools/catch_main.cpp)
add_dependencies(catch_main catch)

# test01
add_subdirectory(${EXT_PROJECTS_DIR}/testu01)
link_directories(${TESTU01_LIBRARY_DIR})

# xorshift executable
add_executable(xorshift xorshift.cpp)
add_dependencies(xorshift project_testu01)
target_include_directories(xorshift PUBLIC ${TESTU01_INCLUDE_DIR})
target_link_libraries(xorshift catch_main testu01)
add_test(xorshift xorshift)

add_subdirectory(distributed)
add_subdirectory(geometric)
