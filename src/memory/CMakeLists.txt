
set(HEADER_FILES
    include/tagged_ptr.h
)

add_library(memory INTERFACE)
target_include_directories(memory INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

add_custom_target(memory_ SOURCES ${HEADER_FILES})
