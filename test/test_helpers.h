#ifndef WEBDRIVERXX_TEST_HELPERS_H
#define WEBDRIVERXX_TEST_HELPERS_H

#include <string>
#include <stdexcept>
#include <stdio.h>
#ifdef WIN32
    #include <direct.h>
	#include <algorithm>
    #define PortableGetcwd _getcwd
#else
    #include <unistd.h>
    #define PortableGetcwd getcwd
#endif

namespace webdriverxx {

inline
std::string GetCurrentDirectory() {
	char buffer[FILENAME_MAX];
	if (!PortableGetcwd(buffer, sizeof(buffer)))
		throw std::runtime_error("Cannot get current directory");
	return std::string(buffer);
}

inline
std::string GetUrl(const std::string& test_page) {
#ifdef WIN32
	std::string path = std::string("/") + GetCurrentDirectory();
	std::replace(path.begin(), path.end(), '\\', '/');
#else
	std::string path = GetCurrentDirectory();
#endif
	return std::string("file://") + path + "/pages/" + test_page;
}

} // namespace webdriverxx

#endif
