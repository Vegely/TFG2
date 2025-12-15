# CMakeLists.txt for KAT tests

cmake_minimum_required(VERSION 3.10)
project(kyber_kat_tests C)

set(CMAKE_C_STANDARD 99)

# Ensure we have the include directories from the main project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Allow configuration of the security level (K=2,3,4) via KYBER_K
if(NOT DEFINED KYBER_K)
    set(KYBER_K 2 CACHE STRING "Kyber parameter K (2=Kyber512, 3=Kyber768, 4=Kyber1024)")
endif()

# Add executable for KAT using the wrapper
add_executable(KyberKAT kat_test_file.c)

# Link against kyber.lib - path assumed relative, update as needed
target_link_libraries(KyberKAT ${CMAKE_CURRENT_SOURCE_DIR}/kyber.lib)

# Use KYBER_K value to define the security level in compilation
target_compile_definitions(KyberKAT PRIVATE KYBER_K=${KYBER_K})

# Include the directory with kyber_wrapper.h if needed
target_include_directories(KyberKAT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
