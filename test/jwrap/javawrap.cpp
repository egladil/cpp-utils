
#include "javawrap.h"
#include <catch2/catch_test_macros.hpp>

void initJVM() {
    static bool initialized = false;
    if (!initialized) {
        jwrap::createJavaVM({"-verbose:jni", "-Djava.compiler=NONE"});
        initialized = true;
    }
}

TEST_CASE("createJavaVM", "[JWrap]") {
    initJVM();

    REQUIRE(jwrap::getEnv() != nullptr);
}

TEST_CASE("Class::forName", "[JWrap]") {
    initJVM();

    auto clazz = jwrap::Class::forName("java/lang/Object");
    std::string name = clazz.getName();

    REQUIRE(name == "java.lang.Object");
}
