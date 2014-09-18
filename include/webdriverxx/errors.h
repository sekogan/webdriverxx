#ifndef WEBDRIVERXX_ERRORS_H
#define WEBDRIVERXX_ERRORS_H

#include "response_status_code.h"
#include <curl/curl.h>
#include <stdexcept>
#include <string>
#include <sstream>

namespace webdriverxx {
namespace detail {

class Fmt
{
public:
	template<typename T>
	Fmt& operator<< (const T& value)
	{
		stream_ << value;
		return *this;
	}

	std::string ToString() const
	{
		return stream_.str();
	}

	operator std::string() const
	{
		return ToString();
	}

private:
	std::ostringstream stream_;
};

} // namespace detail

// Base class for all errors
struct WebDriverException : std::runtime_error
{
	explicit WebDriverException(const std::string& message)
		: std::runtime_error(message)
	{}
};

// HTTP or network failure
struct HttpException : WebDriverException
{
	explicit HttpException(const std::string& message)
		: WebDriverException(message)
		, error_code(CURLE_OK)
	{}

	HttpException(const std::string& prefix, CURLcode error_code)
		: WebDriverException(prefix + " (" + curl_easy_strerror(error_code) + ")")
		, error_code(error_code)
	{}

	CURLcode error_code;
};

// Server returned something invalid
struct InvalidResponseException : WebDriverException
{
	InvalidResponseException(
		const std::string& reason,
		long http_code,
		const std::string& http_response_body
		)
		: WebDriverException(detail::Fmt()
			<< "Invalid response from server ("
			<< "reason: " << reason
			<< ", HTTP code: " << http_code
			<< ", body: " << http_response_body << ")"
			)
		, reason(reason)
		, http_code(http_code)
		, http_response_body(http_response_body)
	{}
	~InvalidResponseException() throw() {}

	std::string reason;
	long http_code;
	std::string http_response_body;
};

// Invalid/unimplemented command or invalid parameters
struct InvalidRequestException : WebDriverException
{
	InvalidRequestException(long http_code, const std::string& message)
		: WebDriverException(detail::Fmt()
			<< "Invalid request (HTTP code: " << http_code
			<< ", message: " << message << ")"
			)
		, http_code(http_code)
		, message(message)
	{}
	~InvalidRequestException() throw() {}

	long http_code;
	std::string message;
};

// Server failed to execute a valid command with valid parameters
struct FailedCommandException : WebDriverException
{
	FailedCommandException(
		response_status_code::Value status,
		const std::string& message
		)
		: WebDriverException(detail::Fmt()
			<< "Command failed (message: " << message << ")"
			)
		, status(status)
		, message(message)
	{}
	~FailedCommandException() throw() {}

	response_status_code::Value status;
	std::string message;
};

namespace detail {

struct HttpSetOptionException : HttpException
{
	HttpSetOptionException(CURLoption option, CURLcode error_code)
		: HttpException(Fmt() << "Cannot set option " << option, error_code)
		, option(option)
	{}

	CURLoption option;
};

} // namespace detail
} // namespace webdriverxx

#endif
