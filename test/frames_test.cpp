#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

class TestFrames : public ::testing::Test {
protected:
	TestFrames()
		: driver(Environment::Instance().GetDriver())
		, url(Environment::Instance().GetTestPageUrl("frames.html"))
	{}

	void SetUp()
	{
		driver.Navigate(url);
	}

	WebDriver driver;
	std::string url;
};

TEST_F(TestFrames, OnTopFrameByDefault) {
	ASSERT_EQ("top_frame", driver.FindElement(ById("tag")).GetAttribute("value"));
}

TEST_F(TestFrames, CanSwitchToFrameByNumber) {
	driver.SetFocusToFrame(1);
	ASSERT_EQ("frame3", driver.FindElement(ById("tag")).GetAttribute("value"));
}

TEST_F(TestFrames, CanSwitchToFrameByName) {
	driver.SetFocusToFrame("frame3_name");
	ASSERT_EQ("frame3", driver.FindElement(ById("tag")).GetAttribute("value"));
}

TEST_F(TestFrames, CanSwitchToFrameByElement) {
	std::vector<Element> frames = driver.FindElements(ByTagName("iframe"));
	ASSERT_EQ(2u, frames.size());
	driver.SetFocusToFrame(frames[1]);
	ASSERT_EQ("frame3", driver.FindElement(ById("tag")).GetAttribute("value"));
}

TEST_F(TestFrames, CanSwitchToDefaultFrame) {
	driver.SetFocusToFrame(1);
	driver.SetFocusToDefaultFrame();
	ASSERT_EQ("top_frame", driver.FindElement(ById("tag")).GetAttribute("value"));
}

TEST_F(TestFrames, CanSwitchToDeepFrames) {
	driver.SetFocusToFrame(0).SetFocusToFrame(1);
	ASSERT_EQ("frame2", driver.FindElement(ById("tag")).GetAttribute("value"));
}

TEST_F(TestFrames, CanSwitchToParentFrame) {
	if (driver.GetBrowser() == browser::Phantom) return; // Not supported in PhantomJS 1.9.7
	driver.SetFocusToFrame(0).SetFocusToFrame(1)
		.SetFocusToParentFrame().SetFocusToParentFrame();
	ASSERT_EQ("top_frame", driver.FindElement(ById("tag")).GetAttribute("value"));
}
