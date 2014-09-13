set(SOURCE_FILES
	"skeleton_test.cpp"
	)

include(ExternalProject)

externalproject_add(googlemock
	PREFIX "gtest-1.7.0"
	URL "http://googlemock.googlecode.com/files/gmock-1.7.0.zip"
	CMAKE_ARGS -Dgtest_force_shared_crt=ON
	INSTALL_COMMAND ""
	)
externalproject_get_property(googlemock source_dir binary_dir)

include_directories("${source_dir}/include")
include_directories("${source_dir}/gtest/include")
link_directories("${binary_dir}")

if (UNIX)
	set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
	find_package(Threads REQUIRED)
	set(GTEST_PTHREAD ${CMAKE_THREAD_LIBS_INIT})
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} googlemock)
target_link_libraries(${PROJECT_NAME} ${GTEST_PTHREAD} gmock gmock_main)
add_test(${PROJECT_NAME} ${PROJECT_NAME})
