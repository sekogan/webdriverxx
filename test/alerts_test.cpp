#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

class TestAlerts : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		Environment::Instance().GetDriver().Navigate(
			Environment::Instance().GetTestPageUrl("alerts.html")
			);
	}

	TestAlerts() : driver(Environment::Instance().GetDriver()) {}

	WebDriver driver;
};

TEST_F(TestAlerts, AcceptsAlert) {
	if (driver.GetBrowser() == browser::Phantom) return;
	driver.Execute("alert('abc')");
	driver.AcceptAlert();
}

TEST_F(TestAlerts, DismissesAlert) {
	if (driver.GetBrowser() == browser::Phantom) return;
	driver.Execute("alert('abc')");
	driver.DismissAlert();
}

TEST_F(TestAlerts, GetsAlertText) {
	if (driver.GetBrowser() == browser::Phantom) return;
	driver.Execute("alert('abc')");
	ASSERT_EQ("abc", driver.GetAlertText());
	driver.DismissAlert();
}

TEST_F(TestAlerts, SendsKeysToAlert) {
	if (driver.GetBrowser() == browser::Phantom) return;
	driver.Execute("result = prompt('abc')");
	driver.SendKeysToAlert("def");
	driver.AcceptAlert();
	ASSERT_EQ("def", driver.Eval<std::string>("return result"));
}

TEST_F(TestAlerts, DismissesSendedKeys) {
	if (driver.GetBrowser() == browser::Phantom) return;
	driver.Execute("result = prompt('abc')");
	driver.SendKeysToAlert("def");
	driver.DismissAlert();
	ASSERT_FALSE(driver.Eval<bool>("return result"));
}
