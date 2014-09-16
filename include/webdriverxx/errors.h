#ifndef WEBDRIVERXX_ERRORS_H
#define WEBDRIVERXX_ERRORS_H

#include "response_status_code.h"
#include <curl/curl.h>
#include <rapidjson/error/en.h>
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
	explicit WebDriverException(const std::string& message_)
		: std::runtime_error(message_)
	{}
};

// HTTP or network failure
struct HttpException : WebDriverException
{
	explicit HttpException(const std::string& message_)
		: WebDriverException(message_)
		, error_code(CURLE_OK)
	{}

	HttpException(const std::string& prefix_, CURLcode error_code_)
		: WebDriverException(prefix_ + " (" + curl_easy_strerror(error_code_) + ")")
		, error_code(error_code_)
	{}

	CURLcode error_code;
};

// Server returned something invalid
struct InvalidResponseException : WebDriverException
{
	InvalidResponseException(
		const std::string& reason_,
		long http_code_,
		const std::string& http_response_body_
		)
		: WebDriverException(detail::Fmt()
			<< "Invalid response from server ("
			<< "reason: " << reason_
			<< ", HTTP code: " << http_code_
			<< ", body: " << http_response_body_ << ")"
			)
		, reason(reason_)
		, http_code(http_code_)
		, http_response_body(http_response_body_)
	{}
	~InvalidResponseException() throw() {}

	std::string reason;
	long http_code;
	std::string http_response_body;
};

// Invalid/unimplemented command or invalid parameters
struct InvalidRequestException : WebDriverException
{
	InvalidRequestException(long http_code_, const std::string& message_)
		: WebDriverException(detail::Fmt()
			<< "Invalid request (HTTP code: " << http_code_
			<< ", message: " << message_ << ")"
			)
		, http_code(http_code_)
		, message(message_)
	{}
	~InvalidRequestException() throw() {}

	long http_code;
	std::string message;
};

// Server failed to execute a valid command with valid parameters
struct FailedCommandException : WebDriverException
{
	FailedCommandException(
		response_status_code::Value status_,
		const std::string& message_
		)
		: WebDriverException(detail::Fmt()
			<< "Command failed (message: " << message_ << ")"
			)
		, status(status_)
		, message(message_)
	{}
	~FailedCommandException() throw() {}

	response_status_code::Value status;
	std::string message;
};

namespace detail {

struct JsonParserException : InvalidResponseException
{
	JsonParserException(
		rapidjson::ParseErrorCode error_code_,
		size_t error_offset_,
		long http_code_,
		const std::string& http_response_body_
		)
		: InvalidResponseException(detail::Fmt()
			<< "JSON parser error ("
				<< "message: " << rapidjson::GetParseError_En(error_code_)
				<< ", offset: " << error_offset_
				<< ")",
			http_code_,
			http_response_body_
			)
	{}
};

struct HttpSetOptionException : HttpException
{
	HttpSetOptionException(CURLoption option_, CURLcode error_code_)
		: HttpException(detail::Fmt() << "Cannot set option " << option_, error_code_)
		, option(option_)
	{}

	CURLoption option;
};

} // namespace detail
} // namespace webdriverxx

#endif
