#ifndef WEBDRIVERXX_ERRORS_H
#define WEBDRIVERXX_ERRORS_H

#include <curl/curl.h>
#include <rapidjson/error/en.h>
#include <stdexcept>
#include <string>
#include <sstream>

namespace webdriverxx {
namespace detail {

class Formatter
{
public:
	template<typename T>
	Formatter& operator<< (const T& value)
	{
		stream_ << value;
		return *this;
	}

	std::string Str() const
	{
		return stream_.str();
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

// Invalid/unimplemented command or invalid parameters
struct InvalidRequestException : WebDriverException
{
	InvalidRequestException(long http_code_, const std::string& message_)
		: WebDriverException((detail::Formatter()
			<< "Invalid request (HTTP code: " << http_code_ << ", message: " << message_ << ")"
			).Str())
		, http_code(http_code_)
	{}

	long http_code;
};

// Server failed to execute valid command with valid parameters
struct FailedCommandException : WebDriverException
{
	explicit FailedCommandException(const std::string& message_)
		: WebDriverException((detail::Formatter()
			<< "Command failed (message: " << message_ << ")"
			).Str())
	{}
};

// Server returned unsupported HTTP code
struct UnsupportedHttpCodeException : WebDriverException
{
	explicit UnsupportedHttpCodeException(long http_code_)
		: WebDriverException((detail::Formatter()
			<< "Unsupported HTTP code (" << http_code_ << ")"
			).Str())
		, http_code(http_code_)
	{}

	long http_code;
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

struct HttpSetOptionException : HttpException
{
	HttpSetOptionException(CURLoption option_, CURLcode error_code_)
		: HttpException((detail::Formatter()
			<< "Cannot set option " << option_
			).Str(), error_code_)
		, option(option_)
	{}

	CURLoption option;
};

// Server returned invalid JSON
struct JsonParserException : WebDriverException
{
	JsonParserException(rapidjson::ParseErrorCode error_code_, size_t error_offset_)
		: WebDriverException((detail::Formatter()
			<< "JSON parser error (message: " << rapidjson::GetParseError_En(error_code_)
			<< " offset: " << error_offset_ << ")"
			).Str())
		, error_code(error_code_)
		, error_offset(error_offset_)
	{}

	rapidjson::ParseErrorCode error_code;
	size_t error_offset;
};

} // namespace webdriverxx

#endif
