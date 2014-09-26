#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <webdriverxx/keys.h>
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
	TestWebDriver() : driver(0) {}

	void SetUp() {
		driver = Environment::Instance().GetDriver();
	}

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
	const std::string url = Environment::Instance().GetTestPageUrl("simple.html");
	driver->Navigate(url);
	ASSERT_EQ(url, driver->GetUrl());
}

class WebDriverOnElementsPage : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		driver = Environment::Instance().GetDriver();
		driver->Navigate(Environment::Instance().GetTestPageUrl("elements.html"));
	}

	void SetUp() {
		ASSERT_TRUE(!!driver);
	}

	static WebDriver* driver;
};

WebDriver* WebDriverOnElementsPage::driver = 0;

TEST_F(WebDriverOnElementsPage, GetsPageSource) {
	std::string source = driver->GetSource();
	ASSERT_NE(std::string::npos, source.find("<html"));
	ASSERT_NE(std::string::npos, source.find("</html>"));
}

TEST_F(WebDriverOnElementsPage, GetsPageTitle) {
	ASSERT_EQ("Test title", driver->GetTitle());
}

TEST_F(WebDriverOnElementsPage, FindsElementById) {
	driver->FindElement(ById("id1"));
}

TEST_F(WebDriverOnElementsPage, DoesNotFindNonExistingElementById) {
	ASSERT_THROW(driver->FindElement(ById("non existing id")), WebDriverException);
}

TEST_F(WebDriverOnElementsPage, FindsElementByClassName) {
	driver->FindElement(ByClassName("black"));
}

TEST_F(WebDriverOnElementsPage, FindsElementByCssSelector) {
	driver->FindElement(ByCssSelector("body div#id1"));
}

TEST_F(WebDriverOnElementsPage, FindsElementByName) {
	driver->FindElement(ByName("john"));
}

TEST_F(WebDriverOnElementsPage, FindsElementByLinkText) {
	driver->FindElement(ByLinkText("some link"));
}

TEST_F(WebDriverOnElementsPage, FindsElementByPartialLinkText) {
	driver->FindElement(ByPartialLinkText("some"));
}

TEST_F(WebDriverOnElementsPage, FindsElementByTagName) {
	driver->FindElement(ByTagName("body"));
}

TEST_F(WebDriverOnElementsPage, FindsElementByXPath) {
	driver->FindElement(ByXPath("//input"));
}

TEST_F(WebDriverOnElementsPage, FindsElementsById) {
	ASSERT_EQ(1u, driver->FindElements(ById("id1")).size());
}

TEST_F(WebDriverOnElementsPage, ReturnsEmptyListIfElementsAreNotFound) {
	ASSERT_EQ(0u, driver->FindElements(ById("non existing id")).size());
}

TEST_F(WebDriverOnElementsPage, FindsMoreThanOneElement) {
	ASSERT_TRUE(1u < driver->FindElements(ByTagName("div")).size());
}

TEST_F(WebDriverOnElementsPage, FindsInnerElementByTagName) {
	Element e1 = driver->FindElement(ByTagName("div"));
	Element e2 = e1.FindElement(ByTagName("div"));
	ASSERT_TRUE(e1 != e2);
}

TEST_F(WebDriverOnElementsPage, FindsOnlyInnerElements) {
	Element e = driver->FindElement(ByTagName("div"));
	ASSERT_EQ(1u, e.FindElements(ByTagName("div")).size());
}

TEST_F(WebDriverOnElementsPage, DoesNotFindNonExistingInnerElements) {
	Element e = driver->FindElement(ByTagName("div"));
	ASSERT_THROW(e.FindElement(ByTagName("p")), WebDriverException);
	ASSERT_EQ(0u, e.FindElements(ByTagName("p")).size());
}

TEST_F(WebDriverOnElementsPage, FindsMoreThanOneInnerElement) {
	ASSERT_EQ(2u, driver->FindElement(ByTagName("div")).FindElements(ByTagName("span")).size());
}

TEST_F(WebDriverOnElementsPage, ClicksElement) {
	driver->FindElement(ByName("john")).Click();
}

// TODO: Element::Submit

TEST_F(WebDriverOnElementsPage, GetsElementText) {
	ASSERT_EQ("first span", driver->FindElement(ByTagName("span")).GetText());
}

TEST_F(WebDriverOnElementsPage, ClearsElement) {
	Element e = driver->FindElement(ByName("john"));
	e.SendKeys("abc");
	ASSERT_NE("", e.GetAttribute("value"));
	e.Clear();
	ASSERT_EQ("", e.GetAttribute("value"));
}

TEST_F(WebDriverOnElementsPage, SendsKeysToElement) {
	Element e = driver->FindElement(ByName("john"));
	e.Clear().SendKeys("abc").SendKeys(keys::Home).SendKeys("def");
	ASSERT_EQ("defabc", e.GetAttribute("value"));
}

TEST_F(WebDriverOnElementsPage, SendsKeysToActiveElement) {
	Element john = driver->FindElement(ByName("john"));
	Element peter = driver->FindElement(ByName("peter"));
	john.Click().Clear();
	driver->SendKeys("abc");
	peter.Click().Clear();
	driver->SendKeys("def");
	ASSERT_EQ("abc", john.GetAttribute("value"));
	ASSERT_EQ("def", peter.GetAttribute("value"));
}

TEST_F(WebDriverOnElementsPage, GetsElementTagName) {
	ASSERT_EQ("div", driver->FindElement(ById("id1")).GetTagName());
}

// IsEnabled
// IsSelected

TEST_F(WebDriverOnElementsPage, GetsElementAttributes) {
	ASSERT_EQ("id1", driver->FindElement(ByTagName("div")).GetAttribute("id"));
}

TEST_F(WebDriverOnElementsPage, DeterminesThatElementsAreEqual) {
	ASSERT_TRUE(driver->FindElement(ByTagName("div")).Equals(
		driver->FindElement(ById("id1"))));
}

TEST_F(WebDriverOnElementsPage, DeterminesThatElementsAreNotEqual) {
	ASSERT_TRUE(!driver->FindElement(ByTagName("span")).Equals(
		driver->FindElement(ById("id1"))));
}

TEST_F(WebDriverOnElementsPage, ComparesElementsWithEqualOperator) {
	ASSERT_TRUE(driver->FindElement(ByTagName("div")) ==
		driver->FindElement(ById("id1")));
}

TEST_F(WebDriverOnElementsPage, ComparesElementsWithNotEqualOperator) {
	ASSERT_TRUE(driver->FindElement(ByTagName("span")) !=
		driver->FindElement(ById("id1")));
}

TEST_F(WebDriverOnElementsPage, GetsElementIsDisplayed) {
	ASSERT_TRUE(driver->FindElement(ById("id1")).IsDisplayed());
	ASSERT_FALSE(driver->FindElement(ById("hidden")).IsDisplayed());
}

TEST_F(WebDriverOnElementsPage, GetsElementLocation) {
	driver->FindElement(ById("id1")).GetLocation();
	driver->FindElement(ById("id1")).GetLocationInView();
}

TEST_F(WebDriverOnElementsPage, GetsElementSize) {
	Size size = driver->FindElement(ById("id1")).GetSize();
	ASSERT_NE(0, size.width);
	ASSERT_NE(0, size.height);
}

TEST_F(WebDriverOnElementsPage, GetsElementCssProperty) {
	ASSERT_EQ("none", driver->FindElement(ById("hidden")).GetCssProperty("display"));
}
