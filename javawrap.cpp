#include "javawrap.h"

#include <jni.h>

namespace {
std::string asString(jstring string) {
    const char* temp = jwrap::getEnv()->GetStringUTFChars(string, nullptr);
    std::string result = temp;
    jwrap::getEnv()->ReleaseStringUTFChars(string, temp);

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
    val.l = getEnv()->NewStringUTF(value.data());
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
    }
}
} // namespace detail

Object::Object(const jobject& handle) {
    this->handle = getEnv()->NewLocalRef(handle);
}

Object::Object(jobject&& handle) : handle(std::move(handle)) {
    handle = nullptr;
}

Object::Object(const Object& object) {
    this->handle = getEnv()->NewLocalRef(object.handle);
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

Object::Object(const std::string& string) {
    handle = getEnv()->NewStringUTF(string.data());
}

Object::Object(const std::string_view& string) {
    handle = getEnv()->NewStringUTF(string.data());
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

    return getEnv()->NewLocalRef(handle);
}

Object Object::newLocalRef() const {
    return getEnv()->NewLocalRef(handle);
}

Object Object::newGlobalRef() const {
    return getEnv()->NewGlobalRef(handle);
}

Object Object::newWeakGlobalRef() const {
    return getEnv()->NewWeakGlobalRef(handle);
}

Object& Object::operator=(const Object& other) {
    free();

    handle = getEnv()->NewLocalRef(other.handle);

    return *this;
}

Object& Object::operator=(Object&& other) {
    free();

    handle = other.handle;
    other.handle = nullptr;

    return *this;
}

bool Object::operator==(const Object& other) const {
    return handle == other.handle || getEnv()->IsSameObject(handle, other.handle);
}

bool Object::operator==(nullptr_t) const {
    return handle == nullptr || getEnv()->IsSameObject(handle, nullptr);
}

Class Object::getClass() const {
    nullCheck();

    return getEnv()->GetObjectClass(handle);
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
}

template <> bool Object::invokeInternal<bool>(jmethodID method, const value_t* args) const {
    nullCheck();
    return getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
}

template <> uint8_t Object::invokeInternal<uint8_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    return getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
}

template <> uint16_t Object::invokeInternal<uint16_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    return getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
}

template <> int16_t Object::invokeInternal<int16_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    return getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
}

template <> int32_t Object::invokeInternal<int32_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    return getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
}

template <> int64_t Object::invokeInternal<int64_t>(jmethodID method, const value_t* args) const {
    nullCheck();
    return getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
}

template <> float Object::invokeInternal<float>(jmethodID method, const value_t* args) const {
    nullCheck();
    return getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
}

template <> double Object::invokeInternal<double>(jmethodID method, const value_t* args) const {
    nullCheck();
    return getEnv()->CallBooleanMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
}

template <> std::string Object::invokeInternal<std::string>(jmethodID method, const value_t* args) const {
    Object temp = invokeInternal<Object>(method, args);
    return asString(static_cast<jstring>(temp.handle));
}

template <> Object Object::invokeInternal<Object>(jmethodID method, const value_t* args) const {
    nullCheck();
    return getEnv()->CallObjectMethodA(handle, method, reinterpret_cast<const jvalue*>(args));
}

Class::Class(const jclass& handle) : TypedObject(static_cast<jobject>(handle), 0) {
}

Class::Class(jclass&& handle) : TypedObject(std::forward<jobject>(handle), 0) {
}

Class Class::forName(const std::string_view& name) {
    return getEnv()->FindClass(name.data());
}

bool Class::isAssignableFrom(const Class& subclass) const {
    nullCheck();
    subclass.nullCheck();
    return getEnv()->IsAssignableFrom(static_cast<jclass>(subclass.handle), static_cast<jclass>(handle));
}

std::string Class::getName() const {
    return invoke<std::string>("getName");
}

jmethodID Class::getMethod(const std::string_view& name, const std::string_view& signature) const {
    nullCheck();
    return getEnv()->GetMethodID(static_cast<jclass>(handle), name.data(), signature.data());
}

Object Class::newInstanceInternal(jmethodID method, const value_t* args) const {
    nullCheck();
    return getEnv()->NewObjectA(static_cast<jclass>(handle), method, reinterpret_cast<const jvalue*>(args));
}

static JNIEnv* jniEnv = nullptr;

void init(JNIEnv* env) {
    jniEnv = env;
}

JNIEnv* getEnv() {
    return jniEnv;
}

} // namespace jwrap
