#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

namespace test {

using namespace webdriverxx;

class TestElement : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		GetDriver().Navigate(GetTestPageUrl("element.html"));
	}

	TestElement() : driver(GetDriver()) {}

	WebDriver driver;
};

TEST_F(TestElement, CanBeClicked) {
	driver.FindElement(ByTag("input")).Click();
}

// TODO: Submit

TEST_F(TestElement, GetsText) {
	ASSERT_EQ("Some text", driver.FindElement(ById("element_with_text")).GetText());
}

TEST_F(TestElement, CanBeCleared) {
	Element e = driver.FindElement(ByTag("input"));
	e.SendKeys("abc");
	ASSERT_NE("", e.GetAttribute("value"));
	e.Clear();
	ASSERT_EQ("", e.GetAttribute("value"));
}

TEST_F(TestElement, GetsTagName) {
	ASSERT_EQ("div", driver.FindElement(ByTag("div")).GetTagName());
}

// TODO: IsEnabled
// TODO: IsSelected

TEST_F(TestElement, GetsAttributes) {
	Element e = driver.FindElement(ById("div_with_attributes"));
	ASSERT_EQ("div_with_attributes", e.GetAttribute("id"));
	ASSERT_EQ("test value", e.GetAttribute("test"));
}

TEST_F(TestElement, IsEqualToOtherElement) {
	Element e = driver.FindElement(ById("first_div"));
	Element other = driver.FindElement(ById("first_div"));
	ASSERT_TRUE(e.Equals(other));
	ASSERT_TRUE(e == other);
}

TEST_F(TestElement, IsNotEqualToOtherElement) {
	Element e = driver.FindElement(ById("first_div"));
	Element other = driver.FindElement(ById("second_div"));
	ASSERT_TRUE(!e.Equals(other));
	ASSERT_TRUE(e != other);
}

TEST_F(TestElement, HasStrictWeakOrdering) {
	Element a = driver.FindElement(ById("first_div"));
	Element b = driver.FindElement(ById("second_div"));
	Element c = driver.FindElement(ById("third_div"));
	Element a2 = a;
	if (c < b) std::swap(b, c);
	if (b < a) std::swap(a, b);
	if (c < b) std::swap(b, c);
	ASSERT_FALSE(a < a2);
	ASSERT_FALSE(a2 < a);
	ASSERT_TRUE(a < b && !(b < a));
	ASSERT_TRUE(a < b && b < c && a < c);
}

TEST_F(TestElement, GetsIsDisplayed) {
	ASSERT_TRUE(driver.FindElement(ById("visible")).IsDisplayed());
	ASSERT_FALSE(driver.FindElement(ById("hidden")).IsDisplayed());
}

TEST_F(TestElement, GetsLocation) {
	driver.FindElement(ById("visible")).GetLocation();
	driver.FindElement(ById("visible")).GetLocationInView();
}

TEST_F(TestElement, GetsSize) {
	Size size = driver.FindElement(ById("visible")).GetSize();
	ASSERT_NE(0, size.width);
	ASSERT_NE(0, size.height);
}

TEST_F(TestElement, GetsCssProperty) {
	ASSERT_EQ("none", driver.FindElement(ById("hidden")).GetCssProperty("display"));
}

} // namespace test
