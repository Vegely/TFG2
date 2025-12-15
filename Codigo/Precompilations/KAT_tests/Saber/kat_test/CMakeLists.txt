cmake_minimum_required(VERSION 3.10)
project(saber_kat_tests C)

set(CMAKE_C_STANDARD 99)

# Ensure we have the include directories from the main project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Allow configuration of the security level (L=2,3,4) via SABER_L
if(NOT DEFINED SABER_L)
    set(SABER_L 4 CACHE STRING "SABER parameter L (2=LightSaber, 3=Saber, 4=FireSaber)")
endif()

# Add executable for KAT using the wrapper
add_executable(SaberKAT kat_test_file.c)

# Link against saber.lib - path assumed relative, update as needed
target_link_libraries(SaberKAT ${CMAKE_CURRENT_SOURCE_DIR}/saber.lib)

# Use SABER_L value to define the security level in compilation
target_compile_definitions(SaberKAT PRIVATE SABER_L=${SABER_L})

# Include the directory with saber_wrapper.h if needed
target_include_directories(SaberKAT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)