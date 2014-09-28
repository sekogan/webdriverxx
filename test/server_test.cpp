#include "environment.h"
#include <webdriverxx/server.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

class TestServer : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		server = new Server(Environment::Instance().GetUrl());
	}

	static void TearDownTestCase() {
		delete server;
		server = 0;
	}

	static Server* server;
};

Server* TestServer::server = 0;

TEST_F(TestServer, GetsStatus) {
	picojson::object status = server->GetStatus();
	ASSERT_TRUE(status["build"].is<picojson::object>());
	ASSERT_TRUE(status["os"].is<picojson::object>());
}

TEST_F(TestServer, GetsSessions) {
	server->GetSessions();
}

TEST_F(TestServer, CreatesSession) {
 	Parameters params = Environment::Instance().GetParameters();
	server->CreateSession(params.required, params.desired).DeleteSession();
}
