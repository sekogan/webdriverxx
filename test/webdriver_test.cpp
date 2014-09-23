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

	void SetUp() {
		ASSERT_TRUE(driver);
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

TEST_F(SharedWebDriver, CanFindElementById) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ById("id1"));
}

TEST_F(SharedWebDriver, CannotFindNonExistingElementById) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_THROW(driver->FindElement(ById("non existing id")), WebDriverException);
}

TEST_F(SharedWebDriver, CanFindElementByClassName) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByClassName("black"));
}

TEST_F(SharedWebDriver, CanFindElementByCssSelector) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByCssSelector("body div#id1"));
}

TEST_F(SharedWebDriver, CanFindElementByName) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByName("john"));
}

TEST_F(SharedWebDriver, CanFindElementByLinkText) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByLinkText("some link"));
}

TEST_F(SharedWebDriver, CanFindElementByPartialLinkText) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByPartialLinkText("some"));
}

TEST_F(SharedWebDriver, CanFindElementByTagName) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByTagName("body"));
}

TEST_F(SharedWebDriver, CanFindElementByXPath) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByXPath("//input"));
}

TEST_F(SharedWebDriver, CanFindElementsById) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ(1, driver->FindElements(ById("id1")).size());
}

TEST_F(SharedWebDriver, ReturnsEmptyListIfElementsAreNotFound) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ(0, driver->FindElements(ById("non existing id")).size());
}

TEST_F(SharedWebDriver, CanFindMoreThanOneElement) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ(3, driver->FindElements(ByTagName("div")).size());
}

TEST_F(SharedWebDriver, CanFindNestedElementByTagName) {
	driver->Navigate(GetUrl("elements.html"));
	Element e1 = driver->FindElement(ByTagName("div"));
	Element e2 = e1.FindElement(ByTagName("div"));
	ASSERT_TRUE(e1 != e2);
}

TEST_F(SharedWebDriver, FindsOnlyNestedElements) {
	driver->Navigate(GetUrl("elements.html"));
	Element e = driver->FindElement(ByTagName("div"));
	ASSERT_EQ(1, e.FindElements(ByTagName("div")).size());
}

TEST_F(SharedWebDriver, CannotFindNonExistingNestedElement) {
	driver->Navigate(GetUrl("elements.html"));
	Element e = driver->FindElement(ByTagName("div"));
	ASSERT_THROW(e.FindElement(ByTagName("p")), WebDriverException);
	ASSERT_EQ(0, e.FindElements(ByTagName("p")).size());
}

TEST_F(SharedWebDriver, CanFindMoreThanOneNestedElement) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ(2, driver->FindElement(ByTagName("div")).FindElements(ByTagName("span")).size());
}
