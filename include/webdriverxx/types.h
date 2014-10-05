#ifndef WEBDRIVERXX_TYPES_H
#define WEBDRIVERXX_TYPES_H

namespace webdriverxx {

struct Size {
	int width;
	int height;
	Size() : width(0), height(0) {}
};

struct Point {
	int x;
	int y;
	Point() : x(0), y(0) {}
};

struct Cookie {
	enum {
		NoExpiry = 0
	};

	std::string name;
	std::string value;
	std::string path;
	std::string domain;
	bool secure;
	bool http_only;
	int expiry; // seconds since midnight, January 1, 1970 UTC

	Cookie() : secure(false), http_only(false), expiry(NoExpiry) {}
	Cookie(
		const std::string& name,
		const std::string& value,
		const std::string& path = std::string(),
		const std::string& domain = std::string(),
		bool secure = false,
		bool http_only = false,
		int expiry = NoExpiry
		)
		: name(name)
		, value(value)
		, path(path)
		, domain(domain)
		, secure(secure)
		, http_only(http_only)
		, expiry(expiry)
	{}

	bool operator == (const Cookie& c) const {
		return name == c.name
			&& value == c.value
			&& path == c.path
			&& domain == c.domain
			&& secure == c.secure
			&& http_only == c.http_only
			&& expiry == c.expiry
			;
	}
};

namespace timeout {
enum Type {
	ImplicitWait,
	PageLoad,
	Script,
	AsyncScript,
	ElementFind
};
} // namespace timeout

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

namespace capabilities {
const char *const BrowserName = "browserName";
// TODO
} // namespace browser

namespace detail {

struct ElementRef {
	std::string ref;
};

} // namespace detail
} // namespace webdriverxx

#endif
