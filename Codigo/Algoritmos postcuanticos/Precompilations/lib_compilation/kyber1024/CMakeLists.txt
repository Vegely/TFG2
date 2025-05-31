# CMakeLists.txt for Kyber library

cmake_minimum_required(VERSION 3.10)
project(kyber_lib C)

set(CMAKE_C_STANDARD 99)

# Set default security level (can be overridden via -DKYBER_K=3)
set(KYBER_K 4 CACHE STRING "Kyber parameter K (2=Kyber512, 3=Kyber768, 4=Kyber1024)")

# Validate KYBER_K
if(NOT (KYBER_K EQUAL 2 OR KYBER_K EQUAL 3 OR KYBER_K EQUAL 4))
    message(FATAL_ERROR "KYBER_K must be 2, 3, or 4")
endif()

# Source files for the Kyber library
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
    randombytes.c
)

# Common compile definitions for both builds
set(COMMON_COMPILE_DEFS KYBER_K=${KYBER_K})

# Shared library
add_library(kyber_shared SHARED ${KYBER_SOURCES})
set_target_properties(kyber_shared PROPERTIES OUTPUT_NAME kyber)
target_compile_definitions(kyber_shared PUBLIC ${COMMON_COMPILE_DEFS})
target_include_directories(kyber_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Static library
add_library(kyber_static STATIC ${KYBER_SOURCES})
set_target_properties(kyber_static PROPERTIES OUTPUT_NAME kyber)
target_compile_definitions(kyber_static PUBLIC ${COMMON_COMPILE_DEFS})
target_include_directories(kyber_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Install targets
install(TARGETS kyber_shared
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS kyber_static
    ARCHIVE DESTINATION lib
)

install(FILES
    kyber_wrapper.h
    DESTINATION include/kyber
)