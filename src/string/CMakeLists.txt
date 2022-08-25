
set(HEADER_FILES
	include/inline_string.h
    include/string_util.h
)

add_library("string" INTERFACE)
target_include_directories("string" INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

add_custom_target(string_ SOURCES ${HEADER_FILES})
