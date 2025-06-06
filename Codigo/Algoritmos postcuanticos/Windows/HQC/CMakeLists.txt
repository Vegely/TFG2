cmake_minimum_required(VERSION 3.14)
project(HQCTest C)

set(CMAKE_C_STANDARD 99)

# Executable target
add_executable(hqc_test main.c)

# Add include directory if necessary
target_include_directories(hqc_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link hqc.lib
target_link_libraries(hqc_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/hqc.lib")
