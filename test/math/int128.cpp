
#include "int128.h"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#if defined(CPPUTILS_INT128) && defined(CPPUTILS_UINT128)
#define TEST_TYPES int128_t, uint128_t
#elif defined(CPPUTILS_INT128)
#define TEST_TYPES int128_t
#elif defined(CPPUTILS_UINT128)
#define TEST_TYPES uint128_t
#else
#define TEST_TYPES
#endif

TEST_CASE("Storage", "[int128]") {
#ifdef CPPUTILS_INT128
    REQUIRE(sizeof(int128_t) == 16);
#endif
#ifdef CPPUTILS_UINT128
    REQUIRE(sizeof(uint128_t) == 16);
#endif
}

TEMPLATE_TEST_CASE("Addition", "[int128]", TEST_TYPES) {
    TestType a = 1;
    TestType b = 2;
    REQUIRE(a + b == 3);
}

TEMPLATE_TEST_CASE("Subtraction", "[int128]", TEST_TYPES) {
    TestType a = 17;
    TestType b = 5;
    REQUIRE(a - b == 12);
}

TEMPLATE_TEST_CASE("Multiplication", "[int128]", TEST_TYPES) {
    TestType a = 3;
    TestType b = 7;
    REQUIRE(a * b == 21);
}

TEMPLATE_TEST_CASE("Division", "[int128]", TEST_TYPES) {
    TestType a = 64;
    TestType b = 16;
    REQUIRE(a / b == 4);
}

TEMPLATE_TEST_CASE("Modulo", "[int128]", TEST_TYPES) {
    TestType a = 13;
    TestType b = 10;
    REQUIRE(a % b == 3);
}

TEMPLATE_TEST_CASE("Left shift", "[int128]", TEST_TYPES) {
    TestType a = 3;
    TestType b = 7;
    REQUIRE(a << b == 384);
}

TEMPLATE_TEST_CASE("Right shift", "[int128]", TEST_TYPES) {
    TestType a = 64;
    TestType b = 3;
    REQUIRE(a >> b == 8);
}
