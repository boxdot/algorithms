cmake_minimum_required(VERSION 2.8.8)
include(ExternalProject)
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

ExternalProject_Add(
    project_testu01
    PREFIX ${CMAKE_BINARY_DIR}/testu01
    URL http://www.iro.umontreal.ca/~simardr/testu01/TestU01.zip
    URL_MD5 ff9bd5106178c1ca0665691db8e57293
    CONFIGURE_COMMAND CC=cc CFLAGS=-Wno-return-type ./configure --prefix=${CMAKE_BINARY_DIR}/testu01/dist
    BUILD_COMMAND make -j
    BUILD_IN_SOURCE 1
    TIMEOUT 10
    LOG_DOWNLOAD 1
)

# Expose required variable to parent scope
set(TESTU01_INCLUDE_DIR ${CMAKE_BINARY_DIR}/testu01/dist/include CACHE INTERNAL
    "Path to include folder for TestU01")
set(TESTU01_LIBRARY_DIR ${CMAKE_BINARY_DIR}/testu01/dist/lib CACHE INTERNAL
    "Path to library folder for TestU01")
