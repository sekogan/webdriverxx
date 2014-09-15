#include <webdriverxx/detail/http_connection.h>
#include <gtest/gtest.h>

using namespace webdriverxx;
using namespace webdriverxx::detail;

TEST(HttpConnection, CanBeCreated) {
	HttpConnection connection;
}

TEST(HttpConnection, ThrowsExceptionIfPortIsClosed) {
	HttpConnection connection;
	ASSERT_THROW(connection.Get("http://127.0.0.1:7778/xxx"), HttpException);
}
