
set(HEADER_FILES
	inline_string.h
    string_util.h
)

add_library("string" INTERFACE)
target_include_directories("string" INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(string_ SOURCES ${HEADER_FILES})
