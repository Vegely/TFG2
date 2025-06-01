# CMakeLists.txt for SABER library

cmake_minimum_required(VERSION 3.10)
project(saber_lib C)

set(CMAKE_C_STANDARD 99)

# Set default security level (can be overridden via -DSABER_L=3)
set(SABER_L 4 CACHE STRING "SABER parameter L (2=LightSaber, 3=Saber, 4=FireSaber)")

# Validate SABER_L
if(NOT (SABER_L EQUAL 2 OR SABER_L EQUAL 3 OR SABER_L EQUAL 4))
    message(FATAL_ERROR "SABER_L must be 2, 3, or 4")
endif()

# Source files for the SABER library
set(SABER_SOURCES
    # Core KEM functions
    kem.c
    
    # Wrapper
    saber_wrapper.c
    
    # Internal functions (typical SABER implementation files)
    SABER_indcpa.c
    poly.c
    poly_mul.c
    pack_unpack.c
    verify.c
    cbd.c
    
    # Symmetric primitives and hashing
    fips202.c
    
    # RNG
    rng.c
)

# Common compile definitions for both builds
set(COMMON_COMPILE_DEFS SABER_L=${SABER_L})

# Set algorithm name based on SABER_L
if(SABER_L EQUAL 2)
    set(ALGO_NAME "LightSaber")
elseif(SABER_L EQUAL 3)
    set(ALGO_NAME "Saber")
elseif(SABER_L EQUAL 4)
    set(ALGO_NAME "FireSaber")
endif()

message(STATUS "Building ${ALGO_NAME} (SABER_L=${SABER_L})")

# Shared library
add_library(saber_shared SHARED ${SABER_SOURCES})
set_target_properties(saber_shared PROPERTIES OUTPUT_NAME saber)
target_compile_definitions(saber_shared PUBLIC ${COMMON_COMPILE_DEFS})
target_include_directories(saber_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Static library
add_library(saber_static STATIC ${SABER_SOURCES})
set_target_properties(saber_static PROPERTIES OUTPUT_NAME saber)
target_compile_definitions(saber_static PUBLIC ${COMMON_COMPILE_DEFS})
target_include_directories(saber_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Compiler flags for optimization
if(CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(saber_shared PRIVATE -O3 -Wall -Wextra)
    target_compile_options(saber_static PRIVATE -O3 -Wall -Wextra)
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    target_compile_options(saber_shared PRIVATE /O2 /W3)
    target_compile_options(saber_static PRIVATE /O2 /W3)
endif()

# Install targets
install(TARGETS saber_shared
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS saber_static
    ARCHIVE DESTINATION lib
)

install(FILES
    saber_wrapper.h
    SABER_params.h
    api.h
    DESTINATION include/saber
)

# Optional: Create a simple test executable
option(SABER_BUILD_TESTS "Build SABER test executable" OFF)
if(SABER_BUILD_TESTS)
    add_executable(saber_test test_saber.c)
    target_link_libraries(saber_test saber_static)
    target_compile_definitions(saber_test PRIVATE ${COMMON_COMPILE_DEFS})
endif()