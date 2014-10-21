#include "environment.h"
#include <webdriverxx/browsers/chrome.h>
#include <webdriverxx/browsers/firefox.h>
#include <webdriverxx/browsers/ie.h>
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
	auto defaults = Capabilities().SetProxy(DirectConnection());
	auto ff = Start(Firefox(defaults));
}

TEST(Firefox, HasCapabilitiesProperties) {
	if (!TestRealBrowsers()) return;
	auto ff = Start(Firefox().SetProxy(DirectConnection()));
}

TEST(Firefox, ConvertsToJson) {
	auto ff = Firefox()
		.SetLogging(firefox::Logging().SetLevel(firefox::log_level::Warning))
		.SetFirefoxBinary("abc");
	const auto json = ToJson(ff);
	const auto c = FromJson<Capabilities>(json);
	const auto logging = c.Get<JsonObject>("loggingPrefs");
	ASSERT_EQ(browser::Firefox, c.GetBrowserName());
	ASSERT_EQ("WARNING", logging.Get<std::string>("driver"));
	ASSERT_EQ("abc", c.Get<std::string>("firefox_binary"));
}

TEST(InternetExplorer, ConvertsToJson) {
	auto ie = InternetExplorer();
	const auto json = ToJson(ie);
	const auto c = FromJson<Capabilities>(json);
	ASSERT_EQ(browser::InternetExplorer, c.GetBrowserName());
}

TEST(Chrome, ConvertsToJson) {
	auto gc = Chrome();
	const auto json = ToJson(gc);
	const auto c = FromJson<Capabilities>(json);
	ASSERT_EQ(browser::Chrome, c.GetBrowserName());
}

} // namespace test
