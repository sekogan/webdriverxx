#ifndef WEBDRIVERXX_DETAIL_HTTP_CONNECTION_H
#define WEBDRIVERXX_DETAIL_HTTP_CONNECTION_H

#include "http_client.h"
#include "http_request.h"
#include "error_handling.h"
#include <curl/curl.h>

namespace webdriverxx {
namespace detail {

class HttpConnection : public IHttpClient
{
public:
	HttpConnection()
		: connection_(InitCurl())
	{}

	~HttpConnection()
	{
		curl_easy_cleanup(connection_);
	}

	HttpResponse Get(const std::string& url) const
	{
		return HttpGetRequest(connection_, url).Execute();
	}
	
	HttpResponse Delete(const std::string& /*url*/) const
	{
		return HttpResponse();
	}
	
	HttpResponse Post(
		const std::string& url,
		const std::string& post_data
		) const
	{
		return HttpPostRequest(connection_, url, post_data).Execute();
	}

	HttpResponse Put(
		const std::string& url,
		const std::string& post_data
		) const
	{
		return HttpPutRequest(connection_, url, post_data).Execute();
	}	

private:
	static
	CURL* InitCurl()
	{
		CURL *const result = curl_easy_init();
		if (!result)
			throw WebDriverException("Cannot initialize CURL");
		return result;
	}

private:
	HttpConnection(HttpConnection&);
	HttpConnection& operator=(HttpConnection&);

private:
	CURL *const connection_;
};

} // namespace detail
} // namespace webdriverxx

#endif
