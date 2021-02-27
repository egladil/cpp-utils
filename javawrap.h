#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "parse_exception.h"

struct JNIEnv_;
struct _JNIEnv;
struct _jmethodID;
struct _jfieldID;
class _jobject;
class _jclass;
class _jarray;

namespace jwrap {
#ifdef __ANDROID__
typedef _JNIEnv JNIEnv;
#else
typedef JNIEnv_ JNIEnv;
#endif

typedef _jobject* jobject;
typedef _jclass* jclass;
typedef _jarray* jarray;
typedef struct _jmethodID* jmethodID;
typedef uint64_t value_t;

class Object;
template <typename ClassId> class TypedObject;
class Class;

template <typename T> class TypedArray;

class NullException : public parse_exception {
  public:
    explicit NullException(const std::string& msg) : parse_exception(msg) {}
    explicit NullException(const char* msg) : parse_exception(msg) {}
};

class CastException : public parse_exception {
  public:
    explicit CastException(const std::string& msg) : parse_exception(msg) {}
    explicit CastException(const char* msg) : parse_exception(msg) {}
};

namespace detail {
template <size_t Size> struct StringWrapper {
    constexpr static const size_t size = Size;
    char chars[size];
};

template <size_t... Length> constexpr auto concatenate(const char (&... strings)[Length]) {
    constexpr size_t TotalLength = (... + Length) - sizeof...(Length);
    StringWrapper<TotalLength + 1> result = {};
    result.chars[TotalLength] = '\0';

    char* dst = result.chars;
    for (const char* src : {strings...}) {
        for (; *src != '\0'; src++, dst++) {
            *dst = *src;
        }
    }
    return result;
}

template <typename T> struct TypeSignature {
    constexpr static auto name = concatenate(T::clazz::name);
    constexpr static auto signature = concatenate("L", name.chars, ";");
};

template <> struct TypeSignature<void> {
    constexpr static auto signature = concatenate("V");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<bool> {
    constexpr static auto signature = concatenate("Z");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<uint8_t> {
    constexpr static auto signature = concatenate("B");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<uint16_t> {
    constexpr static auto signature = concatenate("C");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<int16_t> {
    constexpr static auto signature = concatenate("S");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<int32_t> {
    constexpr static auto signature = concatenate("I");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<int64_t> {
    constexpr static auto signature = concatenate("J");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<float> {
    constexpr static auto signature = concatenate("F");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<double> {
    constexpr static auto signature = concatenate("D");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<std::string_view> {
    constexpr static auto name = concatenate("java/lang/String");
    constexpr static auto signature = concatenate("L", name.chars, ";");
};

template <> struct TypeSignature<std::string> {
    constexpr static auto name = concatenate("java/lang/String");
    constexpr static auto signature = concatenate("L", name.chars, ";");
};

template <size_t Size> struct TypeSignature<const char[Size]> {
    constexpr static auto name = concatenate("java/lang/String");
    constexpr static auto signature = concatenate("L", name.chars, ";");
};

template <> struct TypeSignature<char*> {
    constexpr static auto name = concatenate("java/lang/String");
    constexpr static auto signature = concatenate("L", name.chars, ";");
};

template <> struct TypeSignature<const char*> {
    constexpr static auto name = concatenate("java/lang/String");
    constexpr static auto signature = concatenate("L", name.chars, ";");
};

template <typename T, size_t Size> struct TypeSignature<std::array<T, Size>> {
    constexpr static auto signature = concatenate("[", TypeSignature<T>::signature.chars);
    constexpr static auto name = signature;
};

template <typename T, size_t Size> struct TypeSignature<const T[Size]> {
    constexpr static auto signature = concatenate("[", TypeSignature<T>::signature.chars);
    constexpr static auto name = signature;
};

template <typename T> struct TypeSignature<const T*> {
    constexpr static auto signature = concatenate("[", TypeSignature<T>::signature.chars);
    constexpr static auto name = signature;
};

template <typename T> struct TypeSignature<TypedArray<T>> {
    constexpr static auto signature = concatenate("[", TypeSignature<T>::signature.chars);
    constexpr static auto name = signature;
};

template <typename TReturn, typename... TArgs> struct FunctionTypeSignature {
    constexpr static auto signature =
        concatenate("(", TypeSignature<TArgs>::signature.chars..., ")", TypeSignature<TReturn>::signature.chars);
};

value_t toValue(bool value);
value_t toValue(uint8_t value);
value_t toValue(uint16_t value);
value_t toValue(int16_t value);
value_t toValue(int32_t value);
value_t toValue(int64_t value);
value_t toValue(float value);
value_t toValue(double value);
value_t toValue(const std::string_view& value);
inline value_t toValue(const std::string& value) {
    return toValue(std::string_view(value));
}
inline value_t toValue(const char* value) {
    if (value == nullptr) {
        return 0;
    }
    return toValue(std::string_view(value));
}
value_t toValue(const Object& value);

template <typename T> void destroyTempValue(value_t value) {
}

template <> void destroyTempValue<std::string_view>(value_t value);
template <> inline void destroyTempValue<std::string>(value_t value) {
    destroyTempValue<std::string_view>(value);
}
template <> inline void destroyTempValue<char*>(value_t value) {
    destroyTempValue<std::string_view>(value);
}
template <> inline void destroyTempValue<const char*>(value_t value) {
    destroyTempValue<std::string_view>(value);
}

template <typename... TArgs> class Arguments {
  private:
    std::array<value_t, sizeof...(TArgs)> values;

    void gatherArguments(size_t) {}

    template <typename T, typename... TMoreArgs>
    void gatherArguments(size_t i, const T& arg, const TMoreArgs&... args) {
        values[i] = toValue(arg);
        gatherArguments(i + 1, args...);
    }

    template <typename T = void, typename... TMoreArgs> void destroyArguments(size_t i) {
        destroyTempValue<T>(values[i]);
        destroyArguments<TMoreArgs...>(i + 1);
    }

    template <> void destroyArguments<void>(size_t) {}

  public:
    Arguments(const TArgs&... args) { gatherArguments(0, args...); }

    ~Arguments() { destroyArguments<TArgs...>(0); }

    const value_t* data() const { return values.data(); }
};
} // namespace detail

template <char... chars> struct ClassId {
    constexpr static const char name[sizeof...(chars) + 1] = {chars..., '\0'};

    static Class clazz();

    using type = TypedObject<ClassId>;
};

template <typename T, T... chars> constexpr ClassId<chars...> operator""_class() {
    return {};
}

class Object {
  protected:
    jobject handle = nullptr;

    template <typename TReturn> TReturn invokeInternal(jmethodID method, const value_t* args) const {
        return static_cast<TReturn>(invokeInternal<Object>(method, args));
    }

    template <> void invokeInternal<void>(jmethodID method, const value_t* args) const;
    template <> bool invokeInternal<bool>(jmethodID method, const value_t* args) const;
    template <> uint8_t invokeInternal<uint8_t>(jmethodID method, const value_t* args) const;
    template <> uint16_t invokeInternal<uint16_t>(jmethodID method, const value_t* args) const;
    template <> int16_t invokeInternal<int16_t>(jmethodID method, const value_t* args) const;
    template <> int32_t invokeInternal<int32_t>(jmethodID method, const value_t* args) const;
    template <> int64_t invokeInternal<int64_t>(jmethodID method, const value_t* args) const;
    template <> float invokeInternal<float>(jmethodID method, const value_t* args) const;
    template <> double invokeInternal<double>(jmethodID method, const value_t* args) const;
    template <> std::string invokeInternal<std::string>(jmethodID method, const value_t* args) const;
    template <> Object invokeInternal<Object>(jmethodID method, const value_t* args) const;

  public:
    constexpr Object() noexcept = default;
    Object(const jobject& handle);
    Object(jobject&& handle);
    Object(const Object& object);
    Object(Object&& object);
    explicit Object(bool boxedValue);
    explicit Object(uint8_t boxedValue);
    explicit Object(uint16_t boxedValue);
    explicit Object(int16_t boxedValue);
    explicit Object(int32_t boxedValue);
    explicit Object(int64_t boxedValue);
    explicit Object(float boxedValue);
    explicit Object(double boxedValue);
    explicit Object(const std::string_view& string);
    explicit Object(const std::string& string) : Object(std::string_view(string)) {}
    ~Object();

    void nullCheck() const {
        if (*this == nullptr) {
            throw NullException("Object was null");
        }
    }

    void free();
    jobject newHandle() const;

    Object newLocalRef() const;
    Object newGlobalRef() const;
    Object newWeakGlobalRef() const;

    Object& operator=(const Object& other);
    Object& operator=(Object&& other);

    bool operator==(const Object& other) const;
    bool operator!=(const Object& other) const { return !operator==(other); }

    bool operator==(nullptr_t) const;
    bool operator!=(nullptr_t) const { return !operator==(nullptr); }

    explicit operator jobject() const { return handle; }

    Class getClass() const;

    template <typename TReturn, typename... TArgs> TReturn invoke(jmethodID methodId, const TArgs&... args) const {
        detail::Arguments<TArgs...> values(args...);
        return invokeInternal<TReturn>(methodId, values.data());
    }

    template <typename TReturn, typename... TArgs>
    TReturn invoke(const std::string_view& method, const TArgs&... args) const {
        jmethodID methodId = getClass().getMethod<TReturn, TArgs...>(method);
        return invoke<TReturn, TArgs...>(methodId, args...);
    }

    bool equals(const Object& other) const;
    int32_t hashCode() const;
    std::string toString() const;
};

template <char... ClassName> class TypedObject<ClassId<ClassName...>> : public Object {
  public:
    using clazz = ClassId<ClassName...>;

  protected:
    TypedObject(const jobject& handle, int32_t) : Object(handle) {}
    TypedObject(jobject&& handle, int32_t) : Object(std::forward<jobject>(handle)) {}

  public:
    constexpr TypedObject() noexcept = default;

    TypedObject(const TypedObject& object) : Object(object) {}
    TypedObject(TypedObject&& object) : Object(std::forward<Object>(object)) {}

    template <typename T> explicit TypedObject(const T& object) : Object(object) { typeCheck(); }

    void typeCheck() const {
        if (*this == nullptr) {
            return;
        }

        Class actual = getClass();
        if (!clazz::clazz().isAssignableFrom(actual)) {
            throw CastException(actual.getName() + " cannot be cast to " + std::string(clazz::name));
        }
    }

    TypedObject newLocalRef() const { return TypedObject(Object::newLocalRef()); }
    TypedObject newGlobalRef() const { return TypedObject(Object::newGlobalRef()); }
    TypedObject newWeakGlobalRef() const { return TypedObject(Object::newWeakGlobalRef()); }

    TypedObject& operator=(const TypedObject& other) {
        Object::operator=(other);
        return *this;
    }

    TypedObject& operator=(TypedObject&& other) {
        Object::operator=(std::forward<Object>(other));
        return *this;
    }

    template <typename T> TypedObject& operator=(const T& other) {
        Object::operator=(other);
        typeCheck();
        return *this;
    }

    template <typename T> TypedObject& operator=(T&& other) {
        Object::operator=(std::forward<Object>(other));
        typeCheck();
        return *this;
    }
};

class Class : public TypedObject<decltype("java/lang/Class"_class)> {
  private:
    Object newInstanceInternal(jmethodID method, const value_t* args) const;

    template <typename TReturn> TReturn invokeStaticInternal(jmethodID method, const value_t* args) const {
        return static_cast<TReturn>(invokeStaticInternal<Object>(method, args));
    }

    template <> void invokeStaticInternal<void>(jmethodID method, const value_t* args) const;
    template <> bool invokeStaticInternal<bool>(jmethodID method, const value_t* args) const;
    template <> uint8_t invokeStaticInternal<uint8_t>(jmethodID method, const value_t* args) const;
    template <> uint16_t invokeStaticInternal<uint16_t>(jmethodID method, const value_t* args) const;
    template <> int16_t invokeStaticInternal<int16_t>(jmethodID method, const value_t* args) const;
    template <> int32_t invokeStaticInternal<int32_t>(jmethodID method, const value_t* args) const;
    template <> int64_t invokeStaticInternal<int64_t>(jmethodID method, const value_t* args) const;
    template <> float invokeStaticInternal<float>(jmethodID method, const value_t* args) const;
    template <> double invokeStaticInternal<double>(jmethodID method, const value_t* args) const;
    template <> std::string invokeStaticInternal<std::string>(jmethodID method, const value_t* args) const;
    template <> Object invokeStaticInternal<Object>(jmethodID method, const value_t* args) const;

  public:
    Class(const Class& object) : TypedObject(static_cast<TypedObject>(object)) {}

    Class(Class&& object) : TypedObject(std::forward<TypedObject>(object)) {}

    Class(const TypedObject& object) : TypedObject(object) {}

    Class(TypedObject&& object) : TypedObject(std::forward<TypedObject>(object)) {}

    Class(const jclass& handle);
    Class(jclass&& handle);

    template <typename T> explicit Class(const T& object) : TypeObject(object) {}

    Class& operator=(const Class& other) {
        TypedObject::operator=(other);
        return *this;
    }

    Class& operator=(Class&& other) {
        TypedObject::operator=(std::forward<TypedObject>(other));
        return *this;
    }

    template <typename T> Class& operator=(const T& other) {
        TypedObject::operator=(other);
        return *this;
    }

    template <typename T> Class& operator=(T&& other) {
        TypedObject::operator=(std::forward<T>(other));
        return *this;
    }

    static Class forName(const std::string_view& name);

    template <typename... TArgs> Object newInstance(jmethodID methodId, const TArgs&... args) const {
        detail::Arguments<TArgs...> values(args...);
        return newInstanceInternal(methodId, values.data());
    }

    template <typename... TArgs> Object newInstance(const TArgs&... args) const {
        jmethodID methodId = getMethod<void, TArgs...>("<init>");
        return newInstance<TArgs...>(methodId, args...);
    }

    bool isAssignableFrom(const Class& subclass) const;

    std::string getName() const;

    jmethodID getMethod(const std::string_view& name, const std::string_view& signature) const;
    jmethodID getStaticMethod(const std::string_view& name, const std::string_view& signature) const;

    template <typename TReturn, typename... TArgs> jmethodID getMethod(const std::string_view& name) const {
        return getMethod(name, detail::FunctionTypeSignature<TReturn, TArgs...>::signature.chars);
    }

    template <typename TReturn, typename... TArgs> jmethodID getStaticMethod(const std::string_view& name) const {
        return getStaticMethod(name, detail::FunctionTypeSignature<TReturn, TArgs...>::signature.chars);
    }

    template <typename TReturn, typename... TArgs>
    TReturn invokeStatic(jmethodID methodId, const TArgs&... args) const {
        detail::Arguments<TArgs...> values(args...);
        return invokeStaticInternal<TReturn>(methodId, values.data());
    }

    template <typename TReturn, typename... TArgs>
    TReturn invokeStatic(const std::string_view& method, const TArgs&... args) const {
        jmethodID methodId = getStaticMethod<TReturn, TArgs...>(method);
        return invokeStatic<TReturn, TArgs...>(methodId, args...);
    }
};

template <char... chars> Class ClassId<chars...>::clazz() {
    return Class::forName(ClassId::name);
}

template <typename ClassId> TypedObject<ClassId> java_cast(const Object& object) {
    return TypedObject<ClassId>(object);
}

template <typename ClassId> TypedObject<ClassId> java_cast(Object&& object) {
    return TypedObject<ClassId>(std::forward<Object>(object));
}

void init(JNIEnv* env);
JNIEnv* getEnv();

} // namespace jwrap
