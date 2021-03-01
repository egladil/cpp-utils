#pragma once

#include "javawrap.h"

namespace jwrap {

namespace detail {
template <typename T> Object arrayCreate(const std::string_view& name, size_t size) {
    return arrayCreate<Object>(name, size);
}

template <> Object arrayCreate<bool>(const std::string_view&, size_t size);
template <> Object arrayCreate<uint8_t>(const std::string_view&, size_t size);
template <> Object arrayCreate<uint16_t>(const std::string_view&, size_t size);
template <> Object arrayCreate<int16_t>(const std::string_view&, size_t size);
template <> Object arrayCreate<int32_t>(const std::string_view&, size_t size);
template <> Object arrayCreate<int64_t>(const std::string_view&, size_t size);
template <> Object arrayCreate<float>(const std::string_view&, size_t size);
template <> Object arrayCreate<double>(const std::string_view&, size_t size);
template <> Object arrayCreate<Object>(const std::string_view& name, size_t size);

template <typename T> T arrayGet(const Object& array, size_t index) {
    return static_cast<T>(arrayGet<Object>(array, index));
}

template <> bool arrayGet<bool>(const Object& array, size_t index);
template <> uint8_t arrayGet<uint8_t>(const Object& array, size_t index);
template <> uint16_t arrayGet<uint16_t>(const Object& array, size_t index);
template <> int16_t arrayGet<int16_t>(const Object& array, size_t index);
template <> int32_t arrayGet<int32_t>(const Object& array, size_t index);
template <> int64_t arrayGet<int64_t>(const Object& array, size_t index);
template <> float arrayGet<float>(const Object& array, size_t index);
template <> double arrayGet<double>(const Object& array, size_t index);
template <> std::string arrayGet<std::string>(const Object& array, size_t index);
template <> Object arrayGet<Object>(const Object& array, size_t index);

template <typename T> void arraySet(const Object& array, size_t index, const T& value) {
    arraySet<Object>(array, index, value);
}

template <> void arraySet<bool>(const Object& array, size_t index, const bool& value);
template <> void arraySet<uint8_t>(const Object& array, size_t index, const uint8_t& value);
template <> void arraySet<uint16_t>(const Object& array, size_t index, const uint16_t& value);
template <> void arraySet<int16_t>(const Object& array, size_t index, const int16_t& value);
template <> void arraySet<int32_t>(const Object& array, size_t index, const int32_t& value);
template <> void arraySet<int64_t>(const Object& array, size_t index, const int64_t& value);
template <> void arraySet<float>(const Object& array, size_t index, const float& value);
template <> void arraySet<double>(const Object& array, size_t index, const double& value);
template <> void arraySet<std::string>(const Object& array, size_t index, const std::string& value);
template <> void arraySet<Object>(const Object& array, size_t index, const Object& value);

template <typename T> void arrayInitialize(const Object& array, const T* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        arraySet<T>(array, i, data[i]);
    }
}

template <> void arrayInitialize<bool>(const Object& array, const bool* data, size_t size);
template <> void arrayInitialize<uint8_t>(const Object& array, const uint8_t* data, size_t size);
template <> void arrayInitialize<uint16_t>(const Object& array, const uint16_t* data, size_t size);
template <> void arrayInitialize<int16_t>(const Object& array, const int16_t* data, size_t size);
template <> void arrayInitialize<int32_t>(const Object& array, const int32_t* data, size_t size);
template <> void arrayInitialize<int64_t>(const Object& array, const int64_t* data, size_t size);
template <> void arrayInitialize<float>(const Object& array, const float* data, size_t size);
template <> void arrayInitialize<double>(const Object& array, const double* data, size_t size);

template <typename TArray> class ArrayElement {
  private:
    const TArray& array;
    size_t index = 0;

  public:
    using value_type = typename TArray::value_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    ArrayElement(const TArray& array, size_t index) : array(array), index(index) {}

    operator value_type() const { return arrayGet<value_type>(array, index); }

    ArrayElement& operator=(const_reference value) {
        arraySet<value_type>(array, index, value);
        return *this;
    }
    /*
    inline auto operator->() {
        return static_cast<value_type>(*this).operator->();
    }

    inline auto operator->() const {
        return static_cast<const_reference>(*this).operator->();
    }
    */
};

template <typename TArray> class ArrayIterator {
  private:
    const TArray& array;
    size_t index = 0;

  public:
    using value_type = typename TArray::value_type;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = ArrayElement<TArray>;
    using const_reference = const ArrayElement<TArray>;

    ArrayIterator(const TArray& array, size_t index) : array(array), index(index) {}

    inline ArrayIterator& operator+=(difference_type rhs) {
        index += rhs;
        return *this;
    }

    inline ArrayIterator& operator-=(difference_type rhs) {
        index -= rhs;
        return *this;
    }

    inline reference operator*() const { return *this[0]; }
    /*
    inline auto operator->() const {
        return (**this).operator->();
    }
    */
    inline reference operator[](difference_type rhs) const { return ArrayElement<TArray>(array, index + rhs); }

    inline ArrayIterator& operator++() {
        ++index;
        return *this;
    }

    inline ArrayIterator& operator--() {
        --index;
        return *this;
    }

    inline ArrayIterator operator++(int) const {
        ArrayIterator tmp(*this);
        ++index;
        return tmp;
    }

    inline ArrayIterator operator--(int) const {
        ArrayIterator tmp(*this);
        --index;
        return tmp;
    }

    inline difference_type operator-(const ArrayIterator& rhs) const { return ArrayIterator(index - rhs.index); }

    inline ArrayIterator operator+(difference_type rhs) const { return ArrayIterator(index + rhs); }

    inline ArrayIterator operator-(difference_type rhs) const { return ArrayIterator(index - rhs); }

    friend inline ArrayIterator operator+(difference_type lhs, const ArrayIterator& rhs) {
        return ArrayIterator(lhs + rhs.index);
    }

    friend inline ArrayIterator operator-(difference_type lhs, const ArrayIterator& rhs) {
        return ArrayIterator(lhs - rhs.index);
    }

    inline bool operator==(const ArrayIterator& rhs) const { return index == rhs.index; }

    inline bool operator!=(const ArrayIterator& rhs) const { return index != rhs.index; }

    inline bool operator>(const ArrayIterator& rhs) const { return index > rhs.index; }

    inline bool operator<(const ArrayIterator& rhs) const { return index < rhs.index; }

    inline bool operator>=(const ArrayIterator& rhs) const { return index >= rhs.index; }

    inline bool operator<=(const ArrayIterator& rhs) const { return index <= rhs.index; }
};

template <typename TArray> class ArrayConstIterator {
  private:
    const TArray& array;
    size_t index = 0;

  public:
    using value_type = typename TArray::value_type;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = ArrayElement<TArray>;
    using const_reference = const ArrayElement<TArray>;

    ArrayConstIterator(const TArray& array, size_t index) : array(array), index(index) {}

    inline ArrayConstIterator& operator+=(difference_type rhs) {
        index += rhs;
        return *this;
    }

    inline ArrayConstIterator& operator-=(difference_type rhs) {
        index -= rhs;
        return *this;
    }

    inline const_reference operator*() const { return *this[0]; }
    /*
    inline auto operator->() const {
        return (**this).operator->();
    }
    */
    inline const_reference operator[](difference_type rhs) const { return ArrayElement<TArray>(array, index + rhs); }

    inline ArrayConstIterator& operator++() {
        ++index;
        return *this;
    }

    inline ArrayConstIterator& operator--() {
        --index;
        return *this;
    }

    inline ArrayConstIterator operator++(int) const {
        ArrayConstIterator tmp(*this);
        ++index;
        return tmp;
    }

    inline ArrayConstIterator operator--(int) const {
        ArrayConstIterator tmp(*this);
        --index;
        return tmp;
    }

    inline difference_type operator-(const ArrayConstIterator& rhs) const {
        return ArrayConstIterator(index - rhs.index);
    }

    inline ArrayConstIterator operator+(difference_type rhs) const { return ArrayConstIterator(index + rhs); }

    inline ArrayConstIterator operator-(difference_type rhs) const { return ArrayConstIterator(index - rhs); }

    friend inline ArrayConstIterator operator+(difference_type lhs, const ArrayConstIterator& rhs) {
        return ArrayConstIterator(lhs + rhs.index);
    }

    friend inline ArrayConstIterator operator-(difference_type lhs, const ArrayConstIterator& rhs) {
        return ArrayConstIterator(lhs - rhs.index);
    }

    inline bool operator==(const ArrayConstIterator& rhs) const { return index == rhs.index; }

    inline bool operator!=(const ArrayConstIterator& rhs) const { return index != rhs.index; }

    inline bool operator>(const ArrayConstIterator& rhs) const { return index > rhs.index; }

    inline bool operator<(const ArrayConstIterator& rhs) const { return index < rhs.index; }

    inline bool operator>=(const ArrayConstIterator& rhs) const { return index >= rhs.index; }

    inline bool operator<=(const ArrayConstIterator& rhs) const { return index <= rhs.index; }
};

template <typename T, typename TDerived, typename ClassId> class ArrayBase : public TypedObject<ClassId> {
  public:
    using classId = typename TypedObject<ClassId>::classId;

  protected:
    size_t _size;

  public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = ArrayElement<TDerived>;
    using const_reference = const ArrayElement<TDerived>;

    using iterator = ArrayIterator<TDerived>;
    using const_iterator = ArrayConstIterator<TDerived>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    ArrayBase(const ArrayBase& object) : TypedObject<ClassId>(static_cast<TypedObject<ClassId>&>(object)) {}

    ArrayBase(ArrayBase&& object) : TypedObject<ClassId>(std::forward<TypedObject<ClassId>>(object)) {}

    ArrayBase(const TypedObject<ClassId>& object) : TypedObject<ClassId>(object) {}

    ArrayBase(TypedObject<ClassId>&& object) : TypedObject<ClassId>(std::forward<TypedObject<ClassId>>(object)) {}

    explicit ArrayBase(size_t size)
        : TypedObject<ClassId>(arrayCreate<T>(TypeSignature<T>::name.chars, size)), _size(size) {}

    ArrayBase(const T* data, size_t size) : ArrayBase(size) { arrayInitialize<T>(*this, data, _size); }

    template <size_t Size> explicit ArrayBase(const T (&data)[Size]) : ArrayBase(data, Size) {}

    template <size_t Size> explicit ArrayBase(const std::array<T, Size>& data) : ArrayBase(data.data(), Size) {}

    explicit ArrayBase(const std::initializer_list<T>& data) : ArrayBase(data.begin(), data.size()) {}

    template <typename T2> explicit ArrayBase(const T2& object) : TypedObject<ClassId>(object) {}

    TDerived& operator=(const TDerived& other) {
        TypedObject::operator=(other);
        return static_cast<TDerived&>(*this);
    }

    TDerived& operator=(TDerived&& other) {
        TypedObject::operator=(std::forward<TypedObject>(other));
        return static_cast<TDerived&>(*this);
    }

    template <typename T2> TDerived& operator=(const T2& other) {
        TypedObject::operator=(other);
        return static_cast<TDerived&>(*this);
    }

    template <typename T2> TDerived& operator=(T2&& other) {
        TypedObject::operator=(std::forward<T2>(other));
        return static_cast<TDerived&>(*this);
    }

    reference at(size_t pos) { return reference(static_cast<TDerived&>(*this), pos); }
    const_reference at(size_t pos) const { return const_reference(static_cast<const TDerived&>(*this), pos); }

    reference operator[](size_t pos) { return at(pos); }
    const_reference operator[](size_t pos) const { return at(pos); }

    reference front() { return at(0); }
    const_reference front() const { return at(0); }

    reference back() { return at(_size - 1); }
    const_reference back() const { return at(_size - 1); }

    iterator begin() { return iterator(static_cast<TDerived&>(*this), 0); }
    const_iterator begin() const { return const_iterator(static_cast<const TDerived&>(*this), 0); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return iterator(static_cast<TDerived&>(*this), _size); }
    const_iterator end() const { return const_iterator(static_cast<const TDerived&>(*this), _size); }
    const_iterator cend() const { return end(); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const { return rbegin(); }

    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const { return rend(); }

    bool empty() const { return _size == 0; }
    size_t size() const { return _size; }
    size_t max_size() const { return _size; }
    size_t capacity() const { return _size; }

    void fill(const value_type& value) {
        for (auto& element : *this) {
            element = value;
        }
    }
    /*
    void swap(TDerived& other) {
        std::swap(handle, other.handle);
        std::swap(_size, other.size);
    }
    */
};
} // namespace detail

template <typename T> class TypedArray;

template <char... chars>
class TypedArray<TypedObject<ClassId<chars...>>>
    : public detail::ArrayBase<TypedObject<ClassId<chars...>>, TypedArray<TypedObject<ClassId<chars...>>>,
                               ClassId<'[', 'L', chars..., ';'>> {
  public:
    using detail::ArrayBase<TypedObject<ClassId<chars...>>, TypedArray<TypedObject<ClassId<chars...>>>,
                            ClassId<'[', 'L', chars..., ';'>>::ArrayBase;
};

template <> class TypedArray<bool> : public detail::ArrayBase<bool, TypedArray<bool>, decltype("[Z"_class)> {
  public:
    using detail::ArrayBase<bool, TypedArray<bool>, decltype("[Z"_class)>::ArrayBase;

    void fill(const value_type& value);
};

template <> class TypedArray<uint8_t> : public detail::ArrayBase<uint8_t, TypedArray<uint8_t>, decltype("[B"_class)> {
  public:
    using detail::ArrayBase<uint8_t, TypedArray<uint8_t>, decltype("[B"_class)>::ArrayBase;

    void fill(const value_type& value);
};

template <>
class TypedArray<uint16_t> : public detail::ArrayBase<uint16_t, TypedArray<uint16_t>, decltype("[C"_class)> {
  public:
    using detail::ArrayBase<uint16_t, TypedArray<uint16_t>, decltype("[C"_class)>::ArrayBase;

    void fill(const value_type& value);
};

template <> class TypedArray<int16_t> : public detail::ArrayBase<int16_t, TypedArray<int16_t>, decltype("[S"_class)> {
  public:
    using detail::ArrayBase<int16_t, TypedArray<int16_t>, decltype("[S"_class)>::ArrayBase;

    void fill(const value_type& value);
};

template <> class TypedArray<int32_t> : public detail::ArrayBase<int32_t, TypedArray<int32_t>, decltype("[I"_class)> {
  public:
    using detail::ArrayBase<int32_t, TypedArray<int32_t>, decltype("[I"_class)>::ArrayBase;

    void fill(const value_type& value);
};

template <> class TypedArray<int64_t> : public detail::ArrayBase<int64_t, TypedArray<int64_t>, decltype("[J"_class)> {
  public:
    using detail::ArrayBase<int64_t, TypedArray<int64_t>, decltype("[J"_class)>::ArrayBase;

    void fill(const value_type& value);
};

template <> class TypedArray<float> : public detail::ArrayBase<float, TypedArray<float>, decltype("[F"_class)> {
  public:
    using detail::ArrayBase<float, TypedArray<float>, decltype("[F"_class)>::ArrayBase;

    void fill(const value_type& value);
};

template <> class TypedArray<double> : public detail::ArrayBase<double, TypedArray<double>, decltype("[D"_class)> {
  public:
    using detail::ArrayBase<double, TypedArray<double>, decltype("[D"_class)>::ArrayBase;

    void fill(const value_type& value);
};

template <>
class TypedArray<std::string>
    : public detail::ArrayBase<std::string, TypedArray<std::string>, decltype("[Ljava/lang/String;"_class)> {
  public:
    using detail::ArrayBase<std::string, TypedArray<std::string>, decltype("[Ljava/lang/String;"_class)>::ArrayBase;

    void fill(const value_type& value);
};
} // namespace jwrap
