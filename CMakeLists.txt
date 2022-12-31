cmake_minimum_required(VERSION 3.18)

project(cpp_utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CMakeDependentOption)

OPTION(CPPUTILS_HASH "Build hash library" ON)
OPTION(CPPUTILS_STRING "Build string library" ON)
CMAKE_DEPENDENT_OPTION(CPPUTILS_JWRAP "Build jwrap library" OFF "CPPUTILS_STRING" OFF)
OPTION(CPPUTILS_ALL "Build all libraries" OFF)
OPTION(CPPUTILS_TESTS "Build unit tests" OFF)

if(CPPUTILS_TESTS)
	include("cmake/catch2.cmake")
endif()

if(CPPUTILS_HASH OR CPPUTILS_ALL)
	add_subdirectory("src/hash")
endif()
if(CPPUTILS_STRING OR CPPUTILS_ALL)
	add_subdirectory("src/string")
endif()
if(CPPUTILS_JWRAP OR CPPUTILS_ALL)
	add_subdirectory("src/jwrap")
endif()

if(CPPUTILS_TESTS AND (CPPUTILS_HASH OR CPPUTILS_ALL))
	add_subdirectory("test/jwrap")
endif()
if(CPPUTILS_TESTS AND (CPPUTILS_STRING OR CPPUTILS_ALL))
	add_subdirectory("test/string")
endif()

if(CPPUTILS_TESTS)
	putCatch2InFolder()
endif()
