set(SOURCE_FILES
	alerts_test.cpp
	client_test.cpp
	element_test.cpp
	environment.h
	finder_test.cpp
	frames_test.cpp
	http_connection_test.cpp
	js_test.cpp
	keyboard_test.cpp
	main.cpp
	resource_test.cpp
	session_test.cpp
	shared_test.cpp
	webdriver_test.cpp
	)

file(COPY pages DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif()

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -g")
endif()

include(ExternalProject)

# CURL
if (WIN32)
	externalproject_add(curl_project
		URL http://curl.haxx.se/download/curl-7.29.0.tar.gz
		PREFIX curl
		CMAKE_ARGS -DCURL_STATICLIB=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_CURL_EXE=OFF -DBUILD_CURL_TESTS=OFF -DCMAKE_USE_OPENSSL=OFF -DCURL_ZLIB=OFF -DHTTP_ONLY=ON
		INSTALL_COMMAND ""
		UPDATE_COMMAND ""
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
	GIT_TAG 806c5740e2c673189fee7e53acc0429af0b24691
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
	UPDATE_COMMAND ""
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
	UPDATE_COMMAND ""
	)
externalproject_get_property(gmock_project source_dir binary_dir)
include_directories("${source_dir}/include")
include_directories("${source_dir}/gtest/include")
link_directories("${binary_dir}")
list(APPEND LIBS gmock)
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
