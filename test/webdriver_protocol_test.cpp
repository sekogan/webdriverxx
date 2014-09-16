#include "common_test_data.h"
#include <webdriverxx/detail/webdriver_protocol.h>
#include <webdriverxx/detail/http_client.h>
#include <rapidjson/document.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace webdriverxx;
using namespace webdriverxx::detail;

typedef WebDriverProtocolBase<rapidjson::Document> WebDriverProtocol;

struct MockHttpClient : IHttpClient
{
	MOCK_METHOD1(Get, HttpResponse(const std::string& url));
	MOCK_METHOD1(Put, HttpResponse(const std::string& url));
	MOCK_METHOD1(Post, HttpResponse(const std::string& url));
	MOCK_METHOD1(Delete, HttpResponse(const std::string& url));
};

using namespace ::testing;

struct TestWebDriverProtocol : Test
{
	void SetUp()
	{
		ON_CALL(http_client, Get(_)).WillByDefault(ReturnPointee(&http_response));
		ON_CALL(http_client, Put(_)).WillByDefault(ReturnPointee(&http_response));
		ON_CALL(http_client, Post(_)).WillByDefault(ReturnPointee(&http_response));
		ON_CALL(http_client, Delete(_)).WillByDefault(ReturnPointee(&http_response));
		EXPECT_CALL(http_client, Get(_)).Times(AnyNumber());
		EXPECT_CALL(http_client, Put(_)).Times(AnyNumber());
		EXPECT_CALL(http_client, Post(_)).Times(AnyNumber());
		EXPECT_CALL(http_client, Delete(_)).Times(AnyNumber());
		http_response.http_code = 200;
		http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	}

	MockHttpClient http_client;
	HttpResponse http_response;
};

// Positive tests

TEST_F(TestWebDriverProtocol, CanBeCreated) {
	WebDriverProtocol protocol(kPhantomUrl, http_client);
}

TEST_F(TestWebDriverProtocol, ReturnsScalarValueFromPositiveResponse)
{
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	const WebDriverProtocol::JsonValue& value = protocol.Get("command");
	ASSERT_TRUE(value.IsInt());
	ASSERT_EQ(12345, value.GetInt());
}

TEST_F(TestWebDriverProtocol, ReturnsObjectValueFromPositiveResponse)
{
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":{\"member\":12345}}";
	const WebDriverProtocol::JsonValue& value = protocol.Get("command");
	ASSERT_TRUE(value.IsObject());
	ASSERT_TRUE(value.HasMember("member"));
	ASSERT_TRUE(value["member"].IsInt());
	ASSERT_EQ(12345, value["member"].GetInt());
}

TEST_F(TestWebDriverProtocol, CanHandleNullSessionId)
{
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	http_response.body = "{\"sessionId\":null,\"status\":0,\"value\":12345}";
	ASSERT_NO_THROW(protocol.Get("command"));
}

// Negative tests

TEST_F(TestWebDriverProtocol, DoesNotHideHttpExceptions)
{
	EXPECT_CALL(http_client, Get(_)).WillOnce(Throw(HttpException("")));
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), HttpException);
}
TEST_F(TestWebDriverProtocol, ThrowsInvalidRequestOnHttp404)
{
	http_response.http_code = 404;
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidRequestException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidRequestOnHttp400)
{
	http_response.http_code = 400;
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidRequestException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidRequestOnHttp499)
{
	http_response.http_code = 499;
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidRequestException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidRequestOnHttp501)
{
	http_response.http_code = 501;
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidRequestException);
}

TEST_F(TestWebDriverProtocol, ThrowsFailedCommandExceptionOnHttp500)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"message\":\"12345\"}}";
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), FailedCommandException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndMissingStatus)
{
	http_response.http_code = 500;
	http_response.body = "{\"value\":{\"message\":\"12345\"}}";
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndInvalidStatus)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":\"xxx\",\"value\":{\"message\":\"12345\"}}";
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndMissingValue)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12}";
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndInvalidValue)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":\"xxx\",\"value\":\"12345\"}";
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndMissingMessage)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"xxx\":\"12345\"}}";
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndInvalidMessage)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"message\":12345}}";
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp399)
{
	http_response.http_code = 399;
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp502)
{
	http_response.http_code = 502;
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnEmptyResponse)
{
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	http_response.body = "";
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnMalformedResponse)
{
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	http_response.body = "Blah blah blah";
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnMissingStatus)
{
	http_response.body = "{\"sessionId\":\"123\",\"value\":12345}";
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnInvalidStatus)
{
	http_response.body = "{\"sessionId\":\"123\",\"status\":\"5\",\"value\":12345}";
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnNonZeroStatus)
{
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	http_response.body = "{\"sessionId\":\"123\",\"status\":5,\"value\":12345}";
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnMissingValue)
{
	http_response.body = "{\"sessionId\":\"123\",\"status\":0}";
	WebDriverProtocol protocol(kPhantomUrl, http_client);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}


