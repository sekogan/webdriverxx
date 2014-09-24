#include "test_data.h"
#include "test_helpers.h"
#include <webdriverxx/webdriver.h>
#include <webdriverxx/keys.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

TEST(BasicWebDriver, CanBeCreated) {
	BasicWebDriver driver(g_driver.url);
}

TEST(BasicWebDriver, GetsStatus) {
	BasicWebDriver driver(g_driver.url);
	picojson::object status = driver.GetStatus();
	ASSERT_TRUE(status["build"].is<picojson::object>());
	ASSERT_TRUE(status["os"].is<picojson::object>());
}

TEST(BasicWebDriver, GetsSessions) {
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

TEST(WebDriver, ClosesCurrentWindow) {
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
		ASSERT_TRUE(!!driver);
	}

	static WebDriver* driver;
};

WebDriver* SharedWebDriver::driver = 0;

TEST_F(SharedWebDriver, GetsCapabilities)
{
	Capabilities c = driver->GetCapabilities();
	ASSERT_TRUE(c.Contains("browserName"));
	ASSERT_TRUE(c.Contains("version"));
	ASSERT_TRUE(c.Contains("platform"));
	ASSERT_NE("", c.GetString("browserName"));
}

TEST_F(SharedWebDriver, StartsSecondBrowser) {
	WebDriver second(g_driver.url, g_driver.required, g_driver.desired);
}

TEST_F(SharedWebDriver, GetsCurrentWindow) {
	driver->GetCurrentWindow();
}

TEST_F(SharedWebDriver, GetsWindowHandle) {
	ASSERT_NE("", driver->GetCurrentWindow().GetHandle());
}

TEST_F(SharedWebDriver, SetsFocusToWindow) {
	driver->SetFocusToWindow(driver->GetCurrentWindow().GetHandle());
}

TEST_F(SharedWebDriver, GetsWindowSize) {
	Window window = driver->GetCurrentWindow();
	window.GetSize();
}

TEST_F(SharedWebDriver, SetsWindowSize) {
	Window window = driver->GetCurrentWindow();
	Size size1;
	size1.width = 601;
	size1.height = 602;
	window.SetSize(size1);
	Size size2 = window.GetSize();
	ASSERT_EQ(601, size2.width);
	ASSERT_EQ(602, size2.height);
}

TEST_F(SharedWebDriver, GetsWindowPosition) {
	Window window = driver->GetCurrentWindow();
	window.GetPosition();
}

TEST_F(SharedWebDriver, SetsWindowPosition) {
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

TEST_F(SharedWebDriver, MaximizesWindow) {
	Window window = driver->GetCurrentWindow();
	window.Maximize();
}

TEST_F(SharedWebDriver, GetsWindows) {
	driver->GetWindows();
}

TEST_F(SharedWebDriver, Navigates) {
	std::string url = std::string(g_driver.url) + "status";
	driver->Navigate(url);
	ASSERT_EQ(url, driver->GetUrl());
}

TEST_F(SharedWebDriver, NavigatesToTestPage) {
	driver->Navigate(GetUrl("simple.html"));
	ASSERT_EQ(GetUrl("simple.html"), driver->GetUrl());
}

TEST_F(SharedWebDriver, GetsPageSource) {
	driver->Navigate(GetUrl("elements.html"));
	std::string source = driver->GetSource();
	ASSERT_NE(std::string::npos, source.find("<html>"));
	ASSERT_NE(std::string::npos, source.find("</html>"));
}

TEST_F(SharedWebDriver, GetsPageTitle) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ("Test title", driver->GetTitle());
}

TEST_F(SharedWebDriver, FindsElementById) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ById("id1"));
}

TEST_F(SharedWebDriver, DoesNotFindNonExistingElementById) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_THROW(driver->FindElement(ById("non existing id")), WebDriverException);
}

TEST_F(SharedWebDriver, FindsElementByClassName) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByClassName("black"));
}

TEST_F(SharedWebDriver, FindsElementByCssSelector) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByCssSelector("body div#id1"));
}

TEST_F(SharedWebDriver, FindsElementByName) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByName("john"));
}

TEST_F(SharedWebDriver, FindsElementByLinkText) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByLinkText("some link"));
}

TEST_F(SharedWebDriver, FindsElementByPartialLinkText) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByPartialLinkText("some"));
}

TEST_F(SharedWebDriver, FindsElementByTagName) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByTagName("body"));
}

TEST_F(SharedWebDriver, FindsElementByXPath) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ByXPath("//input"));
}

TEST_F(SharedWebDriver, FindsElementsById) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ(1, driver->FindElements(ById("id1")).size());
}

TEST_F(SharedWebDriver, ReturnsEmptyListIfElementsAreNotFound) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ(0, driver->FindElements(ById("non existing id")).size());
}

TEST_F(SharedWebDriver, FindsMoreThanOneElement) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_TRUE(1 < driver->FindElements(ByTagName("div")).size());
}

TEST_F(SharedWebDriver, FindsInnerElementByTagName) {
	driver->Navigate(GetUrl("elements.html"));
	Element e1 = driver->FindElement(ByTagName("div"));
	Element e2 = e1.FindElement(ByTagName("div"));
	ASSERT_TRUE(e1 != e2);
}

TEST_F(SharedWebDriver, FindsOnlyInnerElements) {
	driver->Navigate(GetUrl("elements.html"));
	Element e = driver->FindElement(ByTagName("div"));
	ASSERT_EQ(1, e.FindElements(ByTagName("div")).size());
}

TEST_F(SharedWebDriver, DoesNotFindNonExistingInnerElements) {
	driver->Navigate(GetUrl("elements.html"));
	Element e = driver->FindElement(ByTagName("div"));
	ASSERT_THROW(e.FindElement(ByTagName("p")), WebDriverException);
	ASSERT_EQ(0, e.FindElements(ByTagName("p")).size());
}

TEST_F(SharedWebDriver, FindsMoreThanOneInnerElement) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ(2, driver->FindElement(ByTagName("div")).FindElements(ByTagName("span")).size());
}

TEST_F(SharedWebDriver, ClicksElement) {
	driver->Navigate(GetUrl("elements.html")).FindElement(ByName("john")).Click();
}

// TODO: Element::Submit

TEST_F(SharedWebDriver, GetsElementText) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ("first span", driver->FindElement(ByTagName("span")).GetText());
}

TEST_F(SharedWebDriver, SendsKeysToElement) {
	driver->Navigate(GetUrl("elements.html"));
	Element e = driver->FindElement(ByName("john"));
	e.SendKeys("abc").SendKeys(keys::Home).SendKeys("def");
	ASSERT_EQ("defabc", e.GetAttribute("value"));
}

TEST_F(SharedWebDriver, SendsKeysToActiveElement) {
	driver->Navigate(GetUrl("elements.html"));
	Element john = driver->FindElement(ByName("john"));
	Element peter = driver->FindElement(ByName("peter"));
	john.Click();
	driver->SendKeys("abc");
	peter.Click();
	driver->SendKeys("def");
	ASSERT_EQ("abc", john.GetAttribute("value"));
	ASSERT_EQ("def", peter.GetAttribute("value"));
}

TEST_F(SharedWebDriver, GetsElementTagName) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ("div", driver->FindElement(ById("id1")).GetTagName());
}

TEST_F(SharedWebDriver, ClearsElement) {
	driver->Navigate(GetUrl("elements.html"));
	Element e = driver->FindElement(ByName("john"));
	e.SendKeys("abc");
	e.Clear();
	ASSERT_EQ("", e.GetAttribute("value"));
}

// IsEnabled
// IsSelected

TEST_F(SharedWebDriver, GetsElementAttributes) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ("id1", driver->FindElement(ByTagName("div")).GetAttribute("id"));
}

TEST_F(SharedWebDriver, DeterminesThatElementsAreEqual) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_TRUE(driver->FindElement(ByTagName("div")).Equals(
		driver->FindElement(ById("id1"))));
}

TEST_F(SharedWebDriver, DeterminesThatElementsAreNotEqual) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_TRUE(!driver->FindElement(ByTagName("span")).Equals(
		driver->FindElement(ById("id1"))));
}

TEST_F(SharedWebDriver, ComparesElementsWithEqualOperator) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_TRUE(driver->FindElement(ByTagName("div")) ==
		driver->FindElement(ById("id1")));
}

TEST_F(SharedWebDriver, ComparesElementsWithNotEqualOperator) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_TRUE(driver->FindElement(ByTagName("span")) !=
		driver->FindElement(ById("id1")));
}

TEST_F(SharedWebDriver, GetsElementIsDisplayed) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_TRUE(driver->FindElement(ById("id1")).IsDisplayed());
	ASSERT_FALSE(driver->FindElement(ById("hidden")).IsDisplayed());
}

TEST_F(SharedWebDriver, GetsElementLocation) {
	driver->Navigate(GetUrl("elements.html"));
	driver->FindElement(ById("id1")).GetLocation();
	driver->FindElement(ById("id1")).GetLocationInView();
}

TEST_F(SharedWebDriver, GetsElementSize) {
	driver->Navigate(GetUrl("elements.html"));
	Size size = driver->FindElement(ById("id1")).GetSize();
	ASSERT_NE(0, size.width);
	ASSERT_NE(0, size.height);
}

TEST_F(SharedWebDriver, GetsElementCssProperty) {
	driver->Navigate(GetUrl("elements.html"));
	ASSERT_EQ("none", driver->FindElement(ById("hidden")).GetCssProperty("display"));
}
