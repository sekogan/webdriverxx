#include "environment.h"
#include <webdriverxx/session.h>
#include <gtest/gtest.h>

namespace test {

using namespace webdriverxx;

class TestMouse : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		GetDriver().Navigate(GetTestPageUrl("mouse.html"));
	}

	TestMouse() : driver(GetDriver()) {}

	void SetUp() {
		target = driver.FindElement(ById("target"));
		click_type = driver.FindElement(ById("click_type")).Clear();
		updown_type = driver.FindElement(ById("updown_type")).Clear();
		updown_button = driver.FindElement(ById("updown_button")).Clear();
	}

	std::string GetValue(const Element& element) {
		return element.GetAttribute("value");
	}

	WebDriver driver;
	Element target;
	Element click_type;
	Element updown_type;
	Element updown_button;
};

TEST_F(TestMouse, SendsClick) {
	driver.MoveToCenterOf(target).Click();
	ASSERT_EQ("click", GetValue(click_type));
}

TEST_F(TestMouse, MovesPointerInsideTarget) {
	driver.MoveToCenterOf(target).MoveTo(Offset(-45,-45)).Click();
	ASSERT_EQ("click", GetValue(click_type));
}

TEST_F(TestMouse, MovesPointerOutsideTarget) {
	if (IsFirefox()) return;
	driver.MoveToCenterOf(target).MoveTo(Offset(55,55)).Click();
	driver.MoveToCenterOf(target).MoveTo(Offset(-55,55)).Click();
	driver.MoveToCenterOf(target).MoveTo(Offset(-55,-55)).Click();
	driver.MoveToCenterOf(target).MoveTo(Offset(55,-55)).Click();
	ASSERT_EQ("", GetValue(click_type));
}

TEST_F(TestMouse, MovesPointerToTopLeftCorner) {
	driver.MoveToTopLeftOf(target).Click();
	ASSERT_EQ("click", GetValue(click_type));
}

TEST_F(TestMouse, MovesPointerToTopLeftCornerWithOffset) {
	driver.MoveToTopLeftOf(target, Offset(95,95)).Click();
	ASSERT_EQ("click", GetValue(click_type));
}

TEST_F(TestMouse, MovesPointerToTopLeftCornerWithOffset2) {
	if (IsFirefox()) return;
	driver.MoveToTopLeftOf(target, Offset(-5,-5)).Click();
	driver.MoveToTopLeftOf(target, Offset(105,105)).Click();
	ASSERT_EQ("", GetValue(click_type));
}

TEST_F(TestMouse, SendsDoubleclicks) {
	driver.MoveToCenterOf(target).DoubleClick();
	ASSERT_EQ("dblclick", GetValue(click_type));
}

TEST_F(TestMouse, SendsButtonDown) {
	driver.MoveToCenterOf(target).ButtonDown();
	ASSERT_EQ("mousedown", GetValue(updown_type));
	driver.ButtonUp();
}

TEST_F(TestMouse, SendsButtonUp) {
	driver.MoveToCenterOf(target).ButtonDown().ButtonUp();
	ASSERT_EQ("mouseup", GetValue(updown_type));
}

TEST_F(TestMouse, SendsDifferentButtons) {
	if (IsFirefox()) return;
	driver.MoveToCenterOf(target).ButtonDown(mouse::LeftButton).ButtonUp(mouse::LeftButton);
	const auto left_button = GetValue(updown_button);
	driver.ButtonDown(mouse::RightButton).ButtonUp(mouse::RightButton);
	const auto right_button = GetValue(updown_button);
	driver.ButtonDown(mouse::MiddleButton).ButtonUp(mouse::MiddleButton);
	const auto middle_button = GetValue(updown_button);
	ASSERT_NE(left_button, right_button);
	ASSERT_NE(left_button, middle_button);
	ASSERT_NE(right_button, middle_button);	
}

} // namespace test
