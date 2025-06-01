cmake_minimum_required(VERSION 3.10)
project(NTL_GF2_Operations C)

set(CMAKE_C_STANDARD 99)

# Flags por compilador
if(MSVC)
    add_compile_options(/W3)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Crear ejecutable
add_executable(ntl_test ntl.c unit_tests.c)

# Enlazar matemáticas en Unix
if(UNIX)
    target_link_libraries(ntl_test m)
endif()

# Habilitar testing
enable_testing()
add_test(NAME ntl_tests COMMAND ntl_test)