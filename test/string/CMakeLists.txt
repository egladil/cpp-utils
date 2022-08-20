
set(SOURCE_FILES
	inline_string.cpp
)

set(HEADER_FILES
)

add_executable(string_test ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(string_test
  Catch2::Catch2WithMain
  "string"
)

set_target_properties(string_test PROPERTIES FOLDER Tests)

include(CTest)
include(Catch)
catch_discover_tests(string_test)
