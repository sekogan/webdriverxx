#include "test_data.h"
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
	HttpResponse response = connection.Get(std::string(kPhantomUrl) + "status");
	ASSERT_EQ(200, response.http_code);
	ASSERT_TRUE(!response.body.empty());
}

TEST(HttpConnection, ThrowsExceptionIfPortIsClosed) {
	HttpConnection connection;
	ASSERT_THROW(connection.Get("http://127.0.0.1:7778/"), WebDriverException);
}
