#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

class TestWebDriver : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		client = new Client(Environment::Instance().GetUrl());
	}

	static void TearDownTestCase() {
		delete client;
		client = 0;
	}

	void SetUp() {
		params = Environment::Instance().GetParameters();
	}

	static Client* client;
	Parameters params;
};

Client* TestWebDriver::client = 0;

TEST_F(TestWebDriver, CreatesSession) {
	size_t number_of_sessions_before = client->GetSessions().size();
	WebDriver testee(params.desired, params.required, params.url);
	size_t number_of_sessions_after = client->GetSessions().size();
	ASSERT_EQ(number_of_sessions_before + 1, number_of_sessions_after);
}

TEST_F(TestWebDriver, DeletesSessionOnDestruction) {
	size_t number_of_sessions_before = 0;
	{
		WebDriver testee(params.desired, params.required, params.url);
		number_of_sessions_before = client->GetSessions().size();
	}
	size_t number_of_sessions_after = client->GetSessions().size();
	ASSERT_EQ(number_of_sessions_before - 1, number_of_sessions_after);
}
