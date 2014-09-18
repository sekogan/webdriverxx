#ifndef WEBDRIVERXX_DETAIL_WEBDRIVER_PROTOCOL_H
#define WEBDRIVERXX_DETAIL_WEBDRIVER_PROTOCOL_H

#include "http_client.h"
#include "../errors.h"
#include <picojson.h>

namespace webdriverxx {
namespace detail {

class WebDriverProtocol
{
public:
	WebDriverProtocol(
		const IHttpClient* http_client,
		const std::string& url
		)
		: http_client_(http_client)
		, url_(url)
	{}

	std::string GetUrl() const
	{
		return url_;
	}

	picojson::value Get(const std::string& command) const
	{
		return ProcessResponse(http_client_->Get(ConcatUrl(url_, command)));
	}

private:
	picojson::value ProcessResponse(const HttpResponse& http_response) const
	{
		if (http_response.http_code / 100 == 4 || http_response.http_code == 501)
			throw InvalidRequestException(http_response.http_code, http_response.body);

		picojson::value response;
		std::string error_message;
		picojson::parse(response, http_response.body.begin(), http_response.body.end(), &error_message);

		if (!error_message.empty())
			throw InvalidResponseException(
				Fmt() << "JSON parser error (" << error_message << ")",
				http_response.http_code, http_response.body
				);

		CheckResponse(response.is<picojson::object>(), "Server response is not an object", http_response);
		CheckResponse(response.contains("status"), "Server response has no member \"status\"", http_response);
		CheckResponse(response.get("status").is<double>(), "Response status code is not a number", http_response);
		const response_status_code::Value status =
			static_cast<response_status_code::Value>(response.get("status").get<double>());
		CheckResponse(response.contains("value"), "Server response has no member \"value\"", http_response);
		const picojson::value& value = response.get("value");

		if (http_response.http_code == 500) // Internal server error
		{
			CheckResponse(value.is<picojson::object>(), "Server returned HTTP code 500 and \"response.value\" is not an object", http_response);
			CheckResponse(value.contains("message"), "Server response has no member \"value.message\"", http_response);
			CheckResponse(value.get("message").is<std::string>(), "\"value.message\" is not a string", http_response);
			throw FailedCommandException(
				status,
				value.get("message").get<std::string>()
				);
		}
		CheckResponse(status == response_status_code::kSuccess, "Non-zero response status code", http_response);
		CheckResponse(http_response.http_code == 200, "Unsupported HTTP code", http_response);
		const picojson::value& sessionId = response.get("sessionId");
		CheckResponse(sessionId.is<picojson::null>() || sessionId.is<std::string>(), "Session ID has unsupported type", http_response);		

		return response;
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

	static
	std::string ConcatUrl(const std::string& a, const std::string& b, const char delim = '/')
	{
		std::string result = a.empty() ? b : a;
		if (!a.empty() && !b.empty())
		{
			if (result[result.length()-1] != delim)
				result += delim;
			result.append(b[0] == delim ? b.begin() + 1 : b.begin(), b.end());
		}
		return result;
	}

private:
	const IHttpClient* http_client_;
	std::string url_;
};

} // namespace detail
} // namespace webdriverxx

#endif
