#ifndef WEBDRIVERXX_DETAIL_RESOURCE_H
#define WEBDRIVERXX_DETAIL_RESOURCE_H

#include "http_client.h"
#include "error_handling.h"
#include "../response_status_code.h"
#include <picojson.h>

namespace webdriverxx {
namespace detail {

const char *const kContentTypeJson = "application/json;charset=UTF-8";

class Resource // copyable
{
public:
	Resource(const IHttpClient* http_client, const std::string& url)
		: http_client_(http_client)
		, url_(url)
	{}

	const std::string& GetUrl() const
	{
		return url_;
	}

	Resource GetSubResource(const std::string& name) const
	{
		return Resource(http_client_, ConcatUrl(url_, name));
	}

	picojson::value Get(const char* command) const
	{
		try
		{
			return ProcessResponse(http_client_->Get(
				ConcatUrl(url_, command)
				));
		}
		catch (const std::exception& e)
		{
			return Rethrow(Fmt() << "Cannot GET \"" << command << "\" ("
				<< "resource: " << url_
				<< ")"
				, picojson::value());
		}
	}

	picojson::value Post(
		const char* command,
		const picojson::value& post_data
		) const
	{
		try
		{
			return ProcessResponse(http_client_->Post(
				ConcatUrl(url_, command),
				kContentTypeJson,
				post_data.serialize()
				));
		}
		catch (const std::exception& e)
		{
			return Rethrow(Fmt() << "Cannot POST \"" << command << "\" ("
				<< "resource: " << url_
				<< ", post_data: " << post_data.serialize()
				<< ")"
				, picojson::value());
		}
	}

private:
	picojson::value ProcessResponse(const HttpResponse& http_response) const
	{
		try
		{
			return ProcessResponseImpl(http_response);
		}
		catch (const std::exception& e)
		{
			return Rethrow(Fmt() << "Error while processing response ("
				<< "HTTP code: " << http_response.http_code
				<< ", body: " << http_response.body
				<< ")"
				, picojson::value());
		}
	}

	picojson::value ProcessResponseImpl(const HttpResponse& http_response) const
	{
		if (http_response.http_code / 100 == 4 || http_response.http_code == 501)
			throw WebDriverException("Invalid request");

		picojson::value response;
		std::string error_message;
		picojson::parse(response, http_response.body.begin(), http_response.body.end(), &error_message);

		if (!error_message.empty())
			throw WebDriverException(
				Fmt() << "JSON parser error (" << error_message << ")"
				);

		Check(response.is<picojson::object>(), "Server response is not an object");
		Check(response.contains("status"), "Server response has no member \"status\"");
		Check(response.get("status").is<double>(), "Response status code is not a number");
		const response_status_code::Value status =
			static_cast<response_status_code::Value>(response.get("status").get<double>());
		Check(response.contains("value"), "Server response has no member \"value\"");
		const picojson::value& value = response.get("value");

		if (http_response.http_code == 500) // Internal server error
		{
			Check(value.is<picojson::object>(), "Server returned HTTP code 500 and \"response.value\" is not an object");
			Check(value.contains("message"), "Server response has no member \"value.message\"");
			Check(value.get("message").is<std::string>(), "\"value.message\" is not a string");
			throw WebDriverException(Fmt() << "Server failed to execute command ("
				<< "message: " << value.get("message").to_str()
				<< ", status: " << response_status_code::ToString(status)
				<< ", status_code: " << status
				<< ")"
				);
		}
		Check(status == response_status_code::kSuccess, "Non-zero response status code");
		Check(http_response.http_code == 200, "Unsupported HTTP code");

		return response;
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
