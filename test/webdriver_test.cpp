#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

class TestBasicWebDriver : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		driver = new BasicWebDriver(Environment::Instance().GetUrl());
	}

	static void TearDownTestCase() {
		delete driver;
		driver = 0;
	}

	void SetUp() {
		ASSERT_TRUE(!!driver);
	}

	static BasicWebDriver* driver;
};

BasicWebDriver* TestBasicWebDriver::driver = 0;

TEST_F(TestBasicWebDriver, GetsStatus) {
	picojson::object status = driver->GetStatus();
	ASSERT_TRUE(status["build"].is<picojson::object>());
	ASSERT_TRUE(status["os"].is<picojson::object>());
}

TEST_F(TestBasicWebDriver, GetsSessions) {
	driver->GetSessions();
}

TEST(WebDriver, RegistersSession) {
	Parameters params = Environment::Instance().GetParameters();
	BasicWebDriver spy(params.url);
	size_t number_of_sessions_before = spy.GetSessions().size();
	WebDriver testee(params.url, params.required, params.desired);
	size_t number_of_sessions_after = spy.GetSessions().size();
	ASSERT_EQ(number_of_sessions_before + 1, number_of_sessions_after);
}

TEST(WebDriver, UnregistersSessionOnScopeExit) {
	Parameters params = Environment::Instance().GetParameters();
	BasicWebDriver spy(params.url);
	size_t number_of_sessions_before = 0;
	{
		WebDriver testee(params.url, params.required, params.desired);
		number_of_sessions_before = spy.GetSessions().size();
	}
	size_t number_of_sessions_after = spy.GetSessions().size();
	ASSERT_EQ(number_of_sessions_before - 1, number_of_sessions_after);
}

TEST(WebDriver, ClosesCurrentWindow) {
	WebDriver* driver = Environment::Instance().GetDriver();
	driver->CloseCurrentWindow();
	Environment::Instance().GetFreshDriver();
}

class TestWebDriver : public ::testing::Test {
protected:
	TestWebDriver() : driver(Environment::Instance().GetDriver()) {}

	WebDriver* driver;
};

TEST_F(TestWebDriver, GetsCapabilities)
{
	Capabilities c = driver->GetCapabilities();
	ASSERT_TRUE(c.Contains("browserName"));
	ASSERT_TRUE(c.Contains("version"));
	ASSERT_TRUE(c.Contains("platform"));
	ASSERT_NE("", c.GetString("browserName"));
}

TEST_F(TestWebDriver, StartsSecondBrowser) {
	Parameters params = Environment::Instance().GetParameters();
	WebDriver second(params.url, params.required, params.desired);
}

TEST_F(TestWebDriver, GetsCurrentWindow) {
	driver->GetCurrentWindow();
}

TEST_F(TestWebDriver, GetsWindowHandle) {
	ASSERT_NE("", driver->GetCurrentWindow().GetHandle());
}

TEST_F(TestWebDriver, SetsFocusToWindow) {
	driver->SetFocusToWindow(driver->GetCurrentWindow().GetHandle());
}

TEST_F(TestWebDriver, GetsWindowSize) {
	Window window = driver->GetCurrentWindow();
	window.GetSize();
}

TEST_F(TestWebDriver, SetsWindowSize) {
	Window window = driver->GetCurrentWindow();
	Size size1;
	size1.width = 601;
	size1.height = 602;
	window.SetSize(size1);
	Size size2 = window.GetSize();
	ASSERT_EQ(601, size2.width);
	ASSERT_EQ(602, size2.height);
}

TEST_F(TestWebDriver, GetsWindowPosition) {
	Window window = driver->GetCurrentWindow();
	window.GetPosition();
}

TEST_F(TestWebDriver, SetsWindowPosition) {
	Window window = driver->GetCurrentWindow();
	Point position1;
	position1.x = 101;
	position1.y = 102;
	window.SetPosition(position1);
	if (driver->GetCapabilities().GetString("browserName") != "phantomjs")
	{
		Point position2 = window.GetPosition();
		ASSERT_EQ(101, position2.x);
		ASSERT_EQ(102, position2.y);
	}
}

TEST_F(TestWebDriver, MaximizesWindow) {
	Window window = driver->GetCurrentWindow();
	window.Maximize();
}

TEST_F(TestWebDriver, GetsWindows) {
	driver->GetWindows();
}

TEST_F(TestWebDriver, Navigates) {
	std::string url = std::string(Environment::Instance().GetUrl()) + "status";
	driver->Navigate(url);
	ASSERT_EQ(url, driver->GetUrl());
}

TEST_F(TestWebDriver, NavigatesToTestPage) {
	const std::string url = Environment::Instance().GetTestPageUrl("webdriver.html");
	driver->Navigate(url);
	ASSERT_EQ(url, driver->GetUrl());
}

TEST_F(TestWebDriver, GoesBack) {
	const std::string page1 = Environment::Instance().GetTestPageUrl("navigation1.html");
	const std::string page2 = Environment::Instance().GetTestPageUrl("navigation2.html");
	driver->Navigate(page1).Navigate(page2).Back();
	ASSERT_EQ(page1, driver->GetUrl());
}

TEST_F(TestWebDriver, GoesForward) {
	const std::string page1 = Environment::Instance().GetTestPageUrl("navigation1.html");
	const std::string page2 = Environment::Instance().GetTestPageUrl("navigation2.html");
	driver->Navigate(page1).Navigate(page2).Back().Forward();
	ASSERT_EQ(page2, driver->GetUrl());
}

TEST_F(TestWebDriver, DoesRefresh) {
	const std::string page = Environment::Instance().GetTestPageUrl("navigation1.html");
	driver->Navigate(page).FindElement(ByTagName("input")).Click().SendKeys("abc");
	ASSERT_EQ("abc", driver->FindElement(ByTagName("input")).GetAttribute("value"));
	driver->Refresh();
	ASSERT_EQ(page, driver->GetUrl());
	ASSERT_EQ("", driver->FindElement(ByTagName("input")).GetAttribute("value"));
}

class WebDriverOnTestPage : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		Environment::Instance().GetDriver()->Navigate(
			Environment::Instance().GetTestPageUrl("webdriver.html")
			);
	}

	WebDriverOnTestPage() : driver(Environment::Instance().GetDriver()) {}

	WebDriver* driver;
};

TEST_F(WebDriverOnTestPage, GetsPageSource) {
	std::string source = driver->GetSource();
	ASSERT_NE(std::string::npos, source.find("<html"));
	ASSERT_NE(std::string::npos, source.find("</html>"));
}

TEST_F(WebDriverOnTestPage, GetsPageTitle) {
	ASSERT_EQ("Test title", driver->GetTitle());
}
