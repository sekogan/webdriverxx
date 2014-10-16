#include "environment.h"
#include <webdriverxx/browsers/firefox.h>
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

namespace test {

using namespace webdriverxx;

TEST(Firefox, WithTheSimplestSyntax) {
	if (!TestRealBrowsers()) return;
	auto ff = Start(Firefox());
}

TEST(Firefox, WithCustomUrl) {
	if (!TestRealBrowsers()) return;
	auto ff = Start(Firefox(), kDefaultWebDriverUrl);
}

TEST(Firefox, WithDefaultCapabilities) {
	if (!TestRealBrowsers()) return;
	auto ff = Start(Firefox(Capabilities().SetProxy(DirectConnection())));
}

TEST(Firefox, HasCapabilitiesPropertiesItself) {
	if (!TestRealBrowsers()) return;
	auto ff = Start(Firefox().SetProxy(DirectConnection()));
}

TEST(Firefox, ConvertsToJson) {
	auto ff = Firefox()
		.SetLogging(firefox::Logging().SetLevel(firefox::logging_level::Warning))
		.SetFirefoxBinary("abc");
	const auto json = ToJson(ff);
	const auto c = FromJson<Capabilities>(json);
	const auto logging = c.Get<JsonObject>("loggingPrefs");
	ASSERT_EQ("firefox", c.GetBrowserName());
	ASSERT_EQ("WARNING", logging.Get<std::string>("driver"));
	ASSERT_EQ("abc", c.Get<std::string>("firefox_binary"));
}

} // namespace test
