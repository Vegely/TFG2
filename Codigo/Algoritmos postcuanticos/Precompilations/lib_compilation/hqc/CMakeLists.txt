# CMakeLists.txt for HQC library

cmake_minimum_required(VERSION 3.10)
project(hqc_lib C)

set(CMAKE_C_STANDARD 99)

# Set default security level (HQC-256 only for now)
set(HQC_SECURITY_LEVEL 256 CACHE STRING "HQC security level (256 only)")

# Validate HQC_SECURITY_LEVEL
if(NOT (HQC_SECURITY_LEVEL EQUAL 256))
    message(FATAL_ERROR "HQC_SECURITY_LEVEL must be 256 (only HQC-256 is currently supported)")
endif()

# Source files for the HQC library
set(HQC_SOURCES
    # Core KEM functions
    kem.c
    hqc.c
    
    # Wrapper
    hqc_wrapper.c
    
    # Internal functions (based on your actual file listing)
    code.c
    fips202.c
    gf2x.c
    parsing.c
    shake_ds.c
    shake_prng.c
    vector.c
    fft.c
    gf.c
    reed_muller.c
    reed_solomon.c

    #RNG
    randombytes.c
)

# Common compile definitions for both builds
set(COMMON_COMPILE_DEFS HQC${HQC_SECURITY_LEVEL})

# Shared library
add_library(hqc_shared SHARED ${HQC_SOURCES})
set_target_properties(hqc_shared PROPERTIES OUTPUT_NAME hqc)
target_compile_definitions(hqc_shared PUBLIC ${COMMON_COMPILE_DEFS})
target_include_directories(hqc_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Static library
add_library(hqc_static STATIC ${HQC_SOURCES})
set_target_properties(hqc_static PROPERTIES OUTPUT_NAME hqc)
target_compile_definitions(hqc_static PUBLIC ${COMMON_COMPILE_DEFS})
target_include_directories(hqc_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Install targets
install(TARGETS hqc_shared
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS hqc_static
    ARCHIVE DESTINATION lib
)

install(FILES
    hqc_wrapper.h
    api.h
    DESTINATION include/hqc
)