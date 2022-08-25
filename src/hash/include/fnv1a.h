#pragma once

#include <stdint.h>
#include <string_view>

namespace hash {
namespace detail {
template <typename> struct Fnv1Constants { static_assert(false, "Only 32 and 64 bit unsigned integer supported"); };

template <> struct Fnv1Constants<uint32_t> {
    static constexpr uint32_t Prime = 0x01000193ul;
    static constexpr uint32_t Offset = 0x811c9dc5ul;
};

template <> struct Fnv1Constants<uint64_t> {
    static constexpr uint64_t Prime = 0x00000100000001B3ull;
    static constexpr uint64_t Offset = 0xcbf29ce484222325ull;
};
} // namespace detail

template <typename T, typename TBegin, typename TEnd> constexpr T fnv1a(const TBegin& begin, const TEnd& end) {
    static_assert(sizeof(*begin) == 1, "Iterators must produce single byte values");

    T hash = detail::Fnv1Constants<T>::Offset;

    for (auto iterator = begin; iterator != end; ++iterator) {
        uint8_t byte = static_cast<uint8_t>(*iterator);
        hash ^= static_cast<uint64_t>(byte);
        hash *= detail::Fnv1Constants<T>::Prime;
    }

    return hash;
}

template <typename T> constexpr T fnv1a(const std::string_view& stringView) {
    return fnv1a<T>(stringView.begin(), stringView.end());
}

} // namespace hash
