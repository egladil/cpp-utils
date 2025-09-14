
include(CheckTypeSize)
include(CMakePushCheckState)
include(CheckIncludeFileCXX)

set(SOURCE_FILES
    int128.cpp
)

set(HEADER_FILES
    gen/int128.h
)

add_library(math ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(math PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" "${CMAKE_CURRENT_BINARY_DIR}/gen/")

cmake_push_check_state()
list(APPEND CMAKE_EXTRA_INCLUDE_FILES "stdint.h")

check_type_size("int128_t" CPPUTILS_INT128_STDINT LANGUAGE CXX)
check_type_size("__int128_t" CPPUTILS_INT128_BUILTIN_T LANGUAGE CXX)
check_type_size("__int128" CPPUTILS_INT128_BUILTIN LANGUAGE CXX)
check_type_size("_BitInt(128)" CPPUTILS_BITINT128 LANGUAGE CXX)
check_type_size("_ExtInt(128)" CPPUTILS_EXTINT128 LANGUAGE CXX)

check_type_size("uint128_t" CPPUTILS_UINT128_STDINT LANGUAGE CXX)
check_type_size("__uint128_t" CPPUTILS_UINT128_BUILTIN_T LANGUAGE CXX)
check_type_size("__uint128" CPPUTILS_UINT128_BUILTIN LANGUAGE CXX)
check_type_size("unsigned _BitInt(128)" CPPUTILS_BITUINT128 LANGUAGE CXX)
check_type_size("unsigned _ExtInt(128)" CPPUTILS_EXTUINT128 LANGUAGE CXX)

check_include_file_cxx("__msvc_int128.hpp" CPPUTILS_MSVC_INT128)

cmake_pop_check_state()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/gen/int128.h" "${CMAKE_CURRENT_BINARY_DIR}/gen/int128.h")
