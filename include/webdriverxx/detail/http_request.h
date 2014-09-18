#ifndef WEBDRIVERXX_DETAIL_HTTP_REQUEST_H
#define WEBDRIVERXX_DETAIL_HTTP_REQUEST_H

#include "../errors.h"
#include <curl/curl.h>
#include <string>
#include <algorithm>

namespace webdriverxx {
namespace detail {

class HttpHeaders
{
public:
	HttpHeaders()
		: head_(0)
	{}

	~HttpHeaders()
	{
		curl_slist_free_all(head_);
	}

	void Add(const std::string& name, const std::string& value)
	{
		head_ = curl_slist_append(head_, (name + ": " + value).c_str());
		if (!head_)
			throw HttpException("Cannot add HTTP header");
	}

	curl_slist* Get() const
	{
		return head_;
	}

private:
	curl_slist* head_;
};

class HttpRequestBase
{
public:
	HttpResponse DoRequest()
	{
		curl_easy_reset(http_connection_);
		SetOption(CURLOPT_URL, url_.c_str());
		SetOption(CURLOPT_WRITEFUNCTION, &WriteCallback);
		HttpResponse response;
		SetOption(CURLOPT_WRITEDATA, &response.body);
		char error_message[CURL_ERROR_SIZE];
  		SetOption(CURLOPT_ERRORBUFFER, &error_message);
		
		SetCustomRequestOptions();
		
		SetOption(CURLOPT_HTTPHEADER, headers_.Get());

		const CURLcode result = curl_easy_perform(http_connection_);
		if (result != CURLE_OK)
			throw HttpException(detail::Fmt()
				<< "Cannot perform HTTP request (" << result << "): "
				<< error_message);

		response.http_code = GetHttpCode();
		return response;
	}

protected:
	HttpRequestBase(
		CURL* http_connection,
		const std::string& url
		)
		: http_connection_(http_connection)
		, url_(url)
	{}

	virtual void SetCustomRequestOptions()
	{}

	template<typename T>
	void SetOption(CURLoption option, const T& value) const
	{
		const CURLcode result = curl_easy_setopt(http_connection_, option, value);
		if (result != CURLE_OK)
			throw HttpSetOptionException(option, result);
	}

	void AddHeader(const std::string& name, const std::string& value)
	{
		headers_.Add(name, value);
	}

private:
	long GetHttpCode() const
	{
		long http_code = 0;
		const CURLcode result = curl_easy_getinfo(http_connection_, CURLINFO_RESPONSE_CODE, &http_code);
		if (result != CURLE_OK)
			throw HttpException("Cannot get HTTP return code: ", result);
		return http_code;
	}

	static
	size_t WriteCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
	{
		std::string* data_received = reinterpret_cast<std::string*>(userdata);
		const size_t buffer_size = size * nmemb;
		data_received->append(reinterpret_cast<const char*>(buffer), buffer_size);
		return buffer_size;
	}

private:
	HttpRequestBase(HttpRequestBase&);
	HttpRequestBase& operator=(HttpRequestBase&);

private:
	CURL *const http_connection_;
	const std::string url_;
	HttpHeaders headers_;
};

class HttpGetRequest : public HttpRequestBase
{
public:
	HttpGetRequest(CURL* http_connection, const std::string& url)
		: HttpRequestBase(http_connection, url)
	{}
};

class HttpPostRequest : public HttpRequestBase
{
public:
	HttpPostRequest(
		CURL* http_connection,
		const std::string& url,
		const std::string& content_type,
		const std::string& post_data
		)
		: HttpRequestBase(http_connection, url)
		, content_type_(content_type)
		, post_data_(post_data)
		, unsent_ptr_(post_data.c_str())
		, unsent_length_(post_data.size())
	{}

private:
	void SetCustomRequestOptions()
	{
		SetOption(CURLOPT_POST, 1L);
		SetOption(CURLOPT_POSTFIELDSIZE, post_data_.length());
		AddHeader("Content-Type", content_type_);
		SetOption(CURLOPT_READFUNCTION, ReadCallback);
		SetOption(CURLOPT_READDATA, &post_data_);
	}

	static
	size_t ReadCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
	{
		HttpPostRequest* that = reinterpret_cast<HttpPostRequest*>(userdata);
		size_t buffer_size = size * nmemb;
		size_t copy_size = that->unsent_length_ < buffer_size ? that->unsent_length_ : buffer_size;
		std::copy(that->unsent_ptr_, that->unsent_ptr_ + copy_size,
			reinterpret_cast<char*>(buffer));
		that->unsent_length_ -= copy_size;
		that->unsent_ptr_ += copy_size;
		return copy_size;
	}

private:
	const std::string& content_type_;
	const std::string& post_data_;
	const char* unsent_ptr_;
	size_t unsent_length_;
};

} // namespace detail
} // namespace webdriverxx

#endif
