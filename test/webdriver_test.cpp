#include "test_data.h"
#include "test_helpers.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

TEST(BasicWebDriver, CanBeCreated) {
	BasicWebDriver driver(g_driver.url);
}

TEST(BasicWebDriver, CanGetStatus) {
	BasicWebDriver driver(g_driver.url);
	picojson::object status = driver.GetStatus();
	ASSERT_TRUE(status["build"].is<picojson::object>());
	ASSERT_TRUE(status["os"].is<picojson::object>());
}

TEST(BasicWebDriver, CanGetSessions) {
	BasicWebDriver driver(g_driver.url);
	driver.GetSessions();
}

TEST(WebDriver, RegistersSession) {
	BasicWebDriver spy(g_driver.url);
	size_t number_of_sessions_before = spy.GetSessions().size();
	WebDriver testee(g_driver.url, g_driver.required, g_driver.desired);
	size_t number_of_sessions_after = spy.GetSessions().size();
	ASSERT_EQ(number_of_sessions_before + 1, number_of_sessions_after);
}

TEST(WebDriver, UnregistersSessionOnScopeExit) {
	BasicWebDriver spy(g_driver.url);
	size_t number_of_sessions_before = 0;
	{
		WebDriver testee(g_driver.url, g_driver.required, g_driver.desired);
		number_of_sessions_before = spy.GetSessions().size();
	}
	size_t number_of_sessions_after = spy.GetSessions().size();
	ASSERT_EQ(number_of_sessions_before - 1, number_of_sessions_after);
}

TEST(WebDriver, CanCloseCurrentWindow) {
	WebDriver driver(g_driver.url, g_driver.required, g_driver.desired);
	driver.CloseCurrentWindow();
}

class SharedWebDriver : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		driver = new WebDriver(g_driver.url, g_driver.required, g_driver.desired);
	}

	static void TearDownTestCase() {
		delete driver;
		driver = 0;
	}

	static WebDriver* driver;
};

WebDriver* SharedWebDriver::driver = 0;

TEST_F(SharedWebDriver, CanGetCapabilities)
{
	Capabilities c = driver->GetCapabilities();
	ASSERT_TRUE(c.Contains("browserName"));
	ASSERT_TRUE(c.Contains("version"));
	ASSERT_TRUE(c.Contains("platform"));
	ASSERT_NE("", c.GetString("browserName"));
}

TEST_F(SharedWebDriver, CanStartSecondBrowser) {
	WebDriver second(g_driver.url, g_driver.required, g_driver.desired);
}

TEST_F(SharedWebDriver, CanGetCurrentWindow) {
	driver->GetCurrentWindow();
}

TEST_F(SharedWebDriver, CanGetWindowHandle) {
	ASSERT_NE("", driver->GetCurrentWindow().GetHandle());
}

TEST_F(SharedWebDriver, CanSetFocusToWindow) {
	driver->SetFocusToWindow(driver->GetCurrentWindow().GetHandle());
}

TEST_F(SharedWebDriver, CanGetWindowSize) {
	Window window = driver->GetCurrentWindow();
	window.GetSize();
}

TEST_F(SharedWebDriver, CanSetWindowSize) {
	Window window = driver->GetCurrentWindow();
	Size size1;
	size1.width = 601;
	size1.height = 602;
	window.SetSize(size1);
	Size size2 = window.GetSize();
	ASSERT_EQ(601, size2.width);
	ASSERT_EQ(602, size2.height);
}

TEST_F(SharedWebDriver, CanGetWindowPosition) {
	Window window = driver->GetCurrentWindow();
	window.GetPosition();
}

TEST_F(SharedWebDriver, CanSetWindowPosition) {
	Window window = driver->GetCurrentWindow();
	Position position1;
	position1.x = 101;
	position1.y = 102;
	window.SetPosition(position1);
	if (driver->GetCapabilities().GetString("browserName") != "phantomjs")
	{
		Position position2 = window.GetPosition();
		ASSERT_EQ(101, position2.x);
		ASSERT_EQ(102, position2.y);
	}
}

TEST_F(SharedWebDriver, CanMaximizeWindow) {
	Window window = driver->GetCurrentWindow();
	window.Maximize();
}

TEST_F(SharedWebDriver, CanGetWindows) {
	driver->GetWindows();
}

TEST_F(SharedWebDriver, CanNavigate) {
	std::string url = std::string(g_driver.url) + "status";
	driver->Navigate(url);
	ASSERT_EQ(url, driver->GetUrl());
}

TEST_F(SharedWebDriver, CanNavigateToTestPage) {
	driver->Navigate(GetUrl("simple.html"));
	ASSERT_EQ(GetUrl("simple.html"), driver->GetUrl());
}
