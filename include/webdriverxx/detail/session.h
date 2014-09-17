#ifndef WEBDRIVERXX_DETAIL_SESSION_H
#define WEBDRIVERXX_DETAIL_SESSION_H

#include "http_client.h"
#include "../capabilities.h"
#include "../errors.h"
#include <rapidjson/document.h>
//#include <rapidjson/stringbuffer.h>

namespace webdriverxx {
namespace detail {

const char *const kContentTypeJson = "application/json;charset=UTF-8";

template<class JsonDocument>
class SessionBase
{
public:
	typedef typename JsonDocument::ValueType JsonValue;
	typedef typename JsonDocument::MemberIterator JsonMemberIterator;

public:
	SessionBase(
		IHttpClient& http_client,
		const std::string& base_url,
		const Capabilities& required = Capabilities(),
		const Capabilities& desired = Capabilities()
		)
		: http_client_(http_client)
	{
		CreateSession(base_url, required, desired, url_, capabilities_);
	}

	const JsonValue& Get(const std::string& command)
	{
		return ProcessResponse(http_client_.Get(MakeUrl(command)));
	}

	const JsonValue& GetLastJsonResponse() const
	{
		return last_json_response_;
	}

private:
	void CreateSession(
		const std::string& base_url,
		const Capabilities& required,
		const Capabilities& desired,
		std::string& sessionUrl,
		Capabilities& sessionCapabilities
		) const
	{
		sessionUrl = base_url;
		// TODO
		// using namespace rapidjson;
		// JsonDocument document;
		// document.SetObject();
		// document.AddMember("aaa", document.GetAllocator()));
	 //    StringBuffer postBody;
  //   	Writer<StringBuffer> writer(postBody);
  //   	document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.


		// // make json
		// const HttpResponse http_response = http_client.Post(
		// 	ConcatUrl(base_url, "session"), kContentTypeJson, postBody.GetString());
		// const JsonValue& capabilitiesJson = ProcessResponse(http_response);
		// CheckResponse(last_json_response_.HasMember("sessionId"), "Server response has no member \"sessionId\"", http_response);
		// CheckResponse(last_json_response_["sessionId"].IsString(), "Session ID is not a string", http_response);
		// CheckResponse(capabilitiesJson.IsObject(), "Capabilities is not an object", http_response);
		// const std::string sessionId = last_json_response_["sessionId"].GetString();
		// sessionUrl = ConcatUrl(base_url, ConcatUrl("session", sessionId));
		// sessionCapabilities = ConvertCapabilities(capabilitiesJson, http_response);
	}

	std::string MakeUrl(const std::string& command) const
	{
		return url_ + command;
	}

	const JsonValue& ProcessResponse(const HttpResponse& http_response)
	{
		JsonValue().Swap(last_json_response_);

		if (http_response.http_code / 100 == 4 || http_response.http_code == 501)
			throw InvalidRequestException(http_response.http_code, http_response.body);

		JsonDocument json(&allocator_);
		if (json.Parse(http_response.body.c_str()).HasParseError())
		{
			throw JsonParserException(json.GetParseError(), json.GetErrorOffset(),
				http_response.http_code, http_response.body);
		}
		last_json_response_.Swap(json);
		const JsonValue& response = last_json_response_;

		CheckResponse(response.HasMember("status"), "Server response has no member \"status\"", http_response);
		CheckResponse(response["status"].IsInt(), "Response status code is not a number", http_response);
		const response_status_code::Value status =
			static_cast<response_status_code::Value>(response["status"].GetInt());
		CheckResponse(response.HasMember("value"), "Server response has no member \"value\"", http_response);
		const JsonValue& value = response["value"];

		if (http_response.http_code == 500) // Internal server error
		{
			CheckResponse(value.HasMember("message"), "Server response has no member \"value.message\"", http_response);
			CheckResponse(value["message"].IsString(), "\"value.message\" is not a string", http_response);
			throw FailedCommandException(
				status,
				value["message"].GetString()
				);
		}
		CheckResponse(status == response_status_code::kSuccess, "Non-zero response status code", http_response);
		CheckResponse(http_response.http_code == 200, "Unsupported HTTP code", http_response);

		return value;
	}

	static
	void CheckResponse(bool condition, const char* reason,
		const HttpResponse& http_response)
	{
		if (!condition)
		{
			throw InvalidResponseException(reason,
				http_response.http_code, http_response.body);
		}
	}

private:
	SessionBase(SessionBase&);
	SessionBase& operator=(SessionBase&);

private:
	IHttpClient& http_client_;
	std::string url_;
	Capabilities capabilities_;
	typename JsonDocument::AllocatorType allocator_;
	JsonValue last_json_response_;
};

} // namespace detail
} // namespace webdriverxx

#endif
