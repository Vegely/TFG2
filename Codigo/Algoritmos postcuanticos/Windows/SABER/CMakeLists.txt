cmake_minimum_required(VERSION 3.14)
project(SaberTest C)

set(CMAKE_C_STANDARD 99)

# Executable target
add_executable(saber_test main.c)

# Add include directory if necessary
target_include_directories(saber_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link saber.lib
target_link_libraries(saber_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/saber.lib")