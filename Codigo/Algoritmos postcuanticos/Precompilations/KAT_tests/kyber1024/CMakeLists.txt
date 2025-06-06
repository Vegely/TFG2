# CMakeLists.txt for Kyber1024 library

cmake_minimum_required(VERSION 3.10)
project(kyber1024_lib C)

set(CMAKE_C_STANDARD 99)

# Set Kyber1024 security level (KYBER_K=4)
set(KYBER_K 4 CACHE STRING "Kyber parameter K (4=Kyber1024)")

# Validate KYBER_K for Kyber1024
if(NOT KYBER_K EQUAL 4)
    message(FATAL_ERROR "This build is configured for Kyber1024 (KYBER_K=4)")
endif()

# Source files for the Kyber1024 library
set(KYBER_SOURCES
    # Core KEM functions
    kem.c
    
    # Wrapper
    kyber_wrapper.c
    
    # Internal functions (add all required source files here)
    indcpa.c
    polyvec.c
    poly.c
    ntt.c
    cbd.c
    reduce.c
    verify.c
    
    # Symmetric primitives
    symmetric-shake.c
    fips202.c
    symmetric-aes.c
    sha512.c
    sha256.c
    aes256ctr.c

    # RNG
    rng.c
)

# Common compile definitions for both builds
set(COMMON_COMPILE_DEFS KYBER_K=${KYBER_K})

# Shared library
add_library(kyber1024_shared SHARED ${KYBER_SOURCES})
set_target_properties(kyber1024_shared PROPERTIES OUTPUT_NAME kyber1024)
target_compile_definitions(kyber1024_shared PUBLIC ${COMMON_COMPILE_DEFS})
target_include_directories(kyber1024_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Static library
add_library(kyber1024_static STATIC ${KYBER_SOURCES})
set_target_properties(kyber1024_static PROPERTIES OUTPUT_NAME kyber1024)
target_compile_definitions(kyber1024_static PUBLIC ${COMMON_COMPILE_DEFS})
target_include_directories(kyber1024_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Install targets
install(TARGETS kyber1024_shared
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS kyber1024_static
    ARCHIVE DESTINATION lib
)

install(FILES
    kyber_wrapper.h
    DESTINATION include/kyber1024
)