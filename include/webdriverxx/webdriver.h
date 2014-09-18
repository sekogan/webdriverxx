#ifndef WEBDRIVERXX_WEBDRIVER_H
#define WEBDRIVERXX_WEBDRIVER_H

#include "capabilities.h"
#include "detail/webdriver_protocol.h"
#include "detail/http_connection.h"
#include <string>

namespace webdriverxx {

const char *const kDefaultUrl = "http://localhost:4444/wd/hub/";

class WebDriver
{
public:
	explicit WebDriver(const std::string& url = kDefaultUrl)
		: root_(&http_connection_, url)
	{}

	picojson::value GetStatus() const
	{
		return root_.Get("status").get("value");
	}

private:
	WebDriver(WebDriver&);
	WebDriver& operator=(WebDriver&);

private:
	const detail::HttpConnection http_connection_;
	const detail::WebDriverProtocol root_;
};

} // namespace webdriverxx

#endif
