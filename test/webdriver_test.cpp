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

TEST(WebDriver, CanGetSessions) {
	WebDriver driver(kPhantomUrl);
	driver.GetSessions();
}

TEST(WebDriver, RegistersSession) {
	WebDriver spy(kPhantomUrl);
	size_t number_of_sessions_before = spy.GetSessions().size();
	WebDriver testee(kPhantomUrl);
	size_t number_of_sessions_after = spy.GetSessions().size();
	ASSERT_EQ(number_of_sessions_before + 1, number_of_sessions_after);
}

TEST(WebDriver, UnregistersSessionOnScopeExit) {
	WebDriver spy(kPhantomUrl);
	size_t number_of_sessions_before = 0;
	{
		WebDriver testee(kPhantomUrl);
		number_of_sessions_before = spy.GetSessions().size();
	}
	size_t number_of_sessions_after = spy.GetSessions().size();
	ASSERT_EQ(number_of_sessions_before - 1, number_of_sessions_after);
}

TEST(WebDriver, CanGetCapabilities)
{
	WebDriver driver(kPhantomUrl);
	Capabilities c = driver.GetCapabilities();
	ASSERT_TRUE(c.Contains("browserName"));
	ASSERT_TRUE(c.Contains("version"));
	ASSERT_TRUE(c.Contains("platform"));
	ASSERT_NE("", c.GetString("browserName"));
}
