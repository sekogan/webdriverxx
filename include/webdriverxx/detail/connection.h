#ifndef WEBDRIVERXX_DETAIL_CONNECTION_H
#define WEBDRIVERXX_DETAIL_CONNECTION_H

#include "http_request.h"
#include "../errors.h"
#include <curl/curl.h>
#include <string>

namespace webdriverxx {
namespace detail {

template<class JsonDocument>
class Connection
{
public:
	typedef typename JsonDocument::ValueType JsonValue;

public:
	explicit Connection(
		const std::string& base_url
		)
		: base_url_(base_url)
		, http_connection_(InitCurl())
	{}

	~Connection()
	{
		curl_easy_cleanup(http_connection_);
	}

	JsonValue Get(const std::string& command) const
	{
		HttpGetRequest request(http_connection_, MakeUrl(command));
		return ProcessResponse(request.DoRequest());
	}

private:
	static CURL* InitCurl()
	{
		CURL *const result = curl_easy_init();
		if (!result)
			throw std::runtime_error("Cannot initialize CURL");
		return result;
	}

	std::string MakeUrl(const std::string& command) const
	{
		return base_url_ + command;
	}

	JsonValue ProcessResponse(const HttpResponse& http_response) const
	{
		if (http_response.http_code != 200)
			throw InvalidHttpCodeException(http_response.http_code);
		JsonDocument document;
		if (document.Parse(http_response.body.c_str()).HasParseError())
			throw JsonParserException(document.GetParseError(), document.GetErrorOffset());
		const auto& status = GetRequiredMember(document, "status");
		if (!status.IsInt() || status.GetInt() != 0)
			throw WebDriverException(std::string("Got server status ") + status.GetString());
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
	CURL *const http_connection_;  
};

} // namespace detail
} // namespace webdriverxx

#endif
