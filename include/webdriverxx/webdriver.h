#ifndef WEBDRIVERXX_WEBDRIVER_H
#define WEBDRIVERXX_WEBDRIVER_H

#include "capabilities.h"
#include "detail/session.h"
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
		: session_(http_connection_, url, required, desired)
	{}

	const JsonValue& GetStatus()
	{
		return session_.Get("status");
	}

	const JsonValue& GetLastJsonResponse() const
	{
		return session_.GetLastJsonResponse();
	}

private:
	WebDriverBase(WebDriverBase&);
	WebDriverBase& operator=(WebDriverBase&);

private:
	detail::HttpConnection http_connection_;
	detail::SessionBase<JsonDocument> session_;
};

typedef WebDriverBase<rapidjson::Document> WebDriver;

} // namespace webdriverxx

#endif
