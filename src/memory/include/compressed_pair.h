#pragma once

#include <concepts>
#include <type_traits>

namespace memory {

namespace detail {

template <typename T1, typename T2> struct CompressedPairImpl {
    T1 first_;
    T2 second_;

    template <typename = void>
        requires(std::is_default_constructible_v<T1> && std::is_default_constructible_v<T2>)
    constexpr CompressedPairImpl() {}

    template <typename = void>
        requires(std::copy_constructible<T1> && std::copy_constructible<T2>)
    constexpr CompressedPairImpl(const T1& first, const T2& second) : first_(first), second_(second) {}

    template <typename = void>
        requires(std::move_constructible<T1> && std::move_constructible<T2>)
    constexpr CompressedPairImpl(T1&& first, T2&& second) : first_(std::move(first)), second_(std::move(second)) {}

    constexpr T1& first() { return first_; }
    constexpr const T1& first() const { return first_; }

    constexpr T2& second() { return second_; }
    constexpr const T2& second() const { return second_; }
};

template <typename T1, typename T2>
    requires(!std::is_empty_v<T1> && std::is_empty_v<T2>)
struct CompressedPairImpl<T1, T2> {
    T1 first_;

    template <typename = void>
        requires(std::is_default_constructible_v<T1>)
    constexpr CompressedPairImpl() {}

    template <typename = void>
        requires(std::copy_constructible<T1>)
    constexpr CompressedPairImpl(const T1& first, const T2& second) : first_(first) {}

    template <typename = void>
        requires(std::move_constructible<T1>)
    constexpr CompressedPairImpl(T1&& first, T2&& second) : first_(std::move(first)) {}

    constexpr T1& first() { return first_; }
    constexpr const T1& first() const { return first_; }

    constexpr T2 second() const { return {}; }
};

template <typename T1, typename T2>
    requires(std::is_empty_v<T1> && !std::is_empty_v<T2>)
struct CompressedPairImpl<T1, T2> {
    T2 second_;

    template <typename = void>
        requires(std::is_default_constructible_v<T2>)
    constexpr CompressedPairImpl() {}

    template <typename = void>
        requires(std::copy_constructible<T2>)
    constexpr CompressedPairImpl(const T1& first, const T2& second) : second_(second) {}

    template <typename = void>
        requires(std::move_constructible<T2>)
    constexpr CompressedPairImpl(T1&& first, T2&& second) : second_(std::move(second)) {}

    constexpr T1 first() const { return {}; }

    constexpr T2& second() { return second_; }
    constexpr const T2& second() const { return second_; }
};

template <typename T1, typename T2>
    requires(std::is_empty_v<T1> && std::is_empty_v<T2>)
struct CompressedPairImpl<T1, T2> {
    constexpr CompressedPairImpl() = default;
    constexpr CompressedPairImpl(const T1& first, const T2& second) {}
    constexpr CompressedPairImpl(T1&& first, T2&& second) {}

    constexpr T1 first() const { return {}; }
    constexpr T2 second() const { return {}; }
};

} // namespace detail

template <typename T1, typename T2> class CompressedPair : private detail::CompressedPairImpl<T1, T2> {
  private:
    using TBase = detail::CompressedPairImpl<T1, T2>;

  public:
    using first_type = T1;
    using second_type = T2;

  public:
    template <typename = void>
        requires(std::is_default_constructible_v<T1> && std::is_default_constructible_v<T2>)
    constexpr CompressedPair() {}

    template <typename = void>
        requires(std::copy_constructible<T1> && std::copy_constructible<T2>)
    constexpr CompressedPair(const T1& first, const T2& second) : TBase(first, second) {}

    template <typename = void>
        requires(std::move_constructible<T1> && std::move_constructible<T2>)
    constexpr CompressedPair(T1&& first, T2&& second) : TBase(std::move(first), std::move(second)) {}

    template <typename = void>
        requires(std::is_copy_assignable_v<T1> && std::is_copy_assignable_v<T2>)
    constexpr CompressedPair& operator=(const CompressedPair& o) {
        if constexpr (!std::is_empty_v<T1>) {
            TBase::first_ = o.first_;
        }
        if constexpr (!std::is_empty_v<T2>) {
            TBase::second_ = o.second_;
        }
    }

    template <typename = void>
        requires(std::is_move_assignable_v<T1> && std::is_move_assignable_v<T2>)
    constexpr CompressedPair& operator=(CompressedPair&& o) {
        if constexpr (!std::is_empty_v<T1>) {
            TBase::first_ = std::move(o.first_);
        }
        if constexpr (!std::is_empty_v<T2>) {
            TBase::second_ = std::move(o.second_);
        }
    }

    using TBase::first;
    using TBase::second;

    template <typename = void> friend constexpr bool operator==(const CompressedPair& lhs, const CompressedPair& rhs) {
        if constexpr (!std::is_empty_v<T1>) {
            if (lhs.first_ != rhs.first_) {
                return false;
            }
        }
        if constexpr (!std::is_empty_v<T2>) {
            if (lhs.second_ != rhs.second_) {
                return false;
            }
        }
        return true;
    }
};

} // namespace memory

namespace std {

template <typename T1, typename T2>
void swap(::memory::CompressedPair<T1, T2>& x, ::memory::CompressedPair<T1, T2>& y) {
    if constexpr (!std::is_empty_v<T1>) {
        swap(x.first(), y.first());
    }
    if constexpr (!std::is_empty_v<T2>) {
        swap(x.second(), y.second());
    }
}

} // namespace std
