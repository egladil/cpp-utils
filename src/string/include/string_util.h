#pragma once

#include "inline_string.h"

namespace str {
namespace detail {
constexpr bool isUpper(char character) {
    return character >= 'A' && character <= 'Z';
}
constexpr bool isLower(char character) {
    return character >= 'a' && character <= 'z';
}
constexpr bool isDigit(char character) {
    return character >= '0' && character <= '9';
}
constexpr bool isSeparator(char character) {
    return character == '_' || character == '-' || character == ' ';
}

constexpr char toUpper(char character) {
    return isLower(character) ? character - 'a' + 'A' : character;
}
constexpr char toLower(char character) {
    return isUpper(character) ? character - 'A' + 'a' : character;
}

} // namespace detail

template <typename... TArgs> constexpr auto concatenate(TArgs&&... args) {
    return (std::forward<TArgs>(args) + ...);
}

template <size_t Alloc, typename F>
constexpr InlineString<Alloc> transform(const InlineString<Alloc>& string, F&& transformation) {
    InlineString<Alloc> result;
    for (const auto character : string) {
        result.append(transformation(character));
    }
    return result;
}

template <size_t Alloc>
constexpr auto replace(const InlineString<Alloc>& string, typename InlineString<Alloc>::value_type from,
                       typename InlineString<Alloc>::value_type to) {
    return transform(string, [&](auto character) { return character == from ? to : character; });
}

namespace detail {
template <size_t Alloc, typename F> constexpr auto toSnakeCase(const InlineString<Alloc>& string, F&& finalConversion) {
    InlineString<Alloc * 2 - 1> result;
    bool lastWasOther = false;
    bool lastWasUpper = false;
    bool lastWasDigit = false;
    bool lastWasSeparator = true;

    for (const auto character : string) {
        const bool curIsUpper = detail::isUpper(character);
        const bool curIsDigit = detail::isDigit(character);
        const bool curIsSeparator = detail::isSeparator(character);
        const bool curIsOther = !curIsUpper && !curIsDigit && !curIsSeparator;

        if ((lastWasOther && curIsUpper) || (lastWasOther && curIsDigit) || (lastWasUpper && curIsUpper) ||
            (lastWasUpper && curIsDigit) || (lastWasDigit && curIsOther) || (lastWasDigit && curIsUpper)) {
            result.append(finalConversion('_'));
        }

        if (!lastWasSeparator || !curIsSeparator) {
            const auto Modified = curIsSeparator ? '_' : toLower(character);
            result.append(finalConversion(Modified));
        }

        lastWasUpper = curIsUpper;
        lastWasDigit = curIsDigit;
        lastWasSeparator = curIsSeparator;
        lastWasOther = curIsOther;
    }

    return result;
}
} // namespace detail

template <size_t Alloc> constexpr auto toUpperSnakeCase(const InlineString<Alloc>& string) {
    return detail::toSnakeCase(string, &detail::toUpper);
}

template <size_t Alloc> constexpr auto toLowerSnakeCase(const InlineString<Alloc>& string) {
    return detail::toSnakeCase(string, &std::forward<InlineString<Alloc>::value_type>);
}

template <size_t Alloc> constexpr auto toPascalCase(const InlineString<Alloc>& string) {
    InlineString<Alloc> result;
    bool lastWasSeparator = true;

    for (const auto character : string) {
        if (detail::isSeparator(character)) {
            lastWasSeparator = true;
            continue;
        }

        if (lastWasSeparator) {
            result.append(detail::toUpper(character));
        } else {
            result.append(detail::toLower(character));
        }

        lastWasSeparator = false;
    }

    return result;
}

template <size_t Alloc> constexpr auto toCamelCase(const InlineString<Alloc>& string) {
    auto result = toPascalCase(string);

    if (result.length > 0) {
        result._data[0] = detail::toLower(result._data[0]);
    }

    return result;
}
} // namespace str
