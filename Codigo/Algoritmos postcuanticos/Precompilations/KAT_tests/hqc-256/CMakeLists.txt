# CMakeLists.txt for HQC-256 library

cmake_minimum_required(VERSION 3.10)
project(hqc_lib C)

set(CMAKE_C_STANDARD 99)

# Set HQC variant (fixed to HQC-256)
set(HQC_VARIANT "HQC-256")
add_compile_definitions(HQC256)

message(STATUS "Building ${HQC_VARIANT}")

# Source files for the HQC-256 library
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
    
    # NIST RNG
    rng.c
)

# Common compile definitions
set(COMMON_COMPILE_DEFS HQC256 NIST_RAND)

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

# Link OpenSSL if needed
find_package(OpenSSL)
if(OpenSSL_FOUND)
    target_link_libraries(hqc_shared OpenSSL::Crypto)
    target_link_libraries(hqc_static OpenSSL::Crypto)
    message(STATUS "OpenSSL found and linked")
else()
    message(WARNING "OpenSSL not found - ensure all crypto dependencies are available")
endif()

# Compiler flags for optimization
if(CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(hqc_shared PRIVATE -O3 -Wall -Wextra)
    target_compile_options(hqc_static PRIVATE -O3 -Wall -Wextra)
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    # Using your specified MSVC flags
    target_compile_options(hqc_shared PRIVATE /nologo /O2)
    target_compile_options(hqc_static PRIVATE /nologo /O2)
    # Add defines for MSVC compatibility
    target_compile_definitions(hqc_shared PRIVATE _CRT_SECURE_NO_WARNINGS MSVC_COMPILER)
    target_compile_definitions(hqc_static PRIVATE _CRT_SECURE_NO_WARNINGS MSVC_COMPILER)
endif()

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
    parameters.h
    hqc.h
    DESTINATION include/hqc
)

# Optional: Create a simple test executable
option(HQC_BUILD_TESTS "Build HQC test executable" OFF)
if(HQC_BUILD_TESTS)
    add_executable(hqc_test test_hqc.c)
    target_link_libraries(hqc_test hqc_static)
    target_compile_definitions(hqc_test PRIVATE ${COMMON_COMPILE_DEFS})
endif()