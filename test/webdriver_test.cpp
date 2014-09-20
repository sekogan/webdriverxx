#include "common_test_data.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

TEST(WebDriver, CanBeCreated) {
	WebDriver driver(kPhantomUrl);
}

TEST(WebDriver, InstancesDoNotHaveConflicts) {
	WebDriver a(kPhantomUrl);
	WebDriver b(kPhantomUrl);
}

TEST(WebDriver, CanGetStatus) {
	WebDriver driver(kPhantomUrl);
	picojson::object status = driver.GetStatus();
	ASSERT_TRUE(status["build"].is<picojson::object>());
	ASSERT_TRUE(status["os"].is<picojson::object>());
}
