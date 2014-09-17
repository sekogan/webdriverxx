#include "common_test_data.h"
#include <webdriverxx/detail/http_connection.h>
#include <gtest/gtest.h>
#include <string>

using namespace webdriverxx;
using namespace webdriverxx::detail;

TEST(HttpConnection, CanBeCreated) {
	HttpConnection connection;
}

TEST(HttpConnection, CanGetPage) {
	HttpConnection connection;
	HttpResponse response = connection.Get(std::string(kPhantomUrl) + "status");
	ASSERT_EQ(200, response.http_code);
	ASSERT_TRUE(!response.body.empty());
}

TEST(HttpConnection, ThrowsExceptionIfPortIsClosed) {
	HttpConnection connection;
	ASSERT_THROW(connection.Get("http://127.0.0.1:7778/"), HttpException);
}

// TODO: remove
#include <webdriverxx/capabilities.h>
#include <picojson.h>

namespace json = picojson;

json::object::value_type ToJsonPair(const std::pair<std::string, std::string>& pair)
{
	return std::make_pair(pair.first, json::value(pair.second));
}

TEST(PicoJson, CanCreateJson)
{
	Capabilities required, desired;
	required.items["aaa"] = "bbb";
	desired.items["ccc"] = "ddd";

	json::value json = json::value(json::object());
	json.get<json::object>()["requiredCapabilities"] = json::value(json::object());
	json.get<json::object>()["desiredCapabilities"] = json::value(json::object());

	json::object& requiredJson = json.get<json::object>()["requiredCapabilities"].get<json::object>();
	std::transform(required.items.begin(), required.items.end(),
		std::inserter(requiredJson, requiredJson.end()),
		ToJsonPair);
	json::object& desiredJson = json.get<json::object>()["desiredCapabilities"].get<json::object>();
	std::transform(desired.items.begin(), desired.items.end(),
		std::inserter(desiredJson, desiredJson.end()),
		ToJsonPair);

	ASSERT_NE(std::string::npos, json.serialize().find("bbb"));
}

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

typedef std::map<std::string, std::string> StringMap;

using namespace rapidjson;

TEST(RapidJson, CanCreateJson)
{
	Capabilities required, desired;
	required.items["aaa"] = "bbb";
	desired.items["ccc"] = "ddd";

	Document document;

	Document::ValueType requiredJson(kObjectType);
	for (StringMap::const_iterator it = required.items.begin();
		it != required.items.end(); ++it)
		requiredJson.AddMember(
			Document::ValueType::StringRefType(it->first.c_str()),
			Document::ValueType(Document::ValueType::StringRefType(it->second.c_str())).Move(),
			document.GetAllocator()
			);	
	Document::ValueType desiredJson(kObjectType);
	for (StringMap::const_iterator it = desired.items.begin();
		it != desired.items.end(); ++it)
		desiredJson.AddMember(
			Document::ValueType::StringRefType(it->first.c_str()),
			Document::ValueType(Document::ValueType::StringRefType(it->second.c_str())).Move(),
			document.GetAllocator()
			);	

	document.SetObject();
	document.AddMember("requiredCapabilities", requiredJson, document.GetAllocator());
	document.AddMember("desiredCapabilities", desiredJson, document.GetAllocator());
	StringBuffer jsonBuffer;
	Writer<StringBuffer> writer(jsonBuffer);
	document.Accept(writer);

	ASSERT_NE(std::string::npos, std::string(jsonBuffer.GetString()).find("bbb"));
}
