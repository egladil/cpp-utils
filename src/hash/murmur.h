
#include <iterator>
#include <stdint.h>
#include <string_view>

namespace hash {
namespace detail {
template <typename> struct MurmurConstants { static_assert(false, "Only 32 and 64 bit unsigned integer supported"); };

template <> struct MurmurConstants<uint32_t> {
    static constexpr int8_t MixShiftA = 16;
    static constexpr int8_t MixShiftB = 13;
    static constexpr int8_t MixShiftC = 16;
    static constexpr uint32_t MixConstantA = 0x85ebca6bul;
    static constexpr uint32_t MixConstantB = 0xc2b2ae35ul;

    static constexpr int8_t Rotate1 = 15;
    static constexpr int8_t Rotate2 = 13;
    static constexpr uint32_t Constant1 = 0xcc9e2d51ul;
    static constexpr uint32_t Constant2 = 0x1b873593ul;
    static constexpr uint32_t Constant3 = 0xe6546b64ul;
};

template <> struct MurmurConstants<uint64_t> {
    static constexpr int8_t MixShiftA = 33;
    static constexpr int8_t MixShiftB = 33;
    static constexpr int8_t MixShiftC = 33;
    static constexpr uint64_t MixConstantA = 0xff51afd7ed558ccdull;
    static constexpr uint64_t MixConstantB = 0xc4ceb9fe1a85ec53ull;

    static constexpr int8_t Rotate1 = 31;
    static constexpr int8_t Rotate2 = 27;
    static constexpr uint32_t Constant1 = 0x87c37b91114253d5ull;
    static constexpr uint32_t Constant2 = 0x4cf5ad432745937full;
    static constexpr uint32_t Constant3 = 0x52dce729ull;
};

template <typename T> constexpr T rotl(T value, int8_t count) {
    return (value << count) | (value >> (sizeof(T) * 8 - count));
}

template <typename T> constexpr T fmix(T value) {
    using Constants = detail::MurmurConstants<T>;

    value ^= value >> Constants::MixShiftA;
    value *= Constants::MixConstantA;
    value ^= value >> Constants::MixShiftB;
    value *= Constants::MixConstantB;
    value ^= value >> Constants::MixShiftC;
    return value;
}

} // namespace detail

template <typename T, typename TBegin, typename TEnd>
constexpr T murmurHash3(const TBegin& begin, const TEnd& end, T seed = 0) {
    static_assert(sizeof(*begin) == 1, "Iterators must produce single byte values");

    using Constants = detail::MurmurConstants<T>;
    constexpr size_t BlockSize = sizeof(T);

    const size_t length = std::distance(begin, end);
    const size_t blockCount = length / BlockSize;

    T hash = seed;
    auto iterator = begin;

    for (size_t i = 0; i < blockCount; ++i) {
        T block = 0;
        for (size_t j = 0; j < BlockSize; ++j) {
            uint8_t byte = static_cast<uint8_t>(*iterator);
            ++iterator;
            block |= static_cast<T>(byte) << (j * 8);
        }

        block *= Constants::Constant1;
        block = detail::rotl(block, Constants::Rotate1);
        block *= Constants::Constant2;

        hash ^= block;
        hash = detail::rotl(hash, Constants::Rotate2);
        hash = hash * 5 + Constants::Constant3;
    }

    if (iterator != end) {
        T block = 0;
        for (size_t offset = 0; iterator != end; ++iterator, offset += 8) {
            uint8_t byte = static_cast<uint8_t>(*iterator);
            block ^= static_cast<T>(byte) << offset;
        }

        block *= Constants::Constant1;
        block = detail::rotl(block, Constants::Rotate1);
        block *= Constants::Constant2;

        hash ^= block;
    }

    hash ^= static_cast<T>(length);
    hash = detail::fmix(hash);

    return hash;
}

inline constexpr uint32_t murmurHash3(const std::string_view& stringView, const uint32_t seed = 0) {
    return murmurHash3<uint32_t>(stringView.begin(), stringView.end(), seed);
}

} // namespace hash
