#ifndef WEBDRIVERXX_DETAIL_HTTP_REQUEST_H
#define WEBDRIVERXX_DETAIL_HTTP_REQUEST_H

#include <curl/curl.h>
#include <string>
#include <sstream>
#include <stdexcept>

namespace webdriverxx {
namespace detail {

struct HttpResponse
{
	long http_code;
	std::string body;

	HttpResponse()
		: http_code(0)
	{}
};

class HttpRequestImpl
{
public:
	HttpResponse DoRequest() const
	{
		HttpResponse response;
		curl_easy_reset(http_connection_);
		SetOption(CURLOPT_URL, url_.c_str());
		SetOption(CURLOPT_WRITEFUNCTION, &CurlWriteCallback);
		SetOption(CURLOPT_WRITEDATA, &response.body);
		//SetOption(CURLOPT_ERRORBUFFER, &curl_error_);
		SetCustomRequestOptions(http_connection_);

		const auto result = curl_easy_perform(http_connection_);
		if (result != CURLE_OK)
		{
			std::ostringstream formatter;
			formatter << "curl_easy_perform failed: " << curl_easy_strerror(result);
			throw std::runtime_error(formatter.str());
		}

		response.http_code = GetHttpCode();
		return response;
	}

protected:
	HttpRequestImpl(
		CURL* http_connection,
		const std::string& url
		)
		: http_connection_(http_connection)
		, url_(url)
	{}

	virtual void SetCustomRequestOptions(CURL* http_connection) const
	{}

	template<typename T>
	void SetOption(CURLoption option, const T& value) const
	{
		const auto result = curl_easy_setopt(http_connection_, option, value);
		if (result != CURLE_OK)
		{
			std::ostringstream formatter;
			formatter << "curl_easy_setopt(" << option << ") failed: "
				<< curl_easy_strerror(result);
			throw std::runtime_error(formatter.str());
		}
	}

private:
	long GetHttpCode() const
	{
		long http_code = 0;
		const auto result = curl_easy_getinfo(http_connection_, CURLINFO_RESPONSE_CODE, &http_code);
		if (result != CURLE_OK)
		{
			std::ostringstream formatter;
			formatter << "Cannot get HTTP return code: " << curl_easy_strerror(result);
			throw std::runtime_error(formatter.str());
		}
		return http_code;
	}

	static size_t CurlWriteCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
	{
		std::string* data_received = reinterpret_cast<std::string*>(userdata);
		const auto buffer_size = size * nmemb;
		data_received->append(reinterpret_cast<const char*>(buffer), buffer_size);
		return buffer_size;
	}

private:
	HttpRequestImpl(HttpRequestImpl&);
	HttpRequestImpl& operator=(HttpRequestImpl&);

private:
	CURL *const http_connection_;
	const std::string url_;
	//char curl_error_[CURL_ERROR_SIZE];
};

class HttpGetRequest : public HttpRequestImpl
{
public:
	HttpGetRequest(CURL* http_connection, const std::string& url)
		: HttpRequestImpl(http_connection, url)
	{}

private:
	void SetCustomRequestOptions(CURL* http_connection) const
	{}
};

} // namespace detail
} // namespace webdriverxx

#endif
