#ifndef WEBDRIVERXX_DETAIL_HTTP_CLIENT_H
#define WEBDRIVERXX_DETAIL_HTTP_CLIENT_H

#include <string>

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

struct IHttpClient
{
	virtual HttpResponse Get(const std::string& url) const = 0;
	virtual HttpResponse Delete(const std::string& url) const = 0;
	virtual HttpResponse Post(
		const std::string& url,
		const char* content_type,
		const std::string& post_data
		) const = 0;
	virtual HttpResponse Put(
		const std::string& url,
		const char* content_type,
		const std::string& post_data
		) const = 0;
	virtual ~IHttpClient() {}
};

} // namespace detail
} // namespace webdriverxx

#endif
