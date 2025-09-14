#pragma once

#include <bit>
#include <compare>
#include <memory>
#include <stdint.h>

namespace memory {

template <typename T, size_t TopBits = 8, size_t Align = alignof(T)> class TaggedPtr {
  private:
    static constexpr size_t alignmentBits = std::bit_width(Align) - 1;
    static constexpr size_t pointerBits = sizeof(T*) * 8 - alignmentBits - TopBits;
    static constexpr uintptr_t topMask = ((1ull << TopBits) - 1);
    static constexpr size_t topShift = sizeof(T*) * 8 - TopBits;
    static constexpr uintptr_t alignmentMask = (1ull << alignmentBits) - 1;
    static constexpr uintptr_t tagMask = (topMask << topShift) | alignmentMask;
    static constexpr uintptr_t pointerMask = ~tagMask;

  public:
    static constexpr size_t tagBits = TopBits + alignmentBits;
    using pointer = T*;
    using element_type = T;

    class TagRef {
      private:
        TaggedPtr* ref;

      public:
        TagRef(TaggedPtr& ref) noexcept : ref(&ref) {}
        TagRef(TagRef&&) noexcept = default;
        TagRef(const TagRef&) noexcept = default;
        ~TagRef() noexcept = default;

        TagRef& operator=(TagRef&&) noexcept = default;
        TagRef& operator=(const TagRef&) noexcept = default;

        TagRef& operator=(uintptr_t value) noexcept {
            ref->setTag(value);
            return *this;
        }

        operator uintptr_t() const noexcept { return ref->getTag(); }
    };

  private:
    union {
        uintptr_t bits;
        T* ptr = {};
    };

  public:
    constexpr TaggedPtr() noexcept = default;
    constexpr TaggedPtr(TaggedPtr&&) noexcept = default;
    constexpr TaggedPtr(const TaggedPtr&) noexcept = default;
    constexpr ~TaggedPtr() noexcept = default;

    TaggedPtr(T* ptr) noexcept : ptr(ptr) { bits &= pointerMask; }

    TaggedPtr(T* ptr, uintptr_t tag) noexcept : ptr(ptr) { setTag(tag); }

    T* get() const noexcept {
        TaggedPtr copy(*this);
        copy.bits &= pointerMask;
        return copy.ptr;
    }

    operator T*() const noexcept { return get(); }
    std::add_lvalue_reference_t<T> operator*() const noexcept { return *get(); }
    T* operator->() const noexcept { return get(); }

    template <typename T2> explicit operator T2*() const noexcept { return static_cast<T2*>(get()); }

    TaggedPtr& operator=(const TaggedPtr&) noexcept = default;
    TaggedPtr& operator=(TaggedPtr&&) noexcept = default;

    TaggedPtr& operator=(T* ptr) noexcept {
        TaggedPtr copy(ptr);
        bits = copy.bits | (bits & tagMask);
        return *this;
    }

    friend std::weak_ordering operator<=>(const TaggedPtr& lhs, const TaggedPtr& rhs) noexcept {
        return lhs.get() <=> rhs.get();
    }

    friend std::weak_ordering operator<=>(const TaggedPtr& lhs, nullptr_t) noexcept { return lhs.get() <=> nullptr; }
    friend std::weak_ordering operator<=>(nullptr_t, const TaggedPtr& rhs) noexcept { return nullptr <=> rhs.get(); }

    friend std::weak_ordering operator<=>(const TaggedPtr& lhs, const T* rhs) noexcept { return lhs.get() <=> rhs; }
    friend std::weak_ordering operator<=>(const T* lhs, const TaggedPtr& rhs) noexcept { return lhs <=> rhs.get(); }

    void swap(TaggedPtr& other) noexcept { std::swap(ptr, other.ptr); }

    uintptr_t tag() const noexcept { return getTag(); }
    TagRef tag() noexcept { return TagRef(*this); }

  private:
    void setTag(uintptr_t value) noexcept {
        uintptr_t top = value & topMask;

        value >>= TopBits;
        uintptr_t align = value & alignmentMask;

        uintptr_t data = bits & pointerMask;
        data |= top << topShift;
        data |= align;
        bits = data;
    }

    uintptr_t getTag() const noexcept {
        uintptr_t data = bits;

        uintptr_t top = (data >> topShift) & topMask;
        uintptr_t align = data & alignmentMask;

        uintptr_t value = top | (align << TopBits);
        return value;
    }
};

template <typename T, typename Deleter = std::default_delete<T>, size_t TopBits = 8> class TaggedUniquePtr {
  private:
    using Pointer = TaggedPtr<T>;

  public:
    static constexpr size_t tagBits = Pointer::tagBits;
    using TagRef = typename Pointer::TagRef;

    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

  private:
    Pointer ptr;

  public:
    constexpr TaggedUniquePtr() noexcept = default;
    constexpr TaggedUniquePtr(nullptr_t) noexcept {}
    explicit TaggedUniquePtr(pointer p) noexcept : ptr(p) {}
    TaggedUniquePtr(const TaggedUniquePtr&) = delete;
    template <typename T2> TaggedUniquePtr(TaggedUniquePtr<T2>&& o) noexcept { *this = std::move(o); }
    template <typename T2> TaggedUniquePtr(std::unique_ptr<T2>&& o) noexcept { *this = std::move(o); }

    ~TaggedUniquePtr() { reset(); }

    TaggedUniquePtr& operator=(const TaggedUniquePtr&) = delete;

    TaggedUniquePtr& operator=(nullptr_t) noexcept {
        reset();
        return *this;
    }

    TaggedUniquePtr& operator=(TaggedUniquePtr&& rhs) noexcept {
        reset();
        swap(rhs);
        return *this;
    }

    template <typename T2> TaggedUniquePtr& operator=(TaggedUniquePtr<T2>&& rhs) noexcept {
        reset();
        uintptr_t tag = rhs.tag();
        ptr = rhs.release();
        ptr.tag() = tag;
        return *this;
    }

    template <typename T2> TaggedUniquePtr& operator=(std::unique_ptr<T2>&& rhs) noexcept {
        reset();
        ptr = rhs.release();
        return *this;
    }

    pointer release() noexcept {
        pointer old = ptr;
        ptr = nullptr;
        return old;
    }

    void reset(pointer ptr = {}) noexcept {
        if (this->ptr != nullptr) {
            deleter_type()(this->ptr.get());
        }
        this->ptr = ptr;
    }

    void swap(TaggedUniquePtr& other) noexcept { ptr.swap(other.ptr); }

    const TaggedPtr<T>& get() const noexcept { return ptr; }
    explicit operator bool() const noexcept { return ptr != nullptr; }

    std::add_lvalue_reference_t<T> operator*() const noexcept { return *ptr; }
    T* operator->() const noexcept { return ptr; }

    friend std::weak_ordering operator<=>(const TaggedUniquePtr& lhs, const TaggedUniquePtr& rhs) noexcept = default;
    friend std::weak_ordering operator<=>(const TaggedUniquePtr& lhs, nullptr_t) noexcept { return lhs <=> nullptr; }
    friend std::weak_ordering operator<=>(nullptr_t, const TaggedUniquePtr& rhs) noexcept { return nullptr <=> rhs; }

    uintptr_t tag() const noexcept { return ptr.tag(); }
    TagRef tag() noexcept { return ptr.tag(); }
};

} // namespace memory
