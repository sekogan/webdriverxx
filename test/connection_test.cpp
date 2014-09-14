#include <webdriverxx/detail/connection.h>
#include <rapidjson/document.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

typedef detail::Connection<rapidjson::Document> Connection;

TEST(Connection, CanBeCreated) {
	Connection connection("http://127.0.0.1/");
}

TEST(Connection, ThrowsExceptionWhenPortIsClosed) {
	Connection connection("http://127.0.0.1:7778/");
	ASSERT_THROW(connection.Get("xxx"), CurlException);
}

TEST(Connection, ThrowsExceptionIfCommandIsUnknown) {
	Connection connection("http://127.0.0.1:7777/");
	ASSERT_THROW(connection.Get("unknown_command"), InvalidHttpCodeException);
}
