#pragma once

#include <limits>

#pragma once

#include "javawrap.h"

namespace jwrap {

namespace detail {
using JavaLangObject = decltype("java/lang/Object"_class);
}

template <typename T> class Iterator : public TypedObject<decltype("java/util/Iterator"_class)> {
  public:
    Iterator(const Iterator& object) : TypedObject(static_cast<const TypedObject&>(object)) {}

    Iterator(Iterator&& object) : TypedObject(std::forward<TypedObject>(object)) {}

    explicit Iterator(const TypedObject& object) : TypedObject(object) {}

    explicit Iterator(TypedObject&& object) : TypedObject(std::forward<TypedObject>(object)) {}

    template <typename T2> explicit Iterator(const T2& object) : TypedObject(object) {}

    Iterator& operator=(const Iterator& other) {
        TypedObject::operator=(std::static_cast<const TypedObject&>(other));
        return *this;
    }

    Iterator& operator=(Iterator&& other) {
        TypedObject::operator=(std::forward<TypedObject>(other));
        return *this;
    }

    template <typename T2> Iterator& operator=(const T2& other) {
        TypedObject::operator=(other);
        return *this;
    }

    template <typename T2> Iterator& operator=(T2&& other) {
        TypedObject::operator=(std::forward<T2>(other));
        return *this;
    }

    bool hasNext() const { return handle != nullptr && invoke<bool>("hasNext"); }
    T next() { return T(invoke<TypedObject<detail::JavaLangObject>>("next")); }
};

namespace detail {

template <typename TList> class ListElement {
  public:
    using value_type = typename TList::value_type;
    using size_type = typename TList::size_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

  private:
    const TList& list;
    size_type index = 0;

  public:
    ListElement(const TList& list, size_type index) : list(list), index(index) {}

    operator value_type() const { return list.get((int32_t)index); }

    ListElement& operator=(const_reference value) {
        list.set((int32_t)index, value);
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

template <typename TList> class IteratorWrapper {
  public:
    struct EndTag {};

    using value_type = typename TList::value_type;
    using size_type = typename TList::size_type;
    using difference_type = typename TList::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;

  private:
    bool atEnd = false;
    Iterator<value_type> iterator;
    value_type current;

    void advance() {
        if (atEnd) {
            return;
        }

        if (iterator.hasNext()) {
            current = iterator.next();
        } else {
            atEnd = true;
        }
    }

  public:
    explicit IteratorWrapper(const EndTag&) : atEnd(true) {}

    explicit IteratorWrapper(const Iterator<value_type>& iterator) : iterator(iterator) { advance(); }

    inline const_reference operator*() const { return current; }
    /*
    inline auto operator->() const {
        return (**this).operator->();
    }
    */
    inline IteratorWrapper& operator++() {
        advance();
        return *this;
    }

    inline bool operator==(const IteratorWrapper& rhs) const {
        if (atEnd && rhs.atEnd) {
            return true;
        } else if (atEnd != rhs.atEnd) {
            return false;
        }

        return iterator == rhs.iterator;
    }

    inline bool operator!=(const IteratorWrapper& rhs) const { return !operator==(rhs); }
};

template <typename TDerived, typename T> class IterableOperations {
  public:
    using value_type = T;

    using iterator = detail::IteratorWrapper<TDerived>;
    using const_iterator = detail::IteratorWrapper<TDerived>;

    // Java
    Iterator<value_type> getIterator() const {
        return static_cast<const TDerived&>(*this).template invoke<Iterator>("iterator");
    }

    // C++
    iterator begin() { return iterator(getIterator()); }
    const_iterator begin() const { return const_iterator(getIterator()); }
    const_iterator cbegin() const { return begin(); }

    iterator end() { return iterator(iterator::EndTag()); }
    const_iterator end() const { return const_iterator(const_iterator::EndTag()); }
    const_iterator cend() const { return end(); }
};

template <typename TDerived, typename T> class CollectionOperations : public IterableOperations<TDerived, T> {
  public:
    using typename IterableOperations<TDerived, T>::value_type;
    using typename IterableOperations<TDerived, T>::iterator;
    using typename IterableOperations<TDerived, T>::const_iterator;

    using size_type = int32_t;
    using difference_type = int32_t;

    // Java
    bool add(const value_type& value) {
        return static_cast<TDerived&>(*this).template invoke<bool>("add", java_cast<detail::JavaLangObject>(value));
    }
    bool contains(const Object& obj) const {
        return static_cast<const TDerived&>(*this).template invoke<bool>("contains",
                                                                         java_cast<detail::JavaLangObject>(obj));
    }
    void remove(const Object& obj) {
        static_cast<TDerived&>(*this).template invoke<void>("remove", java_cast<detail::JavaLangObject>(obj));
    }

    // C++
    bool empty() const { return static_cast<const TDerived&>(*this).template invoke<bool>("isEmpty"); }
    size_type size() const { return (size_type) static_cast<const TDerived&>(*this).template invoke<int32_t>("size"); }
    constexpr size_type max_size() const { return (size_type)std::numeric_limits<int32_t>::max(); }
    // size_t capacity() const { return _size; }

    void clear() { static_cast<TDerived&>(*this).template invoke<void>("clear"); }
};

template <typename TDerived, typename T> class ListOperations : public CollectionOperations<TDerived, T> {
  public:
    using typename CollectionOperations<TDerived, T>::value_type;
    using typename CollectionOperations<TDerived, T>::size_type;
    using typename CollectionOperations<TDerived, T>::difference_type;
    using typename CollectionOperations<TDerived, T>::iterator;
    using typename CollectionOperations<TDerived, T>::const_iterator;

    using reference = ListElement<TDerived>;
    using const_reference = const ListElement<TDerived>;

    // Java
    using CollectionOperations<TDerived, T>::add;
    void add(int32_t index, const value_type& value) {
        static_cast<TDerived&>(*this).template invoke<void>("add"(index, java_cast<detail::JavaLangObject>(value)));
    }
    value_type get(int32_t index) const {
        return static_cast<value_type>(
            static_cast<const TDerived&>(*this).template invoke<TypedObject<detail::JavaLangObject>>("get", index));
    }
    int32_t indexOf(const Object& obj) const {
        return static_cast<const TDerived&>(*this).template invoke<int32_t>("indexOf",
                                                                            java_cast<detail::JavaLangObject>(obj));
    }
    int32_t lastIndexOf(const Object& obj) const {
        return static_cast<const TDerived&>(*this).template invoke<int32_t>("lastIndexOf",
                                                                            java_cast<detail::JavaLangObject>(obj));
    }
    value_type set(int32_t index, const value_type& value) {
        return static_cast<value_type>(
            static_cast<TDerived&>(*this).template invoke<TypedObject<detail::JavaLangObject>>(
                "set", index, java_cast<detail::JavaLangObject>(value)));
    }
    using CollectionOperations<TDerived, T>::remove;
    value_type remove(int32_t index) {
        return static_cast<value_type>(
            static_cast<TDerived&>(*this).template invoke<TypedObject<detail::JavaLangObject>>("remove", index));
    }

    // C++
    reference at(size_type pos) { return reference(*this, pos); }
    const_reference at(size_type pos) const { return const_reference(*this, pos); }

    reference operator[](size_type pos) { return at(pos); }
    const_reference operator[](size_type pos) const { return at(pos); }

    reference front() { return at(0); }
    const_reference front() const { return at(0); }

    reference back() { return at(size() - 1); }
    const_reference back() const { return at(size() - 1); }

    void push_back(const value_type& value) { add(value); }

    template <class... Args> value_type emplace_back(Args&&... args) {
        value_type value = value_type::clazz.clazz().newInstance(args...);
        add(value);
        return value;
    }

    void pop_back() { remove((int32_t)size() - 1); }

    void resize(size_type count) { resize(count, value_type()); }
    void resize(size_type count, const value_type& value) {
        size_type size = size();

        for (; size < count; size++) {
            add(value);
        }

        for (; size > count; size--) {
            remove((int32_t)size - 1);
        }
    }

    void assign(size_type count, const value_type& value) {
        CollectionOperations<TDerived, T>::clear();
        resize(count, value);
    }
    template <class InputIt> void assign(InputIt first, InputIt last) {
        CollectionOperations<TDerived, T>::clear();
        for (auto current = first; current != last; ++current) {
            add(*current);
        }
    }
    void assign(std::initializer_list<value_type> ilist) { assign(ilist.begin(), ilist.end()); }
};

} // namespace detail

template <typename T>
class Iterable : public TypedObject<decltype("java/util/Iterable"_class)>,
                 public detail::IterableOperations<Iterable<T>, T> {
  public:
    Iterable(const Iterable& object) : TypedObject(static_cast<const TypedObject&>(object)) {}

    Iterable(Iterable&& object) : TypedObject(std::forward<TypedObject>(object)) {}

    explicit Iterable(const TypedObject& object) : TypedObject(object) {}

    explicit Iterable(TypedObject&& object) : TypedObject(std::forward<TypedObject>(object)) {}

    template <typename T2> explicit Iterable(const T2& object) : TypedObject(object) {}

    Iterable& operator=(const Iterable& other) {
        TypedObject::operator=(std::static_cast<const TypedObject&>(other));
        return *this;
    }

    Iterable& operator=(Iterable&& other) {
        TypedObject::operator=(std::forward<TypedObject>(other));
        return *this;
    }

    template <typename T2> Iterable& operator=(const T2& other) {
        TypedObject::operator=(other);
        return *this;
    }

    template <typename T2> Iterable& operator=(T2&& other) {
        TypedObject::operator=(std::forward<T2>(other));
        return *this;
    }
};

template <typename T>
class Collection : public TypedObject<decltype("java/util/Collection"_class)>,
                   public detail::CollectionOperations<Collection<T>, T> {
  public:
    Collection(const Collection& object) : TypedObject(static_cast<const TypedObject&>(object)) {}

    Collection(Collection&& object) : TypedObject(std::forward<TypedObject>(object)) {}

    explicit Collection(const TypedObject& object) : TypedObject(object) {}

    explicit Collection(TypedObject&& object) : TypedObject(std::forward<TypedObject>(object)) {}

    template <typename T2> explicit Collection(const T2& object) : TypedObject(object) {}

    Collection& operator=(const Collection& other) {
        TypedObject::operator=(std::static_cast<const TypedObject&>(other));
        return *this;
    }

    Collection& operator=(Collection&& other) {
        TypedObject::operator=(std::forward<TypedObject>(other));
        return *this;
    }

    template <typename T2> Collection& operator=(const T2& other) {
        TypedObject::operator=(other);
        return *this;
    }

    template <typename T2> Collection& operator=(T2&& other) {
        TypedObject::operator=(std::forward<T2>(other));
        return *this;
    }
};

template <typename T>
class List : public TypedObject<decltype("java/util/List"_class)>, public detail::ListOperations<List<T>, T> {
  public:
    List(const List& object) : TypedObject(static_cast<const TypedObject&>(object)) {}

    List(List&& object) : TypedObject(std::forward<TypedObject>(object)) {}

    explicit List(const TypedObject& object) : TypedObject(object) {}

    explicit List(TypedObject&& object) : TypedObject(std::forward<TypedObject>(object)) {}

    template <typename T2> explicit List(const T2& object) : TypedObject(object) {}

    List& operator=(const List& other) {
        TypedObject::operator=(other);
        return *this;
    }

    List& operator=(List&& other) {
        TypedObject::operator=(std::forward<TypedObject>(other));
        return *this;
    }

    template <typename T2> List& operator=(const T2& other) {
        TypedObject::operator=(other);
        return *this;
    }

    template <typename T2> List& operator=(T2&& other) {
        TypedObject::operator=(std::forward<T2>(other));
        return *this;
    }
};

template <typename T, typename InputIt> auto makeArrayList(InputIt first, InputIt last) {
    List<T> result = Class::forName("java/util/ArrayList").newInstance((int32_t)(last - first));
    for (auto current = first; current != last; ++current) {
        result.add(*current);
    }
    return result;
}

template <typename T> auto makeArrayList(std::initializer_list<T> ilist) {
    return makeArrayList<T>(ilist.begin(), ilist.end());
}

template <typename TIterable> auto makeArrayList(const TIterable& iterable) {
    return makeArrayList<typename TIterable::value_type>(iterable.begin(), iterable.end());
}

template <typename T, typename InputIt> auto makeHashSet(InputIt first, InputIt last) {
    Collection<T> result = Class::forName("java/util/makeHashSet").newInstance((int32_t)(last - first));
    for (auto current = first; current != last; ++current) {
        result.add(*current);
    }
    return result;
}

template <typename T> auto makeHashSet(std::initializer_list<T> ilist) {
    return makeHashSet<T>(ilist.begin(), ilist.end());
}

template <typename TIterable> auto makeHashSet(const TIterable& iterable) {
    return makeHashSet<typename TIterable::value_type>(iterable.begin(), iterable.end());
}

} // namespace jwrap
