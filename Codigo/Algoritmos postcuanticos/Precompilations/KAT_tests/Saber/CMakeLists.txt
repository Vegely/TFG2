# CMakeLists.txt for BIKE library

cmake_minimum_required(VERSION 3.10)
project(bike_lib C)

set(CMAKE_C_STANDARD 99)

# Set default security level (can be overridden via -DBIKE_L=3)
set(BIKE_L 5 CACHE STRING "BIKE security level (1=Level1, 3=Level3, 5=Level5)")

# Validate BIKE_L
if(NOT (BIKE_L EQUAL 1 OR BIKE_L EQUAL 3 OR BIKE_L EQUAL 5))
    message(FATAL_ERROR "BIKE_L must be 1, 3, or 5")
endif()

# Source files for the BIKE library
set(BIKE_SOURCES
    # Core KEM functions
    kem.c
    
    # Wrapper
    bike_wrapper.c
    
    # Internal functions (typical BIKE implementation files)
    decode.c
    sampling.c
    ntl.c
    conversions.c
    utilities.c
    hash_wrapper.c
    shake_prng.c
    ring_buffer.c
    
    # NIST RNG
    rng.c
)

# Common compile definitions for both builds
set(COMMON_COMPILE_DEFS BIKE_L=${BIKE_L} NIST_RAND)

# Set algorithm name based on BIKE_L
if(BIKE_L EQUAL 1)
    set(ALGO_NAME "BIKE-L1")
elseif(BIKE_L EQUAL 3)
    set(ALGO_NAME "BIKE-L3")
elseif(BIKE_L EQUAL 5)
    set(ALGO_NAME "BIKE-L5")
endif()

message(STATUS "Building ${ALGO_NAME} (BIKE_L=${BIKE_L})")

# Shared library
add_library(bike_shared SHARED ${BIKE_SOURCES})
set_target_properties(bike_shared PROPERTIES OUTPUT_NAME bike)
target_compile_definitions(bike_shared PUBLIC ${COMMON_COMPILE_DEFS})
target_include_directories(bike_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Static library
add_library(bike_static STATIC ${BIKE_SOURCES})
set_target_properties(bike_static PROPERTIES OUTPUT_NAME bike)
target_compile_definitions(bike_static PUBLIC ${COMMON_COMPILE_DEFS})
target_include_directories(bike_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link OpenSSL if needed
find_package(OpenSSL)
if(OpenSSL_FOUND)
    target_link_libraries(bike_shared OpenSSL::Crypto)
    target_link_libraries(bike_static OpenSSL::Crypto)
    message(STATUS "OpenSSL found and linked")
else()
    message(WARNING "OpenSSL not found - ensure all crypto dependencies are available")
endif()

# Compiler flags for optimization
if(CMAKE_C_COMPILER_ID MATCHES "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(bike_shared PRIVATE -O3 -Wall -Wextra)
    target_compile_options(bike_static PRIVATE -O3 -Wall -Wextra)
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
    target_compile_options(bike_shared PRIVATE /O2 /W3 /wd4244 /std:c11)
    target_compile_options(bike_static PRIVATE /O2 /W3 /wd4244 /std:c11)
    # Add defines for MSVC compatibility
    target_compile_definitions(bike_shared PRIVATE _CRT_SECURE_NO_WARNINGS MSVC_COMPILER)
    target_compile_definitions(bike_static PRIVATE _CRT_SECURE_NO_WARNINGS MSVC_COMPILER)
endif()

# Install targets
install(TARGETS bike_shared
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS bike_static
    ARCHIVE DESTINATION lib
)

install(FILES
    bike_wrapper.h
    api.h
    types.h
    defs.h
    utilities.h
    DESTINATION include/bike
)

# Optional: Create a simple test executable
option(BIKE_BUILD_TESTS "Build BIKE test executable" OFF)
if(BIKE_BUILD_TESTS)
    add_executable(bike_test test_bike.c)
    target_link_libraries(bike_test bike_static)
    target_compile_definitions(bike_test PRIVATE ${COMMON_COMPILE_DEFS})
endif()