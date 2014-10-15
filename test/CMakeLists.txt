set(SOURCE_FILES
	alerts_test.cpp
	capabilities_test.cpp
	client_test.cpp
	element_test.cpp
	environment.h
	examples_test.cpp
	finder_test.cpp
	frames_test.cpp
	http_connection_test.cpp
	js_test.cpp
	keyboard_test.cpp
	main.cpp
	resource_test.cpp
	session_test.cpp
	shared_test.cpp
	to_string_test.cpp
	wait_match_test.cpp
	wait_test.cpp
	webdriver_test.cpp
	)

file(COPY pages DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DWEBDRIVERXX_ENABLE_GMOCK_MATCHERS)

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif()

# Coverage
if (UNIX)
	set(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} Coverage)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo" "Coverage")
	set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
	set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_DEBUG} --coverage")

	find_program(LCOV_EXECUTABLE lcov)
	find_program(GENHTML_EXECUTABLE genhtml)

	add_custom_target(coverage
		${LCOV_EXECUTABLE} --directory=. --zerocounters
		COMMAND ${CMAKE_CTEST_COMMAND} -V
		COMMAND ${LCOV_EXECUTABLE} --capture --directory=. --output-file coverage.info
		COMMAND ${GENHTML_EXECUTABLE} coverage.info --output-directory coverage
		DEPENDS ${PROJECT_NAME}
		)
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
if (CMAKE_COMPILER_IS_GNUCXX)
set(GMOCK_VERSION "1.6.0") # To avoid bug in GCC 4.6
else()
set(GMOCK_VERSION "1.7.0")
endif()
externalproject_add(gmock_project
	PREFIX gmock
	URL "http://googlemock.googlecode.com/files/gmock-${GMOCK_VERSION}.zip"
	CMAKE_ARGS -Dgtest_force_shared_crt=ON
	INSTALL_COMMAND ""
	UPDATE_COMMAND ""
	)
externalproject_get_property(gmock_project source_dir binary_dir)
include_directories("${source_dir}/include" "${source_dir}/gtest/include")
link_directories("${binary_dir}" "${binary_dir}/gtest")
list(APPEND LIBS gmock gtest)
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
