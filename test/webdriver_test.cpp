#include "common_test_data.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

TEST(WebDriver, CanBeCreated) {
	WebDriver driver;
}

TEST(WebDriver, CanGetStatus) {
	WebDriver driver(kPhantomUrl);
	const WebDriver::JsonValue& status = driver.GetStatus();
	ASSERT_TRUE(status.IsObject());
	ASSERT_TRUE(status.HasMember("build") && status["build"].IsObject());
	ASSERT_TRUE(status.HasMember("os") && status["os"].IsObject());
}
