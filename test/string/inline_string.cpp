
#include "inline_string.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Constructor", "[InlineString]") {
    constexpr std::string_view stringView("test");

    SECTION("default") {
        constexpr str::InlineString<128> string;
        REQUIRE(string.size() == 0);
        REQUIRE(string == std::string_view());
    }

    SECTION("copy") {
        constexpr str::InlineString<128> original(stringView);
        constexpr str::InlineString<128> string(original);
        REQUIRE(string.size() == stringView.size());
        REQUIRE(string == stringView);
    }

    SECTION("move") {
        constexpr str::InlineString<128> string([&]() { return str::InlineString<128>(stringView); }());
        REQUIRE(string.size() == stringView.size());
        REQUIRE(string == stringView);
    }

    SECTION("std::string_view") {
        constexpr str::InlineString<128> string(stringView);
        REQUIRE(string.size() == stringView.size());
        REQUIRE(string == stringView);
    }

    SECTION("std::initializer_list") {
        constexpr str::InlineString<128> string({'t', 'e', 's', 't'});
        REQUIRE(string.size() == stringView.size());
        REQUIRE(string == stringView);
    }

    SECTION("char[]") {
        constexpr str::InlineString<128> string("test");
        REQUIRE(string.size() == stringView.size());
        REQUIRE(string == stringView);
    }

    SECTION("std::array") {
        constexpr std::array<std::string_view::value_type, 4> array({'t', 'e', 's', 't'});
        constexpr str::InlineString<128> string(array);
        REQUIRE(string.size() == stringView.size());
        REQUIRE(string == stringView);
    }
}

TEST_CASE("Iterator", "[InlineString]") {
    using std::begin;
    using std::cbegin;
    using std::cend;
    using std::end;

    constexpr std::string_view stringView("test");
    constexpr str::InlineString<128> string(stringView);

    SECTION("begin/end") {
        auto stringViewIterator = begin(stringView);
        auto stringIterator = begin(string);

        REQUIRE(stringIterator != end(string));

        while (stringViewIterator != end(stringView) && stringIterator != end(string)) {
            REQUIRE(*stringIterator == *stringViewIterator);

            ++stringViewIterator;
            ++stringIterator;
        }

        REQUIRE(stringViewIterator == end(stringView));
        REQUIRE(stringIterator == end(string));
    }

    SECTION("cbegin/cend") {
        auto stringViewIterator = cbegin(stringView);
        auto stringIterator = cbegin(string);

        REQUIRE(stringIterator != cend(string));

        while (stringViewIterator != cend(stringView) && stringIterator != cend(string)) {
            REQUIRE(*stringIterator == *stringViewIterator);

            ++stringViewIterator;
            ++stringIterator;
        }

        REQUIRE(stringViewIterator == cend(stringView));
        REQUIRE(stringIterator == cend(string));
    }
}

TEST_CASE("Cast", "[InlineString]") {
    constexpr std::string_view stringView("test");
    constexpr str::InlineString<128> string(stringView);

    REQUIRE(static_cast<std::string_view>(string) == stringView);
    REQUIRE(string.toStringView() == stringView);
}

TEST_CASE("Operator +", "[InlineString]") {
    constexpr std::string_view lhsStringView("test");
    constexpr std::string_view rhsStringView("1234");
    constexpr str::InlineString<128> lhs(lhsStringView);
    constexpr str::InlineString<128> rhs(rhsStringView);

    SECTION("both") {
        constexpr auto string = lhs + rhs;
        REQUIRE(string == std::string_view("test1234"));
    }

    SECTION("lhs char[]") {
        constexpr auto string = "test" + rhs;
        REQUIRE(string == std::string_view("test1234"));
    }
    SECTION("rhs char[]") {
        constexpr auto string = lhs + "1234";
        REQUIRE(string == std::string_view("test1234"));
    }

    SECTION("lhs std::string") {
        auto string = std::string(lhsStringView) + rhs;
        REQUIRE(string == std::string("test1234"));
    }
    SECTION("rhs std::string") {
        auto string = lhs + std::string(rhsStringView);
        REQUIRE(string == std::string("test1234"));
    }
}
