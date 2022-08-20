#pragma once

#include <array>
#include <stdint.h>
#include <string>
#include <string_view>
#include <type_traits>

namespace str {
template <size_t Alloc> class InlineString;

template <size_t Alloc, typename F>
constexpr InlineString<Alloc> transform(const InlineString<Alloc>& string, F&& transformation);

namespace detail {
template <size_t Alloc, typename F> constexpr auto toSnakeCase(const InlineString<Alloc>& string, F&& finalConversion);
}

template <size_t Alloc> constexpr auto toPascalCase(const InlineString<Alloc>& string);
template <size_t Alloc> constexpr auto toCamelCase(const InlineString<Alloc>& string);

template <size_t Alloc> class InlineString {
  public:
    using value_type = std::string_view::value_type;
    template <size_t Alloc2> using TStorage = std::array<value_type, Alloc2>;
    static constexpr size_t bufferSize = Alloc;

    template <size_t Alloc2> friend class InlineString;

    template <size_t Alloc2, typename F>
    friend constexpr InlineString<Alloc2> str::transform(const InlineString<Alloc2>& string, F&& transformation);

    template <size_t Alloc2, typename F>
    friend constexpr auto str::detail::toSnakeCase(const InlineString<Alloc2>& string, F&& finalConversion);
    template <size_t Alloc2> friend constexpr auto str::toPascalCase(const InlineString<Alloc2>& string);
    template <size_t Alloc2> friend constexpr auto str::toCamelCase(const InlineString<Alloc2>& string);

  public:
    constexpr InlineString() = default;
    constexpr InlineString(const InlineString&) = default;
    constexpr InlineString(InlineString&&) = default;

    constexpr InlineString(std::string_view stringView) { append(stringView); }

    constexpr InlineString(std::initializer_list<value_type> data) { append(data); }

    constexpr InlineString(value_type const (&data)[Alloc]) { append<std::string_view>(data); }

    constexpr InlineString(const TStorage<Alloc>& data) { append(data); }

    template <size_t Alloc2> constexpr InlineString(const TStorage<Alloc2>& data) { append(data); }

    constexpr auto begin() const { return toStringView().begin(); }
    constexpr auto end() const { return toStringView().end(); }
    constexpr auto cbegin() const { return toStringView().cbegin(); }
    constexpr auto cend() const { return toStringView().cend(); }

    constexpr operator std::string_view() const { return toStringView(); }
    constexpr std::string_view toStringView() const { return {_data.data(), length}; }

    constexpr const auto& data() const { return _data; }
    constexpr size_t size() const { return length; }

    template <size_t Alloc2> friend constexpr auto operator+(const InlineString& lhs, const InlineString<Alloc2>& rhs) {
        InlineString<Alloc + Alloc2 - 1> result(lhs);
        result.append(rhs);
        return result;
    }

    template <size_t Alloc2> friend constexpr auto operator+(value_type const (&lhs)[Alloc2], const InlineString& rhs) {
        InlineString<Alloc + Alloc2 - 1> result(lhs);
        result.append(rhs);
        return result;
    }

    template <size_t Alloc2> friend constexpr auto operator+(const InlineString& lhs, value_type const (&rhs)[Alloc2]) {
        InlineString<Alloc + Alloc2 - 1> result(lhs);
        result.append(rhs);
        return result;
    }

    friend std::string operator+(const std::string& lhs, const InlineString& rhs) {
        std::string result;
        result.reserve(lhs.size() + rhs.size());
        result.append(lhs);
        result.append(rhs);
        return result;
    }

    friend std::string operator+(const InlineString& lhs, const std::string& rhs) {
        std::string result;
        result.reserve(lhs.size() + rhs.size());
        result.append(lhs);
        result.append(rhs);
        return result;
    }

  private:
    constexpr void append(std::string_view stringView) {
        for (const value_type character : stringView) {
            if (length + 1 >= Alloc) {
                return;
            }

            _data[length] = character;
            ++length;
        }
    }

    template <typename T> constexpr void append(const T& data) {
        for (const value_type character : data) {
            if (length + 1 >= Alloc) {
                return;
            }
            if (character == 0) {
                return;
            }

            _data[length] = character;
            ++length;
        }
    }

    constexpr void append(value_type character) {
        if (length + 1 >= Alloc) {
            return;
        }

        _data[length] = character;
        ++length;
    }

  private:
    size_t length = 0;
    TStorage<Alloc> _data = {0};
};

namespace detail {

template <typename T> constexpr size_t length(T&& data) {
    return std::char_traits<std::string_view::value_type>::length(std::forward<T>(data));
}

template <size_t Alloc> constexpr size_t length(const InlineString<Alloc>& data) {
    return data.size();
}

template <size_t Alloc> constexpr size_t length(InlineString<Alloc>&& data) {
    return data.size();
}

constexpr size_t length(std::string_view data) {
    return data.size();
}

} // namespace detail
} // namespace str

#define OptimizedInlineString(string) str::InlineString<str::detail::length(string) + 1>(string)
