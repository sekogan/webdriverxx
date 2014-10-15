#ifndef WEBDRIVERXX_CAPABILITIES_H
#define WEBDRIVERXX_CAPABILITIES_H

#include "conversions.h"
#include <picojson.h>
#include <string>

namespace webdriverxx {

// List of keys and values indicating features that server can or should provide.
class Capabilities : public JsonObject { // copyable
public:
	explicit Capabilities(const picojson::object& object)
		: JsonObject(object)
	{}
	Capabilities() {}

#define WEBDRIVERXX_CAPABILITY(name, id, type) \
	type Get##name() const { return GetOptional<type>(#id); } \
	void Set##name(const type& value) { Set(#id, value); }

	WEBDRIVERXX_CAPABILITY(BrowserName, browserName, std::string)

#undef WEBDRIVERXX_CAPABILITY
};

namespace browser {
const char *const Android = "android";
const char *const Chrome = "chrome";
const char *const GC = Chrome;
const char *const GoogleChrome = "googlechrome";
const char *const Firefox = "firefox";
const char *const FF = Firefox;
const char *const Firefox2 = "firefox2";
const char *const Firefox3 = "firefox3";
const char *const FirefoxChrome = "firefoxchrome";
const char *const FirefoxProxy = "firefoxproxy";
const char *const HtmlUnit = "htmlunit";
const char *const IE = "internet explorer";
const char *const IE_HTA="iehta";
const char *const IExplore = "iexplore";
const char *const IExploreProxy= "iexploreproxy";
const char *const IPad = "iPad";
const char *const IPhone = "iPhone";
const char *const Mock = "mock";
const char *const Opera = "opera";
const char *const PhantomJS = "phantomjs";
const char *const Phantom = PhantomJS;
const char *const Safari = "safari";
const char *const SafariProxy = "safariproxy";
} // namespace browser

} // namespace webdriverxx

#endif
