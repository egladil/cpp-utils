#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include "inline_string.h"
#include "string_util.h"

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
typedef struct _jfieldID* jfieldID;
typedef uint64_t value_t;

class Object;
template <typename ClassId> class TypedObject;
class Class;

template <typename T> class TypedArray;

class JWrapException : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

class NullException : public JWrapException {
  public:
    using JWrapException::JWrapException;
};

class CastException : public JWrapException {
  public:
    using JWrapException::JWrapException;
};

namespace detail {
template <typename T> struct TypeSignature {
    constexpr static auto name = T::classId::signature;
    constexpr static auto signature = OptimizedInlineString("L" + name + ";");
};

template <> struct TypeSignature<void> {
    constexpr static auto signature = str::InlineString("V");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<bool> {
    constexpr static auto signature = str::InlineString("Z");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<uint8_t> {
    constexpr static auto signature = str::InlineString("B");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<uint16_t> {
    constexpr static auto signature = str::InlineString("C");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<int16_t> {
    constexpr static auto signature = str::InlineString("S");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<int32_t> {
    constexpr static auto signature = str::InlineString("I");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<int64_t> {
    constexpr static auto signature = str::InlineString("J");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<float> {
    constexpr static auto signature = str::InlineString("F");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<double> {
    constexpr static auto signature = str::InlineString("D");
    constexpr static auto name = signature;
};

template <> struct TypeSignature<std::string_view> {
    constexpr static auto name = str::InlineString("java/lang/String");
    constexpr static auto signature = OptimizedInlineString("L" + name + ";");
};

template <> struct TypeSignature<std::string> {
    constexpr static auto name = str::InlineString("java/lang/String");
    constexpr static auto signature = OptimizedInlineString("L" + name + ";");
};

template <size_t Size> struct TypeSignature<char[Size]> {
    constexpr static auto name = str::InlineString("java/lang/String");
    constexpr static auto signature = OptimizedInlineString("L" + name + ";");
};

template <size_t Size> struct TypeSignature<const char[Size]> {
    constexpr static auto name = str::InlineString("java/lang/String");
    constexpr static auto signature = OptimizedInlineString("L" + name + ";");
};

template <> struct TypeSignature<char*> {
    constexpr static auto name = str::InlineString("java/lang/String");
    constexpr static auto signature = OptimizedInlineString("L" + name + ";");
};

template <> struct TypeSignature<const char*> {
    constexpr static auto name = str::InlineString("java/lang/String");
    constexpr static auto signature = OptimizedInlineString("L" + name + ";");
};

template <typename T, size_t Size> struct TypeSignature<std::array<T, Size>> {
    constexpr static auto signature = OptimizedInlineString("[" + TypeSignature<T>::signature);
    constexpr static auto name = signature;
};

template <typename T, size_t Size> struct TypeSignature<const T[Size]> {
    constexpr static auto signature = OptimizedInlineString("[" + TypeSignature<T>::signature);
    constexpr static auto name = signature;
};

template <typename T> struct TypeSignature<const T*> {
    constexpr static auto signature = OptimizedInlineString("[" + TypeSignature<T>::signature);
    constexpr static auto name = signature;
};

template <typename T> struct TypeSignature<TypedArray<T>> {
    constexpr static auto signature = OptimizedInlineString("[" + TypeSignature<T>::signature);
    constexpr static auto name = signature;
};

template <typename TReturn, typename... TArgs> struct FunctionTypeSignature {
    constexpr static auto signature = OptimizedInlineString(
        str::concatenate("(", TypeSignature<TArgs>::signature..., ")", TypeSignature<TReturn>::signature));
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
    constexpr static const auto name = OptimizedInlineString(str::InlineString<sizeof...(chars) + 1>({chars..., '\0'}));
    constexpr static const auto signature = str::replace(name, '.', '/');

    static Class clazz();

    using type = TypedObject<ClassId>;
};

template <typename T, T... chars> constexpr ClassId<chars...> operator""_class() {
    return {};
}

template <typename TReturn, typename... TArgs> class MethodId {
  private:
    jmethodID id;

  public:
    explicit MethodId(jmethodID id) : id(id) {}

    operator jmethodID() const { return id; }
};

template <typename T> class FieldId {
  private:
    jfieldID id;

  public:
    explicit FieldId(jfieldID id) : id(id) {}

    operator jfieldID() const { return id; }
};

namespace detail {

template <typename T> class FieldAccess {
  private:
    const Object& obj;
    FieldId<T> fieldId;

  public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    FieldAccess(const Object& obj, FieldId<T> fieldId) : obj(obj), fieldId(fieldId) {}

    operator value_type() const { return obj.get<T>(fieldId); }

    FieldAccess& operator=(const_reference value) {
        obj.set<T>(fieldId, value);
        return *this;
    }
};
} // namespace detail

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

    template <typename T> T getInternal(jfieldID field) const { return static_cast<T>(getInternal<Object>(field)); }

    template <> bool getInternal<bool>(jfieldID field) const;
    template <> uint8_t getInternal<uint8_t>(jfieldID field) const;
    template <> uint16_t getInternal<uint16_t>(jfieldID field) const;
    template <> int16_t getInternal<int16_t>(jfieldID field) const;
    template <> int32_t getInternal<int32_t>(jfieldID field) const;
    template <> int64_t getInternal<int64_t>(jfieldID field) const;
    template <> float getInternal<float>(jfieldID field) const;
    template <> double getInternal<double>(jfieldID field) const;
    template <> std::string getInternal<std::string>(jfieldID field) const;
    template <> Object getInternal<Object>(jfieldID field) const;

    void setInternal(jfieldID field, bool value) const;
    void setInternal(jfieldID field, uint8_t value) const;
    void setInternal(jfieldID field, uint16_t value) const;
    void setInternal(jfieldID field, int16_t value) const;
    void setInternal(jfieldID field, int32_t value) const;
    void setInternal(jfieldID field, int64_t value) const;
    void setInternal(jfieldID field, float value) const;
    void setInternal(jfieldID field, double value) const;
    void setInternal(jfieldID field, const std::string_view& value) const;
    void setInternal(jfieldID field, const Object& value) const;

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

    template <typename TReturn, typename... TArgs>
    TReturn invoke(MethodId<TReturn, TArgs...> methodId, const TArgs&... args) const {
        detail::Arguments<TArgs...> values(args...);
        return invokeInternal<TReturn>(methodId, values.data());
    }

    template <typename TReturn, typename... TArgs>
    TReturn invoke(const std::string_view& method, const TArgs&... args) const {
        auto methodId = getClass().getMethod<TReturn, TArgs...>(method);
        return invoke<TReturn, TArgs...>(methodId, args...);
    }

    template <typename TReturn, typename... TArgs> auto operator->*(MethodId<TReturn, TArgs...> methodId) {
        return [this, methodId](const TArgs&... args) { return invoke<TReturn, TArgs...>(methodId, args...); };
    }

    template <typename T> T get(FieldId<T> fieldId) const { return getInternal<T>(fieldId); }

    template <typename T> void set(FieldId<T> fieldId, const T& value) const { setInternal(fieldId, value); }

    template <typename T> T get(const std::string_view& field) const {
        auto fieldId = getClass().getField<T>(field);
        return get<T>(fieldId);
    }

    template <typename T> void set(const std::string_view& field, const T& value) const {
        auto fieldId = getClass().getField<T>(field);
        set<T>(fieldId, value);
    }

    template <typename T> auto operator->*(FieldId<T> fieldId) { return detail::FieldAccess<T>(*this, fieldId); }

    bool equals(const Object& other) const;
    int32_t hashCode() const;
    std::string toString() const;
};

template <char... ClassName> class TypedObject<ClassId<ClassName...>> : public Object {
  public:
    using classId = ClassId<ClassName...>;
    static Class clazz();

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
        if (!clazz().isAssignableFrom(actual)) {
            constexpr auto messageTail = OptimizedInlineString(" cannot be cast to " + classId::name);
            throw CastException(actual.getName() + messageTail);
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

    template <typename TReturn, typename... TArgs>
    static TReturn invokeStatic(MethodId<TReturn, TArgs...> methodId, const TArgs&... args) {
        return clazz().invokeStatic<TReturn, TArgs...>(methodId, args...);
    }

    template <typename TReturn, typename... TArgs>
    static TReturn invokeStatic(const std::string_view& method, const TArgs&... args) {
        return clazz().invokeStatic<TReturn, TArgs...>(method, args...);
    }

    template <typename T> static T getStatic(FieldId<T> fieldId) { return clazz().getStatic<T>(fieldId); }

    template <typename T> static void setStatic(FieldId<T> fieldId, const T& value) {
        clazz().setStatic<T>(fieldId, value);
    }

    template <typename T> static T getStatic(const std::string_view& field) { return clazz().getStatic<T>(field); }

    template <typename T> static void setStatic(const std::string_view& field, const T& value) {
        clazz().setStatic<T>(field, value);
    }
};

class Class : public TypedObject<decltype("java.lang.Class"_class)> {
  private:
    jmethodID getMethod(const std::string_view& name, const std::string_view& signature) const;
    jmethodID getStaticMethod(const std::string_view& name, const std::string_view& signature) const;
    jfieldID getField(const std::string_view& name, const std::string_view& signature) const;
    jfieldID getStaticField(const std::string_view& name, const std::string_view& signature) const;

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

    template <typename T> T getStaticInternal(jfieldID field) const {
        return static_cast<T>(getStaticInternal<Object>(field));
    }

    template <> bool getStaticInternal<bool>(jfieldID field) const;
    template <> uint8_t getStaticInternal<uint8_t>(jfieldID field) const;
    template <> uint16_t getStaticInternal<uint16_t>(jfieldID field) const;
    template <> int16_t getStaticInternal<int16_t>(jfieldID field) const;
    template <> int32_t getStaticInternal<int32_t>(jfieldID field) const;
    template <> int64_t getStaticInternal<int64_t>(jfieldID field) const;
    template <> float getStaticInternal<float>(jfieldID field) const;
    template <> double getStaticInternal<double>(jfieldID field) const;
    template <> std::string getStaticInternal<std::string>(jfieldID field) const;
    template <> Object getStaticInternal<Object>(jfieldID field) const;

    void setStaticInternal(jfieldID field, bool value) const;
    void setStaticInternal(jfieldID field, uint8_t value) const;
    void setStaticInternal(jfieldID field, uint16_t value) const;
    void setStaticInternal(jfieldID field, int16_t value) const;
    void setStaticInternal(jfieldID field, int32_t value) const;
    void setStaticInternal(jfieldID field, int64_t value) const;
    void setStaticInternal(jfieldID field, float value) const;
    void setStaticInternal(jfieldID field, double value) const;
    void setStaticInternal(jfieldID field, const std::string_view& value) const;
    void setStaticInternal(jfieldID field, const Object& value) const;

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

    template <typename... TArgs> Object newInstance(MethodId<void, TArgs...> methodId, const TArgs&... args) const {
        detail::Arguments<TArgs...> values(args...);
        return newInstanceInternal(methodId, values.data());
    }

    template <typename... TArgs> Object newInstance(const TArgs&... args) const {
        auto methodId = getMethod<void, TArgs...>("<init>");
        return newInstance<TArgs...>(methodId, args...);
    }

    bool isAssignableFrom(const Class& subclass) const;

    std::string getName() const { return invoke<std::string>("getName"); }

    template <typename TReturn, typename... TArgs> auto getMethod(const std::string_view& name) const {
        return MethodId<TReturn, TArgs...>(
            getMethod(name, detail::FunctionTypeSignature<TReturn, TArgs...>::signature));
    }

    template <typename TReturn, typename... TArgs> auto getStaticMethod(const std::string_view& name) const {
        return MethodId<TReturn, TArgs...>(
            getStaticMethod(name, detail::FunctionTypeSignature<TReturn, TArgs...>::signature));
    }

    template <typename T> auto getField(const std::string_view& name) const {
        return FieldId<T>(getField(name, detail::TypeSignature<T>::signature));
    }

    template <typename T> auto getStaticField(const std::string_view& name) const {
        return FieldId<T>(getStaticField(name, detail::TypeSignature<T>::signature));
    }

    template <typename TReturn, typename... TArgs>
    TReturn invokeStatic(MethodId<TReturn, TArgs...> methodId, const TArgs&... args) const {
        detail::Arguments<TArgs...> values(args...);
        return invokeStaticInternal<TReturn>(methodId, values.data());
    }

    template <typename TReturn, typename... TArgs>
    TReturn invokeStatic(const std::string_view& method, const TArgs&... args) const {
        auto methodId = getStaticMethod<TReturn, TArgs...>(method);
        return invokeStatic<TReturn, TArgs...>(methodId, args...);
    }

    template <typename T> T getStatic(FieldId<T> fieldId) const { return getStaticInternal<T>(fieldId); }

    template <typename T> void setStatic(FieldId<T> fieldId, const T& value) const {
        setStaticInternal(fieldId, value);
    }

    template <typename T> T getStatic(const std::string_view& field) const {
        auto fieldId = getStaticField<T>(field);
        return getStatic<T>(fieldId);
    }

    template <typename T> void setStatic(const std::string_view& field, const T& value) const {
        auto fieldId = getStaticField<T>(field);
        setStatic<T>(fieldId, value);
    }
};

using Throwable = TypedObject<decltype("java.lang.Throwable"_class)>;

class JavaException : public JWrapException {
  private:
    Throwable cause;

  public:
    explicit JavaException(const std::string& msg, const Throwable& cause) : JWrapException(msg), cause(cause) {}
    explicit JavaException(const std::string& msg, Throwable&& cause)
        : JWrapException(msg), cause(std::forward<Throwable>(cause)) {}
    explicit JavaException(const char* msg, const Throwable& cause) : JWrapException(msg), cause(cause) {}
    explicit JavaException(const char* msg, Throwable&& cause)
        : JWrapException(msg), cause(std::forward<Throwable>(cause)) {}

    const Throwable& getCause() const { return cause; }
};

template <typename T> T throwReturn(const Throwable& throwable) {
    throwReturn<void>(throwable);
    return T();
}

template <> void throwReturn(const Throwable& throwable);

template <typename T> T rethrowReturn(const JavaException& ex) {
    Throwable throwable(Class::forName("java/lang/RuntimeException").newInstance(ex.what(), ex.getCause()));
    return throwReturn<T>(throwable);
}

template <char... chars> Class ClassId<chars...>::clazz() {
    return Class::forName(ClassId::signature);
}

template <char... ClassName> Class TypedObject<ClassId<ClassName...>>::clazz() {
    return ClassId<ClassName...>::clazz();
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
