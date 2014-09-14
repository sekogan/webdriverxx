#ifndef WEBDRIVERXX_DETAIL_CONNECTION_H
#define WEBDRIVERXX_DETAIL_CONNECTION_H

#include "http_request.h"
#include <curl/curl.h>
#include <rapidjson/error/en.h>
#include <string>
#include <sstream>
#include <stdexcept>

namespace webdriverxx {
namespace detail {

template<class JsonDocument>
class Connection
{
public:
	typedef typename JsonDocument::ValueType JsonValue;

public:
	explicit Connection(
		const std::string& url
		)
		: url_(url)
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
		return url_ + command;
	}

	JsonValue ProcessResponse(const HttpResponse& http_response) const
	{
		if (http_response.http_code != 200)
		{
			std::ostringstream formatter;
			formatter << "Got HTTP code " << http_response.http_code;
			throw std::runtime_error(formatter.str());
		}
		JsonDocument document;
		if (document.Parse(http_response.body.c_str()).HasParseError())
		{
			std::ostringstream formatter;
			formatter << "JSON parser error: "
				<< rapidjson::GetParseError_En(document.GetParseError())
				<< " at " << document.GetErrorOffset();
			throw std::runtime_error(formatter.str());
		}
		const auto& status = GetRequiredMember(document, "status");
		if (!status.IsInt() || status.GetInt() != 0)
		{
			std::ostringstream formatter;
			formatter << "Got server status " << status.GetString();
			throw std::runtime_error(formatter.str());
		}
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
		{
			std::ostringstream formatter;
			formatter << "Object has no \"" << name << "\" member";
			throw std::runtime_error(formatter.str());
		}
		return iterator->value;
	}

private:
	Connection(Connection&);
	Connection& operator=(Connection&);

private:
	const std::string url_;
	CURL *const http_connection_;  
};

} // namespace detail
} // namespace webdriverxx

#endif
