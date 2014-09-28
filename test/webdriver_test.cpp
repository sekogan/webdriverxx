#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

class TestWebDriver : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		server = new Server(Environment::Instance().GetUrl());
	}

	static void TearDownTestCase() {
		delete server;
		server = 0;
	}

	void SetUp() {
		params = Environment::Instance().GetParameters();
	}

	static Server* server;
	Parameters params;
};

Server* TestWebDriver::server = 0;

TEST_F(TestWebDriver, CreatesSession) {
	size_t number_of_sessions_before = server->GetSessions().size();
	WebDriver testee(params.url, params.required, params.desired);
	size_t number_of_sessions_after = server->GetSessions().size();
	ASSERT_EQ(number_of_sessions_before + 1, number_of_sessions_after);
}

TEST_F(TestWebDriver, DeletesSessionOnDestruction) {
	size_t number_of_sessions_before = 0;
	{
		WebDriver testee(params.url, params.required, params.desired);
		number_of_sessions_before = server->GetSessions().size();
	}
	size_t number_of_sessions_after = server->GetSessions().size();
	ASSERT_EQ(number_of_sessions_before - 1, number_of_sessions_after);
}
