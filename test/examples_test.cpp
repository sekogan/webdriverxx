#include "environment.h"
#include <webdriverxx/webdriver.h>
#ifndef WEBDRIVERXX_ENABLE_GMOCK_MATCHERS
#define WEBDRIVERXX_ENABLE_GMOCK_MATCHERS
#endif
#include <webdriverxx/wait_match.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

class TestExamples : public ::testing::Test {
protected:
	TestExamples() : driver(Environment::Instance().GetDriver()) {}

	void StopNavigation() {
		WaitForMatch([this] {
			return driver.Navigate(Environment::Instance().GetTestPageUrl("non_existing.html")).GetUrl();
		}, ::testing::HasSubstr("non_existing"));
	}

	WebDriver driver;
};

TEST_F(TestExamples, QuickExample) {
	driver
		.Navigate("http://google.com")
		.FindElement(ByCss("input[name=q]"))
		.SendKeys("Hello, world!")
		.Submit();

	StopNavigation(); // Firefox doesn't perform navigation right after Submit.
}

TEST_F(TestExamples, ImplicitWait) {
	driver.SetImplicitTimeoutMs(0);
	try {
		Element element = driver.FindElement(ByName("akela"));
		FAIL();
	} catch (const std::exception&) {}
}

TEST_F(TestExamples, ExplicitWait1) {
	auto find_element = [&]{ return driver.FindElement(ById("async_loaded")); };
	try {
		int timeout = 0;
		Element element = WaitForValue(find_element, timeout);
		FAIL();
	} catch (const std::exception&) {}
}

TEST_F(TestExamples, ExplicitWait2) {
	auto element_is_selected = [&]{
		return driver.FindElement(ById("async_loaded")).IsSelected();
	};
	try {
		int timeout = 0;
		WaitUntil(element_is_selected, timeout);
		FAIL();
	} catch (const std::exception&) {}
}

TEST_F(TestExamples, UseGmockMatchers) {
	driver.Navigate(Environment::Instance().GetTestPageUrl("redirect.html"));
	auto url = [&]{ return driver.GetUrl(); };
	using namespace ::testing;
	WaitForMatch(url, HasSubstr("target"));
	StopNavigation();	
}
