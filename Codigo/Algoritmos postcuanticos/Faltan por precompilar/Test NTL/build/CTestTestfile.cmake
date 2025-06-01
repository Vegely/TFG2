# CMake generated Testfile for 
# Source directory: C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL
# Build directory: C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(ntl_tests "C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/build/Debug/ntl_test.exe")
  set_tests_properties(ntl_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/CMakeLists.txt;24;add_test;C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(ntl_tests "C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/build/Release/ntl_test.exe")
  set_tests_properties(ntl_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/CMakeLists.txt;24;add_test;C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(ntl_tests "C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/build/MinSizeRel/ntl_test.exe")
  set_tests_properties(ntl_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/CMakeLists.txt;24;add_test;C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(ntl_tests "C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/build/RelWithDebInfo/ntl_test.exe")
  set_tests_properties(ntl_tests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/CMakeLists.txt;24;add_test;C:/Users/bogurad/Desktop/Proyectos gitHub/TFG2/Codigo/Algoritmos postcuanticos/Faltan por precompilar/Test NTL/CMakeLists.txt;0;")
else()
  add_test(ntl_tests NOT_AVAILABLE)
endif()
