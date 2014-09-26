#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>
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

Environment* Environment::instance_ = 0;

std::string ConcatPath(const std::string& a, const std::string& b) {
	const std::string delim("/\\");
	return a.empty() ? b : (
		b.empty() ? a : (
		(delim.find(a[a.length() - 1]) != std::string::npos ?
			a.substr(0, a.length() - 1) : a)
		+ "/"
		+ (delim.find(b[0]) != std::string::npos ? b.substr(1) : b)
		));
}

std::string GetCurrentDirectory() {
	char buffer[FILENAME_MAX];
	if (!PortableGetcwd(buffer, sizeof(buffer)))
		throw std::runtime_error("Cannot get current directory");
	return std::string(buffer);
}

bool IsArgument(const std::string& arg, const char* name) {
	return arg.find(std::string("--") + name) == 0;
}

std::string GetArgumentValue(const std::string& arg) {
	const size_t pos = arg.find('=');
	return pos == std::string::npos ? std::string() : arg.substr(pos + 1);
}

std::string GetPath(const std::string& path_name)
{
	size_t pos = path_name.find_last_of("\\/");
	std::string path = pos == std::string::npos ? std::string() :
		path_name.substr(0, pos + 1);
	return path[0] == '/' || (path.length() > 2 && path[1] == ':') ?
		path : ConcatPath(GetCurrentDirectory(), path);
}

Parameters ParseParameters(int argc, char **argv) {
	Parameters result;
	result.url = kPhantomUrl;
	for(int i = 1; i < argc; ++i) {
		const std::string arg = argv[i];
		if (IsArgument(arg, "browser")) {
			result.url = webdriverxx::kDefaultUrl;
			const std::string browser_name = GetArgumentValue(arg);
			result.required.Add("browserName", browser_name);
			result.desired.Add("browserName", browser_name);
		}
	}
	result.test_dir = GetPath(argv[0]);
	return result;
}

int main(int argc, char **argv) {
	std::cerr << argv[0] << "\n";
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(
		new Environment(ParseParameters(argc, argv))
		);
	return RUN_ALL_TESTS();
}
