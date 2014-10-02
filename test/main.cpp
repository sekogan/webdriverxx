#include "environment.h"
#include <gtest/gtest.h>
#include <string>

Environment* Environment::instance_ = 0;

bool IsCommandLineArgument(const std::string& arg, const char* name) {
	return arg.find(std::string("--") + name) == 0;
}

std::string GetCommandLineArgumentValue(const std::string& arg) {
	const size_t pos = arg.find('=');
	return pos == std::string::npos ? std::string() : arg.substr(pos + 1);
}

Parameters ParseParameters(int argc, char **argv) {
	Parameters result;
	for(int i = 1; i < argc; ++i) {
		const std::string arg = argv[i];
		if (IsCommandLineArgument(arg, "browser")) {
			result.url = webdriverxx::kDefaultUrl;
			const std::string browser_name = GetCommandLineArgumentValue(arg);
			result.required.Add("browserName", browser_name);
			result.desired.Add("browserName", browser_name);
		} else if (IsCommandLineArgument(arg, "pages")) {
			result.pages_url = GetCommandLineArgumentValue(arg);
		} else if (IsCommandLineArgument(arg, "webdriver")) {
			result.url = GetCommandLineArgumentValue(arg);
		}
	}
	return result;
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(
		new Environment(ParseParameters(argc, argv))
		);
	return RUN_ALL_TESTS();
}
