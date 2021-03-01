#include "javawrap.h"

#include <sstream>

#include <jni.h>

namespace {
class NullTerminatedString {
  private:
    const std::string_view& view;
    std::string string;

  public:
    NullTerminatedString(const std::string_view& view) : view(view) {
        const char* nullTerminator = view.data() + view.size();
        if (*nullTerminator != '\0') {
            string = std::string(view);
        }
    }

    operator const char*() const {
        if (string.size() == view.size()) {
            return string.data();
        } else {
            return view.data();
        }
    }
};

void handleJniExceptionInternal(const std::string_view& info, const std::string_view& file, int32_t line) {
    jthrowable exception = jwrap::getEnv()->ExceptionOccurred();
    if (exception == nullptr) {
        return;
    }

    jwrap::getEnv()->ExceptionClear();

    std::ostringstream msg;
    msg << info << " (" << file << ":" << line << ")";

    throw jwrap::JavaException(msg.str(), jwrap::Throwable(std::move(exception)));
}

#define handleJniException(x) handleJniExceptionInternal(x, __FILE__, __LINE__)

std::string asString(jstring string) {
    const char* temp = jwrap::getEnv()->GetStringUTFChars(string, nullptr);
    std::string result = temp;
    jwrap::getEnv()->ReleaseStringUTFChars(string, temp);

    handleJniException("string conversion");

    return result;
}
} // namespace

namespace jwrap {

namespace detail {
value_t toValue(bool value) {
    jvalue val;
    val.z = value;
    return reinterpret_cast<value_t&>(val);
}

value_t toValue(uint8_t value) {
    jvalue val;
    val.b = value;
    return reinterpret_cast<value_t&>(val);
}

value_t toValue(uint16_t value) {
    jvalue val;
    val.c = value;
    return reinterpret_cast<value_t&>(val);
}

value_t toValue(int16_t value) {
    jvalue val;
    val.s = value;
    return reinterpret_cast<value_t&>(val);
}

value_t toValue(int32_t value) {
    jvalue val;
    val.i = value;
    return reinterpret_cast<value_t&>(val);
}

value_t toValue(int64_t value) {
    jvalue val;
    val.j = value;
    return reinterpret_cast<value_t&>(val);
}

value_t toValue(float value) {
    jvalue val;
    val.f = value;
    return reinterpret_cast<value_t&>(val);
}

value_t toValue(double value) {
    jvalue val;
    val.d = value;
    return reinterpret_cast<value_t&>(val);
}

value_t toValue(const std::string_view& value) {
    jvalue val;

    val.l = getEnv()->NewStringUTF(NullTerminatedString(value));

    handleJniException("string conversion");

    return reinterpret_cast<value_t&>(val);
}

value_t toValue(const Object& value) {
    jvalue val;
    val.l = static_cast<jobject>(value);
    return reinterpret_cast<value_t&>(val);
}

template <> void destroyTempValue<std::string_view>(value_t value) {
    if (value != 0) {
        getEnv()->DeleteLocalRef(reinterpret_cast<jvalue&>(value).l);
        handleJniException("destroy temp value");
    }
}
} // namespace detail

Object::Object(const jobject& handle) {
    this->handle = getEnv()->NewLocalRef(handle);
    handleJniException("new local ref");
}

Object::Object(jobject&& handle) : handle(std::move(handle)) {
    handle = nullptr;
}

Object::Object(const Object& object) {
    this->handle = getEnv()->NewLocalRef(object.handle);
    handleJniException("new local ref");
}

Object::Object(Object&& object) : handle(std::move(object.handle)) {
    object.handle = nullptr;
}

Object::Object(bool boxedValue) {
    *this = Class::forName("java/lang/Boolean").newInstance(boxedValue);
}

Object::Object(uint8_t boxedValue) {
    *this = Class::forName("java/lang/Boolean").newInstance(boxedValue);
}

Object::Object(uint16_t boxedValue) {
    *this = Class::forName("java/lang/Boolean").newInstance(boxedValue);
}

Object::Object(int16_t boxedValue) {
    *this = Class::forName("java/lang/Boolean").newInstance(boxedValue);
}

Object::Object(int32_t boxedValue) {
    *this = Class::forName("java/lang/Boolean").newInstance(boxedValue);
}

Object::Object(int64_t boxedValue) {
    *this = Class::forName("java/lang/Boolean").newInstance(boxedValue);
}

Object::Object(float boxedValue) {
    *this = Class::forName("java/lang/Boolean").newInstance(boxedValue);
}

Object::Object(double boxedValue) {
    *this = Class::forName("java/lang/Boolean").newInstance(boxedValue);
}

Object::Object(const std::string_view& string) {
    handle = getEnv()->NewStringUTF(NullTerminatedString(string));
    handleJniException("string conversion");
}

Object::~Object() {
    free();
}

void Object::free() {
    switch (getEnv()->GetObjectRefType(handle)) {
    case JNIInvalidRefType: break;
    case JNILocalRefType: getEnv()->DeleteLocalRef(handle); break;
    case JNIGlobalRefType: getEnv()->DeleteGlobalRef(handle); break;
    case JNIWeakGlobalRefType: getEnv()->DeleteWeakGlobalRef(handle); break;
    }

    handle = nullptr;
}

jobject Object::newHandle() const {
    if (handle == nullptr) {
        return nullptr;
    }

    jobject result = getEnv()->NewLocalRef(handle);
    handleJniException("new local ref");
    return std::move(result);
}

Object Object::newLocalRef() const {
    jobject result = getEnv()->NewLocalRef(handle);
    handleJniException("new local ref");
    return std::move(result);
}

Object Object::newGlobalRef() const {
    jobject result = getEnv()->NewGlobalRef(handle);
    handleJniException("new global ref");
    return std::move(result);
}

Object Object::newWeakGlobalRef() const {
    jobject result = getEnv()->NewWeakGlobalRef(handle);
    handleJniException("new weak global ref");
    return std::move(result);
}

Object& Object::operator=(const Object& other) {
    free();

    handle = getEnv()->NewLocalRef(other.handle);
    handleJniException("new local ref");

    return *this;
}

Object& Object::operator=(Object&& other) {
    free();

    handle = other.handle;
    other.handle = nullptr;

    return *this;
}

bool Object::operator==(const Object& other) const {
    if (handle == other.handle) {
        return true;
    }

    bool result = getEnv()->IsSameObject(handle, other.handle);
    handleJniException("equality");
    return result;
}

bool Object::operator==(nullptr_t) const {
    if (handle == nullptr) {
        return true;
    }

    bool result = getEnv()->IsSameObject(handle, nullptr);
    handleJniException("equality");
    return result;
}

Class Object::getClass() const {
    nullCheck();

    jclass result = getEnv()->GetObjectClass(handle);
    handleJniException("get class");
    return std::move(result);
}

bool Object::equals(const Object& other) const {
    return invoke<bool>("equals", java_cast<decltype("java/lang/Object"_class)>(other));
}

int32_t Object::hashCode() const {
    return invoke<int32_t>("hashCode");
}

std::string Object::toString() const {
    return invoke<std::string>("toString");
}

template <> void Object::invokeInternal<void>(jmethodID method, const value_t* args) const {
    nullCheck();
    getEnv()->CallVoidMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke");
}

template <> bool Object::invokeInternal<bool>(jmethodID method, const value_t* args) const {
    nullCheck();
    bool result = getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke");
    return result;
}

template <> uint8_t Object::invokeInternal<uint8_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    uint8_t result = getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke");
    return result;
}

template <> uint16_t Object::invokeInternal<uint16_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    uint16_t result = getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke");
    return result;
}

template <> int16_t Object::invokeInternal<int16_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    int16_t result = getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke");
    return result;
}

template <> int32_t Object::invokeInternal<int32_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    int32_t result = getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke");
    return result;
}

template <> int64_t Object::invokeInternal<int64_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    int64_t result = getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke");
    return result;
}

template <> float Object::invokeInternal<float>(jmethodID method, const value_t* args) const {
    nullCheck();
    float result = getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke");
    return result;
}

template <> double Object::invokeInternal<double>(jmethodID method, const value_t* args) const {
    nullCheck();
    double result = getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke");
    return result;
}

template <> std::string Object::invokeInternal<std::string>(jmethodID method, const value_t* args) const {
    Object temp = invokeInternal<Object>(method, args);
    handleJniException("invoke");
    return asString(static_cast<jstring>(temp.handle));
}

template <> Object Object::invokeInternal<Object>(jmethodID method, const value_t* args) const {
    nullCheck();
    jobject result = getEnv()->CallObjectMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke");
    return std::move(result);
}

Class::Class(const jclass& handle) : TypedObject(static_cast<jobject>(handle), 0) {
}

Class::Class(jclass&& handle) : TypedObject(std::forward<jobject>(handle), 0) {
}

Class Class::forName(const std::string_view& name) {
    jclass result = getEnv()->FindClass(NullTerminatedString(name));
    handleJniException("find class");
    return std::move(result);
}

bool Class::isAssignableFrom(const Class& subclass) const {
    nullCheck();
    subclass.nullCheck();
    bool result = getEnv()->IsAssignableFrom(static_cast<jclass>(subclass.handle), static_cast<jclass>(handle));
    handleJniException("is assignable");
    return result;
}

jmethodID Class::getMethod(const std::string_view& name, const std::string_view& signature) const {
    nullCheck();
    jmethodID result =
        getEnv()->GetMethodID(static_cast<jclass>(handle), NullTerminatedString(name), NullTerminatedString(signature));
    handleJniException("get method");
    return result;
}

jmethodID Class::getStaticMethod(const std::string_view& name, const std::string_view& signature) const {
    nullCheck();
    jmethodID result = getEnv()->GetStaticMethodID(static_cast<jclass>(handle), NullTerminatedString(name),
                                                   NullTerminatedString(signature));
    handleJniException("get static method");
    return result;
}

Object Class::newInstanceInternal(jmethodID method, const value_t* args) const {
    nullCheck();
    jobject result = getEnv()->NewObjectA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("new instance");
    return std::move(result);
}

template <> void Class::invokeStaticInternal<void>(jmethodID method, const value_t* args) const {
    nullCheck();
    getEnv()->CallStaticVoidMethodA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke static");
}

template <> bool Class::invokeStaticInternal<bool>(jmethodID method, const value_t* args) const {
    nullCheck();
    bool result =
        getEnv()->CallStaticBooleanMethodA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke static");
    return result;
}
template <> uint8_t Class::invokeStaticInternal<uint8_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    uint8_t result =
        getEnv()->CallStaticByteMethodA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke static");
    return result;
}
template <> uint16_t Class::invokeStaticInternal<uint16_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    uint16_t result =
        getEnv()->CallStaticCharMethodA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke static");
    return result;
}
template <> int16_t Class::invokeStaticInternal<int16_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    int16_t result =
        getEnv()->CallStaticShortMethodA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke static");
    return result;
}
template <> int32_t Class::invokeStaticInternal<int32_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    int32_t result =
        getEnv()->CallStaticIntMethodA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke static");
    return result;
}
template <> int64_t Class::invokeStaticInternal<int64_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    int64_t result =
        getEnv()->CallStaticLongMethodA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke static");
    return result;
}
template <> float Class::invokeStaticInternal<float>(jmethodID method, const value_t* args) const {
    nullCheck();
    float result =
        getEnv()->CallStaticFloatMethodA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke static");
    return result;
}
template <> double Class::invokeStaticInternal<double>(jmethodID method, const value_t* args) const {
    nullCheck();
    double result =
        getEnv()->CallStaticDoubleMethodA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke static");
    return result;
}
template <> std::string Class::invokeStaticInternal<std::string>(jmethodID method, const value_t* args) const {
    Object temp = invokeStaticInternal<Object>(method, args);
    handleJniException("invoke static");
    return asString(static_cast<jstring>(static_cast<jobject>(temp)));
}
template <> Object Class::invokeStaticInternal<Object>(jmethodID method, const value_t* args) const {
    nullCheck();
    jobject result =
        getEnv()->CallStaticObjectMethodA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
    handleJniException("invoke static");
    return std::move(result);
}

template <> void throwReturn(const Throwable& throwable) {
    throwable.nullCheck();
    getEnv()->Throw(static_cast<jthrowable>(static_cast<jobject>(throwable)));
}

static JNIEnv* jniEnv = nullptr;

void init(JNIEnv* env) {
    jniEnv = env;
}

JNIEnv* getEnv() {
    return jniEnv;
}

} // namespace jwrap
