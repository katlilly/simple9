cmake_minimum_required(VERSION 2.8)

project(simple9)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories("${PROJECT_SOURCE_DIR}")

add_executable(simple9 
    ${PROJECT_SOURCE_DIR}/simple9.c
    ${PROJECT_SOURCE_DIR}/mylib.c
)

 
