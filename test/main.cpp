#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

Environment* Environment::instance_ = 0;

bool IsArgument(const std::string& arg, const char* name) {
	return arg.find(std::string("--") + name) == 0;
}

std::string GetArgumentValue(const std::string& arg) {
	const size_t pos = arg.find('=');
	return pos == std::string::npos ? std::string() : arg.substr(pos + 1);
}

DriverParameters ParseDriverParameters(int argc, char **argv) {
	DriverParameters result;
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
	return result;
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(
		new Environment(ParseDriverParameters(argc, argv))
		);
	return RUN_ALL_TESTS();
}
