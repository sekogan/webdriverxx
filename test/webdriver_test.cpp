#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

const char* const kPhantomUrl = "http://localhost:7777/";

using namespace webdriverxx;

TEST(WebDriver, CanBeCreated) {
	WebDriver driver;
}

TEST(WebDriver, CanGetStatus) {
	WebDriver driver(kPhantomUrl);
	auto status = driver.GetStatus();
	ASSERT_TRUE(status.IsObject());
	ASSERT_TRUE(status.HasMember("build") && status["build"].IsObject());
	ASSERT_TRUE(status.HasMember("os") && status["os"].IsObject());
}
