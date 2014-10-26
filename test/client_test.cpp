#include "environment.h"
#include <webdriverxx/client.h>
#include <gtest/gtest.h>

namespace test {

using namespace webdriverxx;

class TestClient : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		client = new Client(GetWebDriverUrl());
	}

	static void TearDownTestCase() {
		delete client;
		client = 0;
	}

	static Client* client;
};

Client* TestClient::client = 0;

TEST_F(TestClient, GetsStatus) {
	picojson::object status = client->GetStatus();
	ASSERT_TRUE(status["build"].is<picojson::object>());
	ASSERT_TRUE(status["os"].is<picojson::object>());
}

TEST_F(TestClient, GetsSessions) {
	client->GetSessions();
}

TEST_F(TestClient, CreatesSession) {
 	Parameters params = GetParameters();
	client->CreateSession(params.desired, params.required);
}

} // namespace test
