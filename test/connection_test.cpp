#include <webdriverxx/detail/connection.h>
#include <webdriverxx/detail/http_connection.h>
#include <rapidjson/document.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

typedef detail::Connection<rapidjson::Document> Connection;
using detail::HttpConnection;

TEST(Connection, CanBeCreated) {
	HttpConnection http_connection;
	Connection connection("http://127.0.0.1/", http_connection);
}

TEST(Connection, ThrowsExceptionWhenPortIsClosed) {
	HttpConnection http_connection;
	Connection connection("http://127.0.0.1:7778/", http_connection);
	ASSERT_THROW(connection.Get("xxx"), CurlException);
}

TEST(Connection, ThrowsExceptionIfCommandIsUnknown) {
	HttpConnection http_connection;
	Connection connection("http://127.0.0.1:7777/", http_connection);
	ASSERT_THROW(connection.Get("unknown_command"), InvalidRequestException);
}

// TODO: test for server failure
