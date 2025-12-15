cmake_minimum_required(VERSION 3.10)
project(hqc_kat_tests C)

set(CMAKE_C_STANDARD 99)

# Ensure we have the include directories from the main project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Allow configuration of the security level (128, 192, 256) via HQC_LEVEL
if(NOT DEFINED HQC_LEVEL)
    set(HQC_LEVEL 256 CACHE STRING "HQC security level (128=HQC-128, 192=HQC-192, 256=HQC-256)")
endif()

# Add executable for KAT using the wrapper
add_executable(HqcKAT kat_test_file.c)

# Link against hqc.lib - path assumed relative, update as needed
target_link_libraries(HqcKAT ${CMAKE_CURRENT_SOURCE_DIR}/hqc.lib)

# Use HQC_LEVEL value to define the security level in compilation
target_compile_definitions(HqcKAT PRIVATE HQC_LEVEL=${HQC_LEVEL})

# Include the directory with hqc_wrapper.h if needed
target_include_directories(HqcKAT PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)