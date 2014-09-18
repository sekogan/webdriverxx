#include <webdriverxx/detail/webdriver_protocol.h>
#include <webdriverxx/detail/http_client.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace webdriverxx;
using namespace webdriverxx::detail;

const char *const kTestUrl = "http://test/";

struct MockHttpClient : IHttpClient
{
	MOCK_CONST_METHOD1(Get, HttpResponse(const std::string& url));
	MOCK_CONST_METHOD1(Put, HttpResponse(const std::string& url));
	MOCK_CONST_METHOD3(Post, HttpResponse(
		const std::string& url,
		const std::string& contentType,
		const std::string& postData
		));
	MOCK_CONST_METHOD1(Delete, HttpResponse(const std::string& url));
};

using namespace ::testing;

struct TestWebDriverProtocol : Test
{
	void SetUp()
	{
		http_response.http_code = 200;
		http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	
		ON_CALL(http_client, Get(_)).WillByDefault(ReturnPointee(&http_response));
		ON_CALL(http_client, Put(_)).WillByDefault(ReturnPointee(&http_response));
		ON_CALL(http_client, Post(_,_,_)).WillByDefault(ReturnPointee(&http_response));
		ON_CALL(http_client, Delete(_)).WillByDefault(ReturnPointee(&http_response));
		EXPECT_CALL(http_client, Get(_)).Times(AnyNumber());
		EXPECT_CALL(http_client, Put(_)).Times(AnyNumber());
		EXPECT_CALL(http_client, Post(_,_,_)).Times(AnyNumber());
		EXPECT_CALL(http_client, Delete(_)).Times(AnyNumber());
	}

	MockHttpClient http_client;
	HttpResponse http_response;
};

// Positive tests

TEST_F(TestWebDriverProtocol, CanBeCreated) {
	WebDriverProtocol protocol(&http_client, kTestUrl);
}

TEST_F(TestWebDriverProtocol, ReturnsUrl) {
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_EQ(kTestUrl, protocol.GetUrl());
}

TEST_F(TestWebDriverProtocol, IsCopyable) {
	WebDriverProtocol a(&http_client, "123");
	WebDriverProtocol b(&http_client, "456");
	a = b;
	ASSERT_EQ("456", a.GetUrl());
}

TEST_F(TestWebDriverProtocol, ReturnsJsonObject)
{
	WebDriverProtocol protocol(&http_client, kTestUrl);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	ASSERT_TRUE(protocol.Get("command").is<picojson::object>());
}

TEST_F(TestWebDriverProtocol, ReturnsSessionId)
{
	WebDriverProtocol protocol(&http_client, kTestUrl);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	ASSERT_TRUE(protocol.Get("command").contains("sessionId"));
	ASSERT_EQ("123", protocol.Get("command").get("sessionId").get<std::string>());
}

TEST_F(TestWebDriverProtocol, ReturnsNullSessionId)
{
	WebDriverProtocol protocol(&http_client, kTestUrl);
	http_response.body = "{\"sessionId\":null,\"status\":0,\"value\":12345}";
	ASSERT_TRUE(protocol.Get("command").contains("sessionId"));
	ASSERT_TRUE(protocol.Get("command").get("sessionId").is<picojson::null>());
}

TEST_F(TestWebDriverProtocol, ReturnsScalarValueFromPositiveResponse)
{
	WebDriverProtocol protocol(&http_client, kTestUrl);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	picojson::value value = protocol.Get("command").get("value");
	ASSERT_TRUE(value.is<double>());
	ASSERT_EQ(12345, value.get<double>());
}

TEST_F(TestWebDriverProtocol, ReturnsObjectValueFromPositiveResponse)
{
	WebDriverProtocol protocol(&http_client, kTestUrl);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":{\"member\":12345}}";
	picojson::value value = protocol.Get("command").get("value");
	ASSERT_TRUE(value.is<picojson::object>());
	ASSERT_TRUE(value.contains("member"));
	ASSERT_TRUE(value.get("member").is<double>());
	ASSERT_EQ(12345, value.get("member").get<double>());
}

// Negative tests

TEST_F(TestWebDriverProtocol, DoesNotHideHttpExceptions)
{
	EXPECT_CALL(http_client, Get(_)).WillOnce(Throw(HttpException("")));
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), HttpException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidRequestOnHttp404)
{
	http_response.http_code = 404;
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidRequestException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidRequestOnHttp400)
{
	http_response.http_code = 400;
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidRequestException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidRequestOnHttp499)
{
	http_response.http_code = 499;
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidRequestException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidRequestOnHttp501)
{
	http_response.http_code = 501;
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidRequestException);
}

TEST_F(TestWebDriverProtocol, InvalidRequestExceptionContainsHttpCodeAndBody)
{
	http_response.http_code = 501;
	http_response.body = "--12345--";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	try
	{
		protocol.Get("command");
		FAIL(); // Shouldn't get here
	}
	catch (const std::exception& e)
	{
		const std::string message = e.what();
		ASSERT_NE(std::string::npos, message.find("--12345--"));
		ASSERT_NE(std::string::npos, message.find("501"));
	}
}

TEST_F(TestWebDriverProtocol, ThrowsFailedCommandExceptionOnHttp500)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"message\":\"12345\"}}";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), FailedCommandException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndMissingStatus)
{
	http_response.http_code = 500;
	http_response.body = "{\"value\":{\"message\":\"12345\"}}";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndInvalidStatus)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":\"xxx\",\"value\":{\"message\":\"12345\"}}";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndMissingValue)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12}";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndInvalidValue)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":\"xxx\",\"value\":\"12345\"}";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndMissingMessage)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"xxx\":\"12345\"}}";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp500AndInvalidMessage)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"message\":12345}}";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp399)
{
	http_response.http_code = 399;
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnHttp502)
{
	http_response.http_code = 502;
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnEmptyResponse)
{
	WebDriverProtocol protocol(&http_client, kTestUrl);
	http_response.body = "";
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnMalformedResponse)
{
	WebDriverProtocol protocol(&http_client, kTestUrl);
	http_response.body = "Blah blah blah";
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseIfResponseIsNotAnObject)
{
	WebDriverProtocol protocol(&http_client, kTestUrl);
	http_response.body = "\"value\":123";
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnMissingStatus)
{
	http_response.body = "{\"sessionId\":\"123\",\"value\":12345}";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnInvalidStatus)
{
	http_response.body = "{\"sessionId\":\"123\",\"status\":\"5\",\"value\":12345}";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnNonZeroStatus)
{
	WebDriverProtocol protocol(&http_client, kTestUrl);
	http_response.body = "{\"sessionId\":\"123\",\"status\":5,\"value\":12345}";
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}

TEST_F(TestWebDriverProtocol, ThrowsInvalidResponseOnMissingValue)
{
	http_response.body = "{\"sessionId\":\"123\",\"status\":0}";
	WebDriverProtocol protocol(&http_client, kTestUrl);
	ASSERT_THROW(protocol.Get("command"), InvalidResponseException);
}
