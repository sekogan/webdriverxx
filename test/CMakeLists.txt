set(SOURCE_FILES
	http_connection_test.cpp
	resource_test.cpp
	webdriver_test.cpp
	test_helpers.h
	test_data.h
	)

file(COPY pages DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include(ExternalProject)

# CURL
if (WIN32)
	externalproject_add(curl_project
		URL http://curl.haxx.se/download/curl-7.29.0.tar.gz
		PREFIX curl
		CMAKE_ARGS -DCURL_STATICLIB=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_CURL_EXE=OFF -DBUILD_CURL_TESTS=OFF -DCMAKE_USE_OPENSSL=OFF -DCURL_ZLIB=OFF -DHTTP_ONLY=ON
		INSTALL_COMMAND ""
	)
	externalproject_get_property(curl_project source_dir binary_dir)
	include_directories("${source_dir}/include")
	link_directories("${binary_dir}/lib")
	list(APPEND LIBS libcurl wsock32 ws2_32)
	list(APPEND DEPS curl_project)
	add_definitions(-DCURL_STATICLIB)
else()
	find_package(CURL REQUIRED)
	include_directories(${CURL_INCLUDE_DIR})
	list(APPEND LIBS ${CURL_LIBRARIES})
endif()

# picojson
externalproject_add(picojson_project
	PREFIX picojson
	GIT_REPOSITORY https://github.com/kazuho/picojson
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
	)
externalproject_get_property(picojson_project source_dir)
include_directories("${source_dir}")
list(APPEND DEPS picojson_project)

# GMock
externalproject_add(gmock_project
	PREFIX gmock
	URL "http://googlemock.googlecode.com/files/gmock-1.7.0.zip"
	CMAKE_ARGS -Dgtest_force_shared_crt=ON
	INSTALL_COMMAND ""
	)
externalproject_get_property(gmock_project source_dir binary_dir)
include_directories("${source_dir}/include")
include_directories("${source_dir}/gtest/include")
link_directories("${binary_dir}")
list(APPEND LIBS gmock gmock_main)
list(APPEND DEPS gmock_project)

# pthread
if (UNIX)
	set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
	find_package(Threads REQUIRED)
	list(APPEND LIBS ${CMAKE_THREAD_LIBS_INIT})
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} ${DEPS})
target_link_libraries(${PROJECT_NAME} ${LIBS})
add_test(${PROJECT_NAME} ${PROJECT_NAME})
