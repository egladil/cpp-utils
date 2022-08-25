
set(SOURCE_FILES
	javawrap.cpp
	javawrap_array.cpp
)

set(HEADER_FILES
	include/javawrap.h
	include/javawrap_array.h
	include/javawrap_collection.h
)

add_library(jwrap ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(jwrap PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

find_package(JNI REQUIRED)
target_include_directories(jwrap PRIVATE ${JNI_INCLUDE_DIRS})

target_link_libraries(jwrap "string")

set_target_properties(jwrap PROPERTIES COMPILE_FLAGS "-Wno-gnu-string-literal-operator-template")
