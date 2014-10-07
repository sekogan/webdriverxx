#include <webdriverxx/detail/resource.h>
#include <webdriverxx/detail/http_client.h>
#include <webdriverxx/response_status_code.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace webdriverxx;
using namespace webdriverxx::detail;

const char *const kTestUrl = "http://test/";

struct MockHttpClient : IHttpClient, SharedObjectBase {
	MOCK_CONST_METHOD1(Get, HttpResponse(const std::string& url));
	MOCK_CONST_METHOD2(Post, HttpResponse(const std::string& url, const std::string& data));
	MOCK_CONST_METHOD1(Delete, HttpResponse(const std::string& url));
};

using namespace ::testing;

struct TestResource : Test {
	void SetUp() {
		http_response.http_code = 200;
		http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	
		http_client = Shared<MockHttpClient>(new MockHttpClient);
		DefaultValue<HttpResponse>::Set(HttpResponse());
		ON_CALL(*http_client, Get(_)).WillByDefault(ReturnPointee(&http_response));
		ON_CALL(*http_client, Post(_,_)).WillByDefault(ReturnPointee(&http_response));
		ON_CALL(*http_client, Delete(_)).WillByDefault(ReturnPointee(&http_response));
		EXPECT_CALL(*http_client, Get(_)).Times(AnyNumber());
		EXPECT_CALL(*http_client, Post(_,_)).Times(AnyNumber());
		EXPECT_CALL(*http_client, Delete(_)).Times(AnyNumber());
	}

	Shared<MockHttpClient> http_client;
	HttpResponse http_response;
};

// Positive tests

TEST_F(TestResource, CanBeCreated) {
	Resource resource(kTestUrl, http_client);
}

TEST_F(TestResource, ReturnsUrl) {
	Resource resource(kTestUrl, http_client);
	ASSERT_EQ(kTestUrl, resource.GetUrl());
}

TEST_F(TestResource, CanBeUsedToMakeSubResource) {
	Shared<Resource> a(new Resource("a", http_client));
	ASSERT_EQ("a/c", MakeSubResource(a, "c")->GetUrl());
	ASSERT_EQ("a/c",MakeSubResource(a, "/c")->GetUrl());
	Shared<Resource> b(new Resource("b/", http_client));
	ASSERT_EQ("b/c", MakeSubResource(b, "c")->GetUrl());
	ASSERT_EQ("b/c", MakeSubResource(b, "/c")->GetUrl());
}

TEST_F(TestResource, DoesNotDeleteResourceByDefault)
{
	EXPECT_CALL(*http_client, Delete(_)).Times(0);
	Resource resource(kTestUrl, http_client);
}

TEST_F(TestResource, DeletesResourceIfOwnershipIsEnabled)
{
	EXPECT_CALL(*http_client, Delete(kTestUrl));
	Resource resource(kTestUrl, http_client, Resource::IsOwner);
}

TEST_F(TestResource, SharesOwnershipOfParentResource)
{
	EXPECT_CALL(*http_client, Delete(_)).Times(0);
	Shared<Resource> parent(new Resource("parent", http_client, Resource::IsOwner));
	Shared<Resource> child = MakeSubResource(parent, "child", Resource::IsOwner);
	parent = Shared<Resource>();
	Mock::VerifyAndClear(http_client); // Parent shouldn't be deleted at this point because child is alive
	InSequence check_delete_order;
	EXPECT_CALL(*http_client, Delete("parent/child"));
	EXPECT_CALL(*http_client, Delete("parent"));
	child = Shared<Resource>();
}

TEST_F(TestResource, RootResourceReturnsJsonObject)
{
	RootResource resource(kTestUrl, http_client);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	ASSERT_TRUE(resource.Get("command").is<picojson::object>());
}

TEST_F(TestResource, RootResourceReturnsSessionId)
{
	RootResource resource(kTestUrl, http_client);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	ASSERT_TRUE(resource.Get("command").contains("sessionId"));
	ASSERT_EQ("123", resource.Get("command").get("sessionId").to_str());
}

TEST_F(TestResource, RootResourceReturnsNullSessionId)
{
	RootResource resource(kTestUrl, http_client);
	http_response.body = "{\"sessionId\":null,\"status\":0,\"value\":12345}";
	ASSERT_TRUE(resource.Get("command").contains("sessionId"));
	ASSERT_TRUE(resource.Get("command").get("sessionId").is<picojson::null>());
}

TEST_F(TestResource, RootResourceReturnsScalarValueFromPositiveResponse)
{
	RootResource resource(kTestUrl, http_client);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	picojson::value value = resource.Get("command").get("value");
	ASSERT_TRUE(value.is<double>());
	ASSERT_EQ(12345, value.get<double>());
}

TEST_F(TestResource, RootResourceReturnsObjectValueFromPositiveResponse)
{
	RootResource resource(kTestUrl, http_client);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":{\"member\":12345}}";
	picojson::value value = resource.Get("command").get("value");
	ASSERT_TRUE(value.is<picojson::object>());
	ASSERT_TRUE(value.contains("member"));
	ASSERT_TRUE(value.get("member").is<double>());
	ASSERT_EQ(12345, value.get("member").get<double>());
}

TEST_F(TestResource, ReturnsScalarValueFromPositiveResponse)
{
	Resource resource(kTestUrl, http_client);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":12345}";
	picojson::value value = resource.Get("command");
	ASSERT_TRUE(value.is<double>());
	ASSERT_EQ(12345, value.get<double>());
}

TEST_F(TestResource, ReturnsObjectValueFromPositiveResponse)
{
	Resource resource(kTestUrl, http_client);
	http_response.http_code = 200;
	http_response.body = "{\"sessionId\":\"123\",\"status\":0,\"value\":{\"member\":12345}}";
	picojson::value value = resource.Get("command");
	ASSERT_TRUE(value.is<picojson::object>());
	ASSERT_TRUE(value.contains("member"));
	ASSERT_TRUE(value.get("member").is<double>());
	ASSERT_EQ(12345, value.get("member").get<double>());
}

// Negative tests

TEST_F(TestResource, ThrowsOnHttp404)
{
	http_response.http_code = 404;
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp400)
{
	http_response.http_code = 400;
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp499)
{
	http_response.http_code = 499;
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp501)
{
	http_response.http_code = 501;
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, DoesNotHideHttpExceptions)
{
	EXPECT_CALL(*http_client, Get(_)).WillOnce(Throw(WebDriverException("HTTP failed")));
	Resource resource(kTestUrl, http_client);
	try {
		resource.Get("command");
		FAIL(); // Shouldn't get here
	} catch (const std::exception& e) {
		const std::string message = e.what();
		ASSERT_NE(std::string::npos, message.find("HTTP failed"));
	}
}

TEST_F(TestResource, AddsContextToExceptions)
{
	EXPECT_CALL(*http_client, Get(_)).WillOnce(Throw(WebDriverException("HTTP failed")));
	Resource resource(kTestUrl, http_client);
	try {
		resource.Get("pinky");
		FAIL(); // Shouldn't get here
	} catch (const std::exception& e) {
		const std::string message = e.what();
		ASSERT_NE(std::string::npos, message.find("pinky"));
	}
}

TEST_F(TestResource, WebDriverExceptionContainsCommandAndHttpCodeAndBody)
{
	http_response.http_code = 501;
	http_response.body = "--oops--";
	Resource resource(kTestUrl, http_client);
	try {
		resource.Get("pinky");
		FAIL(); // Shouldn't get here
	} catch (const std::exception& e) {
		const std::string message = e.what();
		ASSERT_NE(std::string::npos, message.find("pinky"));
		ASSERT_NE(std::string::npos, message.find("--oops--"));
		ASSERT_NE(std::string::npos, message.find("501"));
	}
}

TEST_F(TestResource, WebDriverExceptionContainsStatusAndStatusDescription)
{
	http_response.http_code = 500;
	http_response.body = Fmt() << "{\"status\":"
		<< response_status_code::kNoSuchWindow
		<< ",\"value\":{\"message\":\"12345\"}}";
	Resource resource(kTestUrl, http_client);
	try {
		resource.Get("pinky");
		FAIL(); // Shouldn't get here
	} catch (const std::exception& e) {
		const std::string message = e.what();
		ASSERT_NE(std::string::npos, message.find(Fmt() << response_status_code::kNoSuchWindow));
		ASSERT_NE(std::string::npos, message.find(response_status_code::ToString(response_status_code::kNoSuchWindow)));
	}
}

TEST_F(TestResource, ThrowsOnHttp500)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"message\":\"12345\"}}";
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp500AndMissingStatus)
{
	http_response.http_code = 500;
	http_response.body = "{\"value\":{\"message\":\"12345\"}}";
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp500AndInvalidStatus)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":\"xxx\",\"value\":{\"message\":\"12345\"}}";
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp500AndMissingValue)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12}";
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp500AndInvalidValue)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":\"xxx\",\"value\":\"12345\"}";
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp500AndMissingMessage)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"xxx\":\"12345\"}}";
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp500AndInvalidMessage)
{
	http_response.http_code = 500;
	http_response.body = "{\"status\":12,\"value\":{\"message\":12345}}";
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp399)
{
	http_response.http_code = 399;
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnHttp502)
{
	http_response.http_code = 502;
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnEmptyResponse)
{
	Resource resource(kTestUrl, http_client);
	http_response.body = "";
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnMalformedResponse)
{
	Resource resource(kTestUrl, http_client);
	http_response.body = "Blah blah blah";
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsIfResponseIsNotAnObject)
{
	Resource resource(kTestUrl, http_client);
	http_response.body = "\"value\":123";
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnMissingStatus)
{
	http_response.body = "{\"sessionId\":\"123\",\"value\":12345}";
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnInvalidStatus)
{
	http_response.body = "{\"sessionId\":\"123\",\"status\":\"5\",\"value\":12345}";
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnNonZeroStatus)
{
	Resource resource(kTestUrl, http_client);
	http_response.body = "{\"sessionId\":\"123\",\"status\":5,\"value\":12345}";
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}

TEST_F(TestResource, ThrowsOnMissingValue)
{
	http_response.body = "{\"sessionId\":\"123\",\"status\":0}";
	Resource resource(kTestUrl, http_client);
	ASSERT_THROW(resource.Get("command"), WebDriverException);
}
