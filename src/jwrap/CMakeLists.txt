
set(SOURCE_FILES
	javawrap.cpp
	javawrap_array.cpp
)

set(HEADER_FILES
	javawrap.h
	javawrap_array.h
	javawrap_collection.h
)

add_library(jwrap ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(jwrap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(JNI REQUIRED)
target_include_directories(jwrap PRIVATE ${JNI_INCLUDE_DIRS})
