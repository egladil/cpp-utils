cmake_minimum_required(VERSION 3.18)

project(cpp_utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include("cmake/catch2.cmake")

add_subdirectory("src/hash")
add_subdirectory("src/string")
add_subdirectory("src/jwrap")

add_subdirectory("test/jwrap")
add_subdirectory("test/string")

putCatch2InFolder()
