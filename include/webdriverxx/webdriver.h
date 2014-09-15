#ifndef WEBDRIVERXX_WEBDRIVER_H
#define WEBDRIVERXX_WEBDRIVER_H

#include "capabilities.h"
#include "detail/webdriver_protocol.h"
#include "detail/http_connection.h"
#include <rapidjson/document.h>
#include <string>

namespace webdriverxx {

const char *const kDefaultUrl = "http://localhost:4444/wd/hub/";

template<class JsonDocument>
class WebDriverBase
{
public:
	typedef typename JsonDocument::ValueType JsonValue;

public:
	explicit WebDriverBase(
		const std::string& url = kDefaultUrl,
		const Capabilities& required = Capabilities(),
		const Capabilities& desired = Capabilities()
		)
		: protocol_(url, http_connection_)
	{}

	JsonValue GetStatus()
	{
		return protocol_.Get("status");
	}

	const JsonValue& GetLastFailedCommandDetails() const
	{
		return protocol_.GetLastFailedCommandDetails();
	}

private:
	WebDriverBase(WebDriverBase&);
	WebDriverBase& operator=(WebDriverBase&);

private:
	detail::HttpConnection http_connection_;
	detail::WebDriverProtocolBase<JsonDocument> protocol_;
};

typedef WebDriverBase<rapidjson::Document> WebDriver;

} // namespace webdriverxx

#endif
