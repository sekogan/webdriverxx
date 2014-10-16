#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <webdriverxx/keys.h>
#include <gtest/gtest.h>

namespace test {

using namespace webdriverxx;

class TestKeyboard : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		GetDriver().Navigate(GetTestPageUrl("keyboard.html"));
	}

	TestKeyboard() : driver(GetDriver()) {}

	WebDriver driver;
};

TEST_F(TestKeyboard, SendsKeysToElement) {
	Element e = driver.FindElement(ByName("first"));
	e.Clear()
		.SendKeys("abc")
		.SendKeys(keys::Left).SendKeys(keys::Left).SendKeys(keys::Left)
		.SendKeys("def")
		;
	ASSERT_EQ("defabc", e.GetAttribute("value"));
}

TEST_F(TestKeyboard, SendsShortcuts) {
	Element e = driver.FindElement(ByName("first"));
	e.Clear()
		.SendKeys(Shortcut() << keys::Shift << "a" << "bc")
		.SendKeys("def")
		;
	ASSERT_EQ("ABCdef", e.GetAttribute("value"));
}

TEST_F(TestKeyboard, SendsKeysToActiveElement) {
	Element first = driver.FindElement(ByName("first"));
	Element second = driver.FindElement(ByName("second"));
	first.Click().Clear();
	driver.SendKeys("abc");
	second.Click().Clear();
	driver.SendKeys("def");
	ASSERT_EQ("abc", first.GetAttribute("value"));
	ASSERT_EQ("def", second.GetAttribute("value"));
}

} // namespace test
