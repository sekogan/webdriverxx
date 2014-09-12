include(ExternalProject)

ExternalProject_Add(googlemock
	PREFIX "${CMAKE_BINARY_DIR}/gtest-1.7.0"
	URL "http://googlemock.googlecode.com/files/gmock-1.7.0.zip"
	URL_HASH SHA1=f9d9dd882a25f4069ed9ee48e70aff1b53e3c5a5
	CMAKE_ARGS -Dgtest_force_shared_crt=ON
	INSTALL_COMMAND "")
externalproject_get_property(googlemock source_dir binary_dir)

include_directories("${source_dir}/include")
include_directories("${source_dir}/gtest/include")
link_directories("${binary_dir}")

function(AddTest test_name)
    add_executable(${test_name} "${test_name}.cpp")
    add_dependencies(${test_name} googlemock)
    target_link_libraries(${test_name} gmock gmock_main)
    add_test(${test_name} ${test_name})
endfunction(AddTest)

AddTest(skeleton_test)
