#include "common_test_data.h"
#include <webdriverxx/detail/session.h>
#include <webdriverxx/detail/http_client.h>
#include <rapidjson/document.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace webdriverxx;
using namespace webdriverxx::detail;

typedef SessionBase<rapidjson::Document> Session;

struct MockHttpClient : IHttpClient
{
	MOCK_METHOD1(Get, HttpResponse(const std::string& url));
	MOCK_METHOD1(Put, HttpResponse(const std::string& url));
	MOCK_METHOD1(Post, HttpResponse(const std::string& url));
	MOCK_METHOD1(Delete, HttpResponse(const std::string& url));
};

using namespace ::testing;

struct TestSession : Test
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

TEST_F(TestSession, CanBeCreated) {
	Session session(http_client, kPhantomUrl);
}

TEST_F(TestSession, ReturnsScalarValueFromPositiveResponse)
{
	Session session(http_client, kPhantomUrl);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	const Session::JsonValue& value = session.Get("command");
	ASSERT_TRUE(value.IsInt());
	ASSERT_EQ(12345, value.GetInt());
}

TEST_F(TestSession, ReturnsObjectValueFromPositiveResponse)
{
	Session session(http_client, kPhantomUrl);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":{\"member\":12345}}";
	const Session::JsonValue& value = session.Get("command");
	ASSERT_TRUE(value.IsObject());
	ASSERT_TRUE(value.HasMember("member"));
	ASSERT_TRUE(value["member"].IsInt());
	ASSERT_EQ(12345, value["member"].GetInt());
}

TEST_F(TestSession, ReturnsEmptyLastServerResponseByDefault)
{
	Session session(http_client, kPhantomUrl);
	const Session::JsonValue& response = session.GetLastJsonResponse();
	ASSERT_TRUE(!response.IsObject());
}

TEST_F(TestSession, ReturnsEmptyLastServerResponseAfterNonJsonResponse)
{
	Session session(http_client, kPhantomUrl);

	// Normal response
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	session.Get("command");
	ASSERT_TRUE(session.GetLastJsonResponse().IsObject());

	// 501 response
	http_response.http_code = 501;
	http_response.body = "error message";
	ASSERT_ANY_THROW(session.Get("command"));
	ASSERT_TRUE(!session.GetLastJsonResponse().IsObject());

	// Normal response
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	session.Get("command");
	ASSERT_TRUE(session.GetLastJsonResponse().IsObject());

	// Malformed response
	http_response.http_code = 200;
	http_response.body = "blah blah blah";
	ASSERT_ANY_THROW(session.Get("command"));
	ASSERT_TRUE(!session.GetLastJsonResponse().IsObject());
}

TEST_F(TestSession, ReturnsLastServerResponse)
{
	Session session(http_client, kPhantomUrl);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	session.Get("command");
	const Session::JsonValue& response = session.GetLastJsonResponse();
	ASSERT_TRUE(response.IsObject());
	ASSERT_TRUE(response.HasMember("value") && response["value"].IsInt());
	ASSERT_EQ(12345, response["value"].GetInt());
}

TEST_F(TestSession, CanHandleNullSessionId)
{
	Session session(http_client, kPhantomUrl);
	http_response.body = "{\"sessionId\":null,\"status\":0,\"value\":12345}";
	ASSERT_NO_THROW(session.Get("command"));
}

// Negative tests

TEST_F(TestSession, DoesNotHideHttpExceptions)
{
	EXPECT_CALL(http_client, Get(_)).WillOnce(Throw(HttpException("")));
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), HttpException);
}
TEST_F(TestSession, ThrowsInvalidRequestOnHttp404)
{
	http_response.http_code = 404;
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidRequestException);
}

TEST_F(TestSession, ThrowsInvalidRequestOnHttp400)
{
	http_response.http_code = 400;
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidRequestException);
}

TEST_F(TestSession, ThrowsInvalidRequestOnHttp499)
{
	http_response.http_code = 499;
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidRequestException);
}

TEST_F(TestSession, ThrowsInvalidRequestOnHttp501)
{
	http_response.http_code = 501;
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidRequestException);
}

TEST_F(TestSession, InvalidRequestExceptionContainsHttpCodeAndBody)
{
	http_response.http_code = 501;
	http_response.body = "--12345--";
	Session session(http_client, kPhantomUrl);
	try
	{
		session.Get("command");
		FAIL(); // Shouldn't get here
	}
	catch (const std::exception& e)
	{
		const std::string message = e.what();
		ASSERT_NE(std::string::npos, message.find("--12345--"));
		ASSERT_NE(std::string::npos, message.find("501"));
	}
}

TEST_F(TestSession, ThrowsFailedCommandExceptionOnHttp500)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"message\":\"12345\"}}";
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), FailedCommandException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnHttp500AndMissingStatus)
{
	http_response.http_code = 500;
	http_response.body = "{\"value\":{\"message\":\"12345\"}}";
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnHttp500AndInvalidStatus)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":\"xxx\",\"value\":{\"message\":\"12345\"}}";
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnHttp500AndMissingValue)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12}";
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnHttp500AndInvalidValue)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":\"xxx\",\"value\":\"12345\"}";
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnHttp500AndMissingMessage)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"xxx\":\"12345\"}}";
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnHttp500AndInvalidMessage)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"message\":12345}}";
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnHttp399)
{
	http_response.http_code = 399;
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnHttp502)
{
	http_response.http_code = 502;
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnEmptyResponse)
{
	Session session(http_client, kPhantomUrl);
	http_response.body = "";
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnMalformedResponse)
{
	Session session(http_client, kPhantomUrl);
	http_response.body = "Blah blah blah";
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnMissingStatus)
{
	http_response.body = "{\"sessionId\":\"123\",\"value\":12345}";
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnInvalidStatus)
{
	http_response.body = "{\"sessionId\":\"123\",\"status\":\"5\",\"value\":12345}";
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnNonZeroStatus)
{
	Session session(http_client, kPhantomUrl);
	http_response.body = "{\"sessionId\":\"123\",\"status\":5,\"value\":12345}";
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}

TEST_F(TestSession, ThrowsInvalidResponseOnMissingValue)
{
	http_response.body = "{\"sessionId\":\"123\",\"status\":0}";
	Session session(http_client, kPhantomUrl);
	ASSERT_THROW(session.Get("command"), InvalidResponseException);
}


