#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <webdriverxx/wait_match.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

class TestExamples : public ::testing::Test {
protected:
	TestExamples() : driver(Environment::Instance().GetDriver()) {}

	void StopNavigation() { // Workaround for Firefox unable to navigate right after Submit()
		WaitForMatch([this] {
			return driver.Navigate(Environment::Instance().GetTestPageUrl("non_existing.html")).GetUrl();
		}, ::testing::HasSubstr("non_existing"));
	}

	WebDriver driver;
};

TEST_F(TestExamples, QuickExample)
{
	driver
		.Navigate("http://google.com")
		.FindElement(ByXPath(".//input[@name='q']"))
		.SendKeys("Hello, world!")
		.Submit();

	StopNavigation();
}
