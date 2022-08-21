
set(HEADER_FILES
    fnv1a.h
	murmur.h
)

add_library(hash INTERFACE)
target_include_directories(hash INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(hash_ SOURCES ${HEADER_FILES})
