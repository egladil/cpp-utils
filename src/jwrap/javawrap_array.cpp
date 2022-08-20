#include "javawrap_array.h"

#include <jni.h>

namespace jwrap {

namespace detail {

template <> Object arrayCreate<bool>(const std::string_view&, size_t size) {
    return getEnv()->NewBooleanArray(static_cast<jsize>(size));
}

template <> Object arrayCreate<uint8_t>(const std::string_view&, size_t size) {
    return getEnv()->NewByteArray(static_cast<jsize>(size));
}

template <> Object arrayCreate<uint16_t>(const std::string_view&, size_t size) {
    return getEnv()->NewCharArray(static_cast<jsize>(size));
}

template <> Object arrayCreate<int16_t>(const std::string_view&, size_t size) {
    return getEnv()->NewShortArray(static_cast<jsize>(size));
}

template <> Object arrayCreate<int32_t>(const std::string_view&, size_t size) {
    return getEnv()->NewIntArray(static_cast<jsize>(size));
}

template <> Object arrayCreate<int64_t>(const std::string_view&, size_t size) {
    return getEnv()->NewLongArray(static_cast<jsize>(size));
}

template <> Object arrayCreate<float>(const std::string_view&, size_t size) {
    return getEnv()->NewFloatArray(static_cast<jsize>(size));
}

template <> Object arrayCreate<double>(const std::string_view&, size_t size) {
    return getEnv()->NewDoubleArray(static_cast<jsize>(size));
}

template <> Object arrayCreate<Object>(const std::string_view& name, size_t size) {
    jclass clazz = getEnv()->FindClass(name.data());
    return getEnv()->NewObjectArray(static_cast<jsize>(size), clazz, nullptr);
}

template <> bool arrayGet<bool>(const Object& array, size_t index) {
    array.nullCheck();

    jboolean result;
    getEnv()->GetBooleanArrayRegion(static_cast<jbooleanArray>(static_cast<jobject>(array)), (jsize)index, 1, &result);
    return result;
}

template <> uint8_t arrayGet<uint8_t>(const Object& array, size_t index) {
    array.nullCheck();

    jbyte result;
    getEnv()->GetByteArrayRegion(static_cast<jbyteArray>(static_cast<jobject>(array)), (jsize)index, 1, &result);
    return result;
}

template <> uint16_t arrayGet<uint16_t>(const Object& array, size_t index) {
    array.nullCheck();

    uint16_t result;
    getEnv()->GetCharArrayRegion(static_cast<jcharArray>(static_cast<jobject>(array)), (jsize)index, 1, &result);
    return result;
}

template <> int16_t arrayGet<int16_t>(const Object& array, size_t index) {
    array.nullCheck();

    int16_t result;
    getEnv()->GetShortArrayRegion(static_cast<jshortArray>(static_cast<jobject>(array)), (jsize)index, 1, &result);
    return result;
}

template <> int32_t arrayGet<int32_t>(const Object& array, size_t index) {
    array.nullCheck();

    jint result;
    getEnv()->GetIntArrayRegion(static_cast<jintArray>(static_cast<jobject>(array)), (jsize)index, 1, &result);
    return result;
}

template <> int64_t arrayGet<int64_t>(const Object& array, size_t index) {
    array.nullCheck();

    int64_t result;
    getEnv()->GetLongArrayRegion(static_cast<jlongArray>(static_cast<jobject>(array)), (jsize)index, 1, &result);
    return result;
}

template <> float arrayGet<float>(const Object& array, size_t index) {
    array.nullCheck();

    float result;
    getEnv()->GetFloatArrayRegion(static_cast<jfloatArray>(static_cast<jobject>(array)), (jsize)index, 1, &result);
    return result;
}

template <> double arrayGet<double>(const Object& array, size_t index) {
    array.nullCheck();

    double result;
    getEnv()->GetDoubleArrayRegion(static_cast<jdoubleArray>(static_cast<jobject>(array)), (jsize)index, 1, &result);
    return result;
}

template <> std::string arrayGet<std::string>(const Object& array, size_t index) {
    array.nullCheck();

    Object string = arrayGet<Object>(array, index);

    const char* chars = getEnv()->GetStringUTFChars(static_cast<jstring>(static_cast<jobject>(string)), nullptr);
    std::string result = chars;
    getEnv()->ReleaseStringUTFChars(static_cast<jstring>(static_cast<jobject>(string)), chars);

    return result;
}

template <> Object arrayGet<Object>(const Object& array, size_t index) {
    array.nullCheck();
    return getEnv()->GetObjectArrayElement(static_cast<jobjectArray>(static_cast<jobject>(array)), (jsize)index);
}

template <> void arraySet<bool>(const Object& array, size_t index, const bool& value) {
    array.nullCheck();

    jboolean temp = value;
    getEnv()->SetBooleanArrayRegion(static_cast<jbooleanArray>(static_cast<jobject>(array)), (jsize)index, 1, &temp);
}

template <> void arraySet<uint8_t>(const Object& array, size_t index, const uint8_t& value) {
    array.nullCheck();

    jbyte temp = value;
    getEnv()->SetByteArrayRegion(static_cast<jbyteArray>(static_cast<jobject>(array)), (jsize)index, 1, &temp);
}

template <> void arraySet<uint16_t>(const Object& array, size_t index, const uint16_t& value) {
    array.nullCheck();
    getEnv()->SetCharArrayRegion(static_cast<jcharArray>(static_cast<jobject>(array)), (jsize)index, 1, &value);
}

template <> void arraySet<int16_t>(const Object& array, size_t index, const int16_t& value) {
    array.nullCheck();
    getEnv()->SetShortArrayRegion(static_cast<jshortArray>(static_cast<jobject>(array)), (jsize)index, 1, &value);
}

template <> void arraySet<int32_t>(const Object& array, size_t index, const int32_t& value) {
    array.nullCheck();

    jint temp = value;
    getEnv()->SetIntArrayRegion(static_cast<jintArray>(static_cast<jobject>(array)), (jsize)index, 1, &temp);
}

template <> void arraySet<int64_t>(const Object& array, size_t index, const int64_t& value) {
    array.nullCheck();
    getEnv()->SetLongArrayRegion(static_cast<jlongArray>(static_cast<jobject>(array)), (jsize)index, 1, &value);
}

template <> void arraySet<float>(const Object& array, size_t index, const float& value) {
    array.nullCheck();
    getEnv()->SetFloatArrayRegion(static_cast<jfloatArray>(static_cast<jobject>(array)), (jsize)index, 1, &value);
}

template <> void arraySet<double>(const Object& array, size_t index, const double& value) {
    array.nullCheck();
    getEnv()->SetDoubleArrayRegion(static_cast<jdoubleArray>(static_cast<jobject>(array)), (jsize)index, 1, &value);
}

template <> void arraySet<std::string>(const Object& array, size_t index, const std::string& value) {
    arraySet<Object>(array, index, Object(getEnv()->NewStringUTF(value.data())));
}

template <> void arraySet<Object>(const Object& array, size_t index, const Object& value) {
    array.nullCheck();
    getEnv()->SetObjectArrayElement(static_cast<jobjectArray>(static_cast<jobject>(array)), (jsize)index,
                                    static_cast<jobject>(value));
}

template <> void arrayInitialize<bool>(const Object& array, const bool* data, size_t size) {
    array.nullCheck();

    auto target = getEnv()->GetBooleanArrayElements(static_cast<jbooleanArray>(static_cast<jobject>(array)), nullptr);
    std::copy(data, data + size, target);
    getEnv()->ReleaseBooleanArrayElements(static_cast<jbooleanArray>(static_cast<jobject>(array)), target, 0);
}

template <> void arrayInitialize<uint8_t>(const Object& array, const uint8_t* data, size_t size) {
    array.nullCheck();

    auto target = getEnv()->GetByteArrayElements(static_cast<jbyteArray>(static_cast<jobject>(array)), nullptr);
    std::copy(data, data + size, target);
    getEnv()->ReleaseByteArrayElements(static_cast<jbyteArray>(static_cast<jobject>(array)), target, 0);
}

template <> void arrayInitialize<uint16_t>(const Object& array, const uint16_t* data, size_t size) {
    array.nullCheck();

    auto target = getEnv()->GetCharArrayElements(static_cast<jcharArray>(static_cast<jobject>(array)), nullptr);
    std::copy(data, data + size, target);
    getEnv()->ReleaseCharArrayElements(static_cast<jcharArray>(static_cast<jobject>(array)), target, 0);
}

template <> void arrayInitialize<int16_t>(const Object& array, const int16_t* data, size_t size) {
    array.nullCheck();

    auto target = getEnv()->GetShortArrayElements(static_cast<jshortArray>(static_cast<jobject>(array)), nullptr);
    std::copy(data, data + size, target);
    getEnv()->ReleaseShortArrayElements(static_cast<jshortArray>(static_cast<jobject>(array)), target, 0);
}

template <> void arrayInitialize<int32_t>(const Object& array, const int32_t* data, size_t size) {
    array.nullCheck();

    auto target = getEnv()->GetIntArrayElements(static_cast<jintArray>(static_cast<jobject>(array)), nullptr);
    std::copy(data, data + size, target);
    getEnv()->ReleaseIntArrayElements(static_cast<jintArray>(static_cast<jobject>(array)), target, 0);
}

template <> void arrayInitialize<int64_t>(const Object& array, const int64_t* data, size_t size) {
    array.nullCheck();

    auto target = getEnv()->GetLongArrayElements(static_cast<jlongArray>(static_cast<jobject>(array)), nullptr);
    std::copy(data, data + size, target);
    getEnv()->ReleaseLongArrayElements(static_cast<jlongArray>(static_cast<jobject>(array)), target, 0);
}

template <> void arrayInitialize<float>(const Object& array, const float* data, size_t size) {
    array.nullCheck();

    auto target = getEnv()->GetFloatArrayElements(static_cast<jfloatArray>(static_cast<jobject>(array)), nullptr);
    std::copy(data, data + size, target);
    getEnv()->ReleaseFloatArrayElements(static_cast<jfloatArray>(static_cast<jobject>(array)), target, 0);
}

template <> void arrayInitialize<double>(const Object& array, const double* data, size_t size) {
    array.nullCheck();

    auto target = getEnv()->GetDoubleArrayElements(static_cast<jdoubleArray>(static_cast<jobject>(array)), nullptr);
    std::copy(data, data + size, target);
    getEnv()->ReleaseDoubleArrayElements(static_cast<jdoubleArray>(static_cast<jobject>(array)), target, 0);
}
} // namespace detail

void TypedArray<bool>::fill(const value_type& value) {
    nullCheck();

    auto target = getEnv()->GetBooleanArrayElements(static_cast<jbooleanArray>(handle), nullptr);
    std::fill(target, target + _size, value);
    getEnv()->ReleaseBooleanArrayElements(static_cast<jbooleanArray>(handle), target, 0);
}

void TypedArray<uint8_t>::fill(const value_type& value) {
    nullCheck();

    auto target = getEnv()->GetByteArrayElements(static_cast<jbyteArray>(handle), nullptr);
    std::fill(target, target + _size, value);
    getEnv()->ReleaseByteArrayElements(static_cast<jbyteArray>(handle), target, 0);
}

void TypedArray<uint16_t>::fill(const value_type& value) {
    nullCheck();

    auto target = getEnv()->GetCharArrayElements(static_cast<jcharArray>(handle), nullptr);
    std::fill(target, target + _size, value);
    getEnv()->ReleaseCharArrayElements(static_cast<jcharArray>(handle), target, 0);
}

void TypedArray<int16_t>::fill(const value_type& value) {
    nullCheck();

    auto target = getEnv()->GetShortArrayElements(static_cast<jshortArray>(handle), nullptr);
    std::fill(target, target + _size, value);
    getEnv()->ReleaseShortArrayElements(static_cast<jshortArray>(handle), target, 0);
}

void TypedArray<int32_t>::fill(const value_type& value) {
    nullCheck();

    auto target = getEnv()->GetIntArrayElements(static_cast<jintArray>(handle), nullptr);
    std::fill(target, target + _size, value);
    getEnv()->ReleaseIntArrayElements(static_cast<jintArray>(handle), target, 0);
}

void TypedArray<int64_t>::fill(const value_type& value) {
    nullCheck();

    auto target = getEnv()->GetLongArrayElements(static_cast<jlongArray>(handle), nullptr);
    std::fill(target, target + _size, value);
    getEnv()->ReleaseLongArrayElements(static_cast<jlongArray>(handle), target, 0);
}

void TypedArray<float>::fill(const value_type& value) {
    nullCheck();

    auto target = getEnv()->GetFloatArrayElements(static_cast<jfloatArray>(handle), nullptr);
    std::fill(target, target + _size, value);
    getEnv()->ReleaseFloatArrayElements(static_cast<jfloatArray>(handle), target, 0);
}

void TypedArray<double>::fill(const value_type& value) {
    nullCheck();

    auto target = getEnv()->GetDoubleArrayElements(static_cast<jdoubleArray>(handle), nullptr);
    std::fill(target, target + _size, value);
    getEnv()->ReleaseDoubleArrayElements(static_cast<jdoubleArray>(handle), target, 0);
}

void TypedArray<std::string>::fill(const value_type& value) {
}
} // namespace jwrap
