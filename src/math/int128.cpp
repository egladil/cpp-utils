#include "int128.h"

// Workaround for clang-cl
#ifdef CPPUTILS_MSVC_INT128
#include <__msvc_int128.hpp>

namespace {
template <typename TBuiltin, typename TStruct> [[maybe_unused]] TBuiltin divti3(TBuiltin lhs, TBuiltin rhs) {
    TStruct value(static_cast<uint64_t>(lhs), static_cast<uint64_t>(lhs >> 64));
    value /= TStruct(static_cast<uint64_t>(rhs), static_cast<uint64_t>(rhs >> 64));

    uint64_t low = static_cast<uint64_t>(value);
    uint64_t high = static_cast<uint64_t>(value >> 64);

    uint128_t result = low | (static_cast<uint128_t>(high) << 64);
    return static_cast<TBuiltin>(result);
}

template <typename TBuiltin, typename TStruct> [[maybe_unused]] TBuiltin modti3(TBuiltin lhs, TBuiltin rhs) {
    TStruct value(static_cast<uint64_t>(lhs), static_cast<uint64_t>(lhs >> 64));
    value %= TStruct(static_cast<uint64_t>(rhs), static_cast<uint64_t>(rhs >> 64));

    uint64_t low = static_cast<uint64_t>(value);
    uint64_t high = static_cast<uint64_t>(value >> 64);

    uint128_t result = low | (static_cast<uint128_t>(high) << 64);
    return static_cast<TBuiltin>(result);
}
} // namespace

#ifdef CPPUTILS_INT128_BUILTIN
extern "C" int128_t __divti3(int128_t lhs, int128_t rhs) {
    return divti3<int128_t, std::_Signed128>(lhs, rhs);
}

extern "C" int128_t __modti3(int128_t lhs, int128_t rhs) {
    return modti3<int128_t, std::_Signed128>(lhs, rhs);
}
#endif

#ifdef CPPUTILS_UINT128_BUILTIN
extern "C" uint128_t __udivti3(uint128_t lhs, uint128_t rhs) {
    return divti3<uint128_t, std::_Unsigned128>(lhs, rhs);
}

extern "C" uint128_t __umodti3(uint128_t lhs, uint128_t rhs) {
    return modti3<uint128_t, std::_Unsigned128>(lhs, rhs);
}
#endif

#endif
