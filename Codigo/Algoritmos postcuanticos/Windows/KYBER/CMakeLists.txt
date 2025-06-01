cmake_minimum_required(VERSION 3.14)
project(KyberTest C)

set(CMAKE_C_STANDARD 99)

# Executable target
add_executable(kyber_test main.c)

# Add include directory if necessary
target_include_directories(kyber_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link kyber.lib
target_link_libraries(kyber_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/kyber.lib")
