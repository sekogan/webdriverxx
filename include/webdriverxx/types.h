#ifndef WEBDRIVERXX_TYPES_H
#define WEBDRIVERXX_TYPES_H

#include <string>

namespace webdriverxx {

typedef unsigned long long TimePoint;
typedef unsigned Duration;

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

typedef const char* Type;

Type const Implicit = "implicit";
Type const PageLoad = "page load";
Type const Script = "script";

} // namespace timeout

} // namespace webdriverxx

#endif
