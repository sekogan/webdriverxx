#include "common_test_data.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

TEST(WebDriver, CanBeCreated) {
	WebDriver driver(kPhantomUrl);
}

TEST(WebDriver, CanGetStatus) {
	WebDriver driver(kPhantomUrl);
	picojson::value status = driver.GetStatus();
	ASSERT_TRUE(status.is<picojson::object>());
	ASSERT_TRUE(status.contains("build") && status.get("build").is<picojson::object>());
	ASSERT_TRUE(status.contains("os") && status.get("os").is<picojson::object>());
}
