#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

class TestElement : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		Environment::Instance().GetDriver()->Navigate(
			Environment::Instance().GetTestPageUrl("element.html")
			);
	}

	TestElement() : driver(Environment::Instance().GetDriver()) {}

	WebDriver* driver;
};

TEST_F(TestElement, CanBeClicked) {
	driver->FindElement(ByTagName("input")).Click();
}

// TODO: Element::Submit

TEST_F(TestElement, GetsText) {
	ASSERT_EQ("Some text", driver->FindElement(ById("element_with_text")).GetText());
}

TEST_F(TestElement, CanBeCleared) {
	Element e = driver->FindElement(ByTagName("input"));
	e.SendKeys("abc");
	ASSERT_NE("", e.GetAttribute("value"));
	e.Clear();
	ASSERT_EQ("", e.GetAttribute("value"));
}

TEST_F(TestElement, GetsTagName) {
	ASSERT_EQ("div", driver->FindElement(ByTagName("div")).GetTagName());
}

// IsEnabled
// IsSelected

TEST_F(TestElement, GetsAttributes) {
	Element e = driver->FindElement(ById("div_with_attributes"));
	ASSERT_EQ("div_with_attributes", e.GetAttribute("id"));
	ASSERT_EQ("test value", e.GetAttribute("test"));
}

TEST_F(TestElement, IsEqualToOtherElement) {
	Element e = driver->FindElement(ById("first_div"));
	Element other = driver->FindElement(ById("first_div"));
	ASSERT_TRUE(e.Equals(other));
	ASSERT_TRUE(e == other);
}

TEST_F(TestElement, IsNotEqualToOtherElement) {
	Element e = driver->FindElement(ById("first_div"));
	Element other = driver->FindElement(ById("second_div"));
	ASSERT_TRUE(!e.Equals(other));
	ASSERT_TRUE(e != other);
}

TEST_F(TestElement, GetsIsDisplayed) {
	ASSERT_TRUE(driver->FindElement(ById("visible")).IsDisplayed());
	ASSERT_FALSE(driver->FindElement(ById("hidden")).IsDisplayed());
}

TEST_F(TestElement, GetsLocation) {
	driver->FindElement(ById("visible")).GetLocation();
	driver->FindElement(ById("visible")).GetLocationInView();
}

TEST_F(TestElement, GetsSize) {
	Size size = driver->FindElement(ById("visible")).GetSize();
	ASSERT_NE(0, size.width);
	ASSERT_NE(0, size.height);
}

TEST_F(TestElement, GetsCssProperty) {
	ASSERT_EQ("none", driver->FindElement(ById("hidden")).GetCssProperty("display"));
}
