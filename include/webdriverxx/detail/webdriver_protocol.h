#ifndef WEBDRIVERXX_DETAIL_WEBDRIVER_PROTOCOL_H
#define WEBDRIVERXX_DETAIL_WEBDRIVER_PROTOCOL_H

#include "http_client.h"
#include "../errors.h"

namespace webdriverxx {
namespace detail {

template<class JsonDocument>
class WebDriverProtocolBase
{
public:
	typedef typename JsonDocument::ValueType JsonValue;
	typedef typename JsonDocument::MemberIterator JsonMemberIterator;

public:
	WebDriverProtocolBase(
		const std::string& base_url,
		IHttpClient& http_client
		)
		: base_url_(base_url)
		, http_client_(http_client)
	{}

	const JsonValue& Get(const std::string& command)
	{
		return ProcessResponse(http_client_.Get(MakeUrl(command)));
	}

	const JsonValue& GetLastServerFailInfo() const
	{
		return last_server_fail_info_;
	}

private:
	std::string MakeUrl(const std::string& command) const
	{
		return base_url_ + command;
	}

	const JsonValue& ProcessResponse(const HttpResponse& http_response)
	{
		JsonValue().Swap(last_server_fail_info_);

		if (http_response.http_code / 100 == 4 || http_response.http_code == 501)
			throw InvalidRequestException(http_response.http_code, http_response.body);

		JsonDocument json(&allocator_);
		if (json.Parse(http_response.body.c_str()).HasParseError())
		{
			throw JsonParserException(json.GetParseError(), json.GetErrorOffset(),
				http_response.http_code, http_response.body);
		}

		CheckResponse(json.HasMember("status"), "Server response has no member \"status\"", http_response);
		CheckResponse(json["status"].IsInt(), "Response status code is not a number", http_response);
		const response_status_code::Value status =
			static_cast<response_status_code::Value>(json["status"].GetInt());
		CheckResponse(json.HasMember("value"), "Server response has no member \"value\"", http_response);

		if (http_response.http_code == 500) // Internal server error
		{
			last_server_fail_info_.Swap(json["value"]);
			CheckResponse(last_server_fail_info_.HasMember("message"), "Server response has no member \"value.message\"", http_response);
			CheckResponse(last_server_fail_info_["message"].IsString(), "\"value.message\" is not a string", http_response);
			throw FailedCommandException(
				status,
				last_server_fail_info_["message"].GetString()
				);
		}
		CheckResponse(status == response_status_code::kSuccess, "Non-zero response status code", http_response);
		CheckResponse(http_response.http_code == 200, "Unsupported HTTP code", http_response);

		last_server_response_.Swap(json["value"]);
		return last_server_response_;
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
	WebDriverProtocolBase(WebDriverProtocolBase&);
	WebDriverProtocolBase& operator=(WebDriverProtocolBase&);

private:
	const std::string base_url_;
	IHttpClient& http_client_;
	typename JsonDocument::AllocatorType allocator_;
	JsonValue last_server_fail_info_;
	JsonValue last_server_response_;
};

} // namespace detail
} // namespace webdriverxx

#endif
