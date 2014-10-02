#include "environment.h"
#include <webdriverxx/detail/http_connection.h>
#include <gtest/gtest.h>
#include <string>

using namespace webdriverxx;
using namespace webdriverxx::detail;

TEST(HttpConnection, CanBeCreated) {
	HttpConnection connection;
}

TEST(HttpConnection, GetsPage) {
	HttpConnection connection;
	HttpResponse response = connection.Get(std::string(Environment::Instance().GetUrl()) + "status");
	ASSERT_EQ(200, response.http_code);
	ASSERT_TRUE(!response.body.empty());
}

TEST(HttpConnection, ThrowsExceptionIfPortIsClosed) {
	HttpConnection connection;
	const char *const kUrlWithClosedPort = "http://127.0.0.1:7778/";
	ASSERT_THROW(connection.Get(kUrlWithClosedPort), WebDriverException);
}
