#ifndef WEBDRIVERXX_DETAIL_HTTP_REQUEST_H
#define WEBDRIVERXX_DETAIL_HTTP_REQUEST_H

#include "error_handling.h"
#include <curl/curl.h>
#include <string>
#include <algorithm>

namespace webdriverxx {
namespace detail {

const char *const kContentTypeJson = "application/json;charset=UTF-8";

class HttpHeaders {
public:
	HttpHeaders() : head_(0) {}

	~HttpHeaders() {
		curl_slist_free_all(head_);
	}

	void Add(const std::string& name, const std::string& value) {
		head_ = curl_slist_append(head_, (name + ": " + value).c_str());
		WEBDRIVERXX_CHECK(head_, "Cannot add HTTP header");
	}

	curl_slist* Get() const {
		return head_;
	}

private:
	curl_slist* head_;
};

class HttpRequest {
public:
	HttpRequest(
		CURL* http_connection,
		const std::string& url
		)
		: http_connection_(http_connection)
		, url_(url)
	{}

	virtual ~HttpRequest() {}

	HttpResponse Execute() {
		curl_easy_reset(http_connection_);
		SetOption(CURLOPT_URL, url_.c_str());
		HttpResponse response;
		SetOption(CURLOPT_WRITEFUNCTION, &WriteCallback);
		SetOption(CURLOPT_WRITEDATA, &response.body);
		char error_message[CURL_ERROR_SIZE];
  		SetOption(CURLOPT_ERRORBUFFER, &error_message);
		AddHeader("Accept", kContentTypeJson);
		
		SetCustomRequestOptions();
		
		SetOption(CURLOPT_HTTPHEADER, headers_.Get());

		const CURLcode result = curl_easy_perform(http_connection_);
		WEBDRIVERXX_CHECK(result == CURLE_OK, Fmt()
			<< "Cannot perform HTTP request ("
			<< "result: " << result
			<< ", message: " << error_message
			<< ")"
			);

		response.http_code = GetHttpCode();
		return response;
	}

protected:
	virtual void SetCustomRequestOptions() {}

	template<typename T>
	void SetOption(CURLoption option, const T& value) const {
		const CURLcode result = curl_easy_setopt(http_connection_, option, value);
		WEBDRIVERXX_CHECK(result == CURLE_OK, Fmt()
			<< "Cannot set HTTP session option ("
			<< "option: " << option
			<< ", message: \"" << curl_easy_strerror(result) << "\""
			<< ")"
			);
	}

	void AddHeader(const std::string& name, const std::string& value) {
		headers_.Add(name, value);
	}

private:
	long GetHttpCode() const {
		long http_code = 0;
		const CURLcode result = curl_easy_getinfo(http_connection_, CURLINFO_RESPONSE_CODE, &http_code);
		WEBDRIVERXX_CHECK(result == CURLE_OK, Fmt()
			<< "Cannot get HTTP code (" << curl_easy_strerror(result) << ")"
			);
		return http_code;
	}

	static
	size_t WriteCallback(void* buffer, size_t size, size_t nmemb, void* userdata) {
		std::string* data_received = reinterpret_cast<std::string*>(userdata);
		const size_t buffer_size = size * nmemb;
		data_received->append(reinterpret_cast<const char*>(buffer), buffer_size);
		return buffer_size;
	}

private:
	HttpRequest(HttpRequest&);
	HttpRequest& operator=(HttpRequest&);

private:
	CURL *const http_connection_;
	const std::string url_;
	HttpHeaders headers_;
};

typedef HttpRequest HttpGetRequest;

class HttpDeleteRequest : public HttpRequest {
public:
	HttpDeleteRequest(CURL* http_connection, const std::string& url)
		: HttpRequest(http_connection, url)
	{}

private:
	void SetCustomRequestOptions() {
		SetOption(CURLOPT_CUSTOMREQUEST, "DELETE");
	}
};

class HttpPostRequest : public HttpRequest {
public:
	HttpPostRequest(
		CURL* http_connection,
		const std::string& url,
		const std::string& upload_data
		)
		: HttpRequest(http_connection, url)
		, upload_data_(upload_data)
		, unsent_ptr_(upload_data.c_str())
		, unsent_length_(upload_data.size())
	{}

protected:
	void SetCustomRequestOptions() {
		SetOption(CURLOPT_POST, 1L);
		SetOption(CURLOPT_POSTFIELDSIZE, upload_data_.length());
		AddHeader("Content-Type", kContentTypeJson);
		SetOption(CURLOPT_READFUNCTION, ReadCallback);
		SetOption(CURLOPT_READDATA, this);
	}

private:
	static
	size_t ReadCallback(void* buffer, size_t size, size_t nmemb, void* userdata) {
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
	const std::string& upload_data_;
	const char* unsent_ptr_;
	size_t unsent_length_;
};

} // namespace detail
} // namespace webdriverxx

#endif
