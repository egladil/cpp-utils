
set(HEADER_FILES
    include/fnv1a.h
	include/murmur.h
)

add_library(hash INTERFACE)
target_include_directories(hash INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

add_custom_target(hash_ SOURCES ${HEADER_FILES})
