cmake_minimum_required(VERSION 2.8)

project(simple9)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug)

include_directories("${PROJECT_SOURCE_DIR}")

add_executable(simple9 simple9.c)


