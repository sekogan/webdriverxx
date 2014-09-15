#ifndef WEBDRIVERXX_DETAIL_CONNECTION_H
#define WEBDRIVERXX_DETAIL_CONNECTION_H

#include "http_client.h"
#include "../errors.h"

namespace webdriverxx {
namespace detail {

template<class JsonDocument>
class Connection
{
public:
	typedef typename JsonDocument::ValueType JsonValue;

public:
	Connection(
		const std::string& base_url,
		IHttpClient& http_client
		)
		: base_url_(base_url)
		, http_client_(http_client)
	{}

	JsonValue Get(const std::string& command)
	{
		return ProcessResponse(http_client_.Get(MakeUrl(command)));
	}

	const JsonValue& GetLastFailedCommandDetails() const
	{
		return last_failed_command_details_;
	}

private:
	std::string MakeUrl(const std::string& command) const
	{
		return base_url_ + command;
	}

	JsonValue ProcessResponse(const HttpResponse& http_response)
	{
		if (http_response.http_code % 100 == 4 || http_response.http_code == 501)
			throw InvalidRequestException(http_response.http_code, http_response.body);
		JsonDocument document(&allocator_);
		if (document.Parse(http_response.body.c_str()).HasParseError())
			throw JsonParserException(document.GetParseError(), document.GetErrorOffset());
		if (http_response.http_code == 500) // Internal server error
		{
			last_failed_command_details_.Swap(GetRequiredMember(document, "value"));
			throw FailedCommandException(GetRequiredMember(document, "message").GetString());
		}
		if (http_response.http_code != 200)
			throw UnsupportedHttpCodeException(http_response.http_code);
		const auto& status = GetRequiredMember(document, "status");
		if (!status.IsInt() || status.GetInt() != 0)
			throw WebDriverException(std::string("Invalid server status ") + status.GetString());
		auto& value = GetRequiredMember(document, "value");
		JsonValue result;
		result.Swap(value);
		return result;
	}

	JsonValue& GetRequiredMember(JsonValue& value, const char* name) const
	{
		if (!value.IsObject())
			throw std::runtime_error("Value is not an object");
		const auto iterator = value.FindMember(name);
		if (iterator == value.MemberEnd())
			throw WebDriverException(std::string("Object has no member \"") + name + "\"");
		return iterator->value;
	}

private:
	Connection(Connection&);
	Connection& operator=(Connection&);

private:
	const std::string base_url_;
	IHttpClient& http_client_;
	typename JsonDocument::AllocatorType allocator_;
	JsonValue last_failed_command_details_;
};

} // namespace detail
} // namespace webdriverxx

#endif
