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
		ON_CALL(http_client, Get(_)).WillByDefault(Return(ByRef(http_response)));
		ON_CALL(http_client, Put(_)).WillByDefault(Return(ByRef(http_response)));
		ON_CALL(http_client, Post(_)).WillByDefault(Return(ByRef(http_response)));
		ON_CALL(http_client, Delete(_)).WillByDefault(Return(ByRef(http_response)));
		EXPECT_CALL(http_client, Get(_)).Times(AnyNumber());
		EXPECT_CALL(http_client, Put(_)).Times(AnyNumber());
		EXPECT_CALL(http_client, Post(_)).Times(AnyNumber());
		EXPECT_CALL(http_client, Delete(_)).Times(AnyNumber());
	}

	MockHttpClient http_client;
	HttpResponse http_response;
};

TEST_F(TestWebDriverProtocol, CanBeCreated) {
	WebDriverProtocol protocol("http://127.0.0.1/", http_client);
}

// TODO: negative tests
