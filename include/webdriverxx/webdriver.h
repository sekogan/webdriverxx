#ifndef WEBDRIVERXX_WEBDRIVER_H
#define WEBDRIVERXX_WEBDRIVER_H

#include "capabilities.h"
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
	{}

	JsonValue GetStatus() const
	{
		JsonDocument d;
		JsonValue v;
		v.Swap(d);
		return v;
	}

private:
	WebDriverBase(WebDriverBase&);
	WebDriverBase& operator=(WebDriverBase&);
};

typedef WebDriverBase<rapidjson::Document> WebDriver;

} // namespace webdriverxx

#endif
