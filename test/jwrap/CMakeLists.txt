
set(SOURCE_FILES
	javawrap.cpp
)

set(HEADER_FILES
)

add_executable(jwrap_test ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(jwrap_test
  Catch2::Catch2WithMain
  "jwrap"
)

find_package(JNI REQUIRED)
target_include_directories(jwrap_test PRIVATE ${JNI_INCLUDE_DIRS})
target_link_libraries(jwrap_test ${JNI_LIBRARIES})

set_target_properties(jwrap_test PROPERTIES FOLDER Tests)

include(CTest)
include(Catch)
catch_discover_tests(jwrap_test)
