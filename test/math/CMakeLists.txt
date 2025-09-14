
set(SOURCE_FILES
	int128.cpp
)

set(HEADER_FILES
)

add_executable(math_test ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(math_test
  Catch2::Catch2WithMain
  math
)

set_target_properties(math_test PROPERTIES FOLDER Tests)

include(CTest)
include(Catch)
catch_discover_tests(math_test)
