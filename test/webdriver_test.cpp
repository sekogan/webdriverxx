#include "test_data.h"
#include "test_helpers.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

TEST(WebDriver, CanBeCreated) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
}

TEST(WebDriver, InstancesDoNotHaveConflicts) {
	WebDriver a(g_driver.url, g_driver.required, g_driver.desired);
	WebDriver b(g_driver.url, g_driver.required, g_driver.desired);
}

TEST(WebDriver, CanGetStatus) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	picojson::object status = driver.GetStatus();
	ASSERT_TRUE(status["build"].is<picojson::object>());
	ASSERT_TRUE(status["os"].is<picojson::object>());
}

TEST(WebDriver, CanGetSessions) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	driver.GetSessions();
}

TEST(WebDriver, RegistersSession) {
	WebDriver spy(g_driver.url, g_driver.required, g_driver.desired);
	size_t number_of_sessions_before = spy.GetSessions().size();
	WebDriver testee(g_driver.url, g_driver.required, g_driver.desired);
	size_t number_of_sessions_after = spy.GetSessions().size();
	ASSERT_EQ(number_of_sessions_before + 1, number_of_sessions_after);
}

TEST(WebDriver, UnregistersSessionOnScopeExit) {
	WebDriver spy(g_driver.url, g_driver.required, g_driver.desired);
	size_t number_of_sessions_before = 0;
	{
		WebDriver testee(g_driver.url, g_driver.required, g_driver.desired);
		number_of_sessions_before = spy.GetSessions().size();
	}
	size_t number_of_sessions_after = spy.GetSessions().size();
	ASSERT_EQ(number_of_sessions_before - 1, number_of_sessions_after);
}

TEST(WebDriver, CanGetCapabilities)
{
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	Capabilities c = driver.GetCapabilities();
	ASSERT_TRUE(c.Contains("browserName"));
	ASSERT_TRUE(c.Contains("version"));
	ASSERT_TRUE(c.Contains("platform"));
	ASSERT_NE("", c.GetString("browserName"));
}

TEST(WebDriver, CanGetCurrentWindow) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	driver.GetCurrentWindow();
}

TEST(WebDriver, CanGetWindowHandle) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	ASSERT_NE("", driver.GetCurrentWindow().GetHandle());
}

TEST(WebDriver, CanCloseCurrentWindow) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	driver.CloseCurrentWindow();
}

TEST(WebDriver, CanSetFocusToWindow) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	driver.SetFocusToWindow(driver.GetCurrentWindow().GetHandle());
}

TEST(WebDriver, CanGetWindowSize) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	Window window = driver.GetCurrentWindow();
	window.GetSize();
}

TEST(WebDriver, CanSetWindowSize) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	Window window = driver.GetCurrentWindow();
	Size size1;
	size1.width = 601;
	size1.height = 602;
	window.SetSize(size1);
	Size size2 = window.GetSize();
	ASSERT_EQ(601, size2.width);
	ASSERT_EQ(602, size2.height);
}

TEST(WebDriver, CanGetWindowPosition) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	Window window = driver.GetCurrentWindow();
	window.GetPosition();
}

TEST(WebDriver, CanSetWindowPosition) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	Window window = driver.GetCurrentWindow();
	Position position1;
	position1.x = 101;
	position1.y = 102;
	window.SetPosition(position1);
	if (driver.GetCapabilities().GetString("browserName") != "phantomjs")
	{
		Position position2 = window.GetPosition();
		ASSERT_EQ(101, position2.x);
		ASSERT_EQ(102, position2.y);
	}
}

TEST(WebDriver, CanMaximizeWindow) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	Window window = driver.GetCurrentWindow();
	window.Maximize();
}

TEST(WebDriver, CanGetWindows) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	driver.GetWindows();
}

TEST(WebDriver, CanNavigate) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	std::string url = std::string(g_driver.url) + "status";
	driver.Navigate(url);
	ASSERT_EQ(url, driver.GetUrl());
}

TEST(WebDriver, CanNavigateToTestPage) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	driver.Navigate(GetUrl("simple.html"));
	ASSERT_EQ(GetUrl("simple.html"), driver.GetUrl());
}
