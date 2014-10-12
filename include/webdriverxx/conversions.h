#ifndef WEBDRIVERXX_CONVERSIONS_H
#define WEBDRIVERXX_CONVERSIONS_H

#include "types.h"
#include "detail/error_handling.h"
#include "detail/meta.h"
#include <picojson.h>
#include <algorithm>
#include <iterator>

namespace webdriverxx {

template<typename T>
picojson::value ToJson(const T& value);

inline
picojson::value ToJson(const char* value) {
	return picojson::value(value);
}

inline
picojson::value ToJson(char* value) {
	return ToJson(static_cast<const char*>(value));
}

inline
picojson::value ToJson(const std::string& value) {
	return ToJson(value.c_str());
}

inline
picojson::value ToJson(const picojson::value& value) {
	return value;
}

inline
picojson::value ToJson(const picojson::object& value) {
	return picojson::value(value);
}

inline
picojson::value ToJson(int value) {
	return picojson::value(static_cast<double>(value));
}

namespace detail2 { // Should be different from detail to prevent detail::ToJson overloads from hiding webdriverxx::ToJson

struct ToJsonDefaultFilter {
	template<typename T>
	static picojson::value Apply(const T& value) {
		return picojson::value(value);
	}
};

template<typename NextFilter>
struct ToJsonContainerFilter {
	template<typename T>
	static picojson::value Apply(const T& value) {
		return Impl(value, 0);
	}

private:
	template<typename T>
	static picojson::value Impl(const T& value, decltype(&*std::begin(detail::ValueRef<T>()))) {
		typedef typename std::iterator_traits<decltype(std::begin(value))>::value_type Item;
		picojson::value result = picojson::value(picojson::array());
		picojson::array& dst = result.get<picojson::array>();
		std::transform(std::begin(value), std::end(value), std::back_inserter(dst), [](const Item& item) {
			return ToJson(item);
		});
		return result;
	}

	template<typename T>
	static picojson::value Impl(const T& value, ...) {
		return NextFilter::template Apply(value);
	}
};

} // detail2

template<typename T>
inline
picojson::value ToJson(const T& value) {
	namespace detail = ::webdriverxx::detail2;
	return
		detail::ToJsonContainerFilter<
		detail::ToJsonDefaultFilter
		>::template Apply(value);
}

class JsonObject { // copyable
public:
	JsonObject() : value_(picojson::object()) {}

	template<typename T>
	JsonObject& With(const std::string& name, const T& value) {
		value_.get<picojson::object>()[name] = ToJson(value);
		return *this;
	}

	const picojson::value& Build() const {
		return value_;
	}

private:
	picojson::value value_;
};

///////////////////////////////////////////////////////////////////

template<typename T>
T FromJson(const picojson::value& value);

namespace detail {

struct FromJsonDefaultFilter {
	template<typename T>
	static T Apply(const picojson::value& value) {
		return value.get<T>();
	}
};

template<typename NextFilter>
struct FromJsonContainerFilter {
	template<typename T>
	static T Apply(const picojson::value& value) {
		return Impl<T>(value, 0);
	}

private:
	template<typename T>
	static T Impl(const picojson::value& value, decltype(&*std::begin(detail::ValueRef<T>()))) {
		WEBDRIVERXX_CHECK(value.is<picojson::array>(), "Value is not an array");
		const picojson::array& array = value.get<picojson::array>();
		T result;
		typedef typename std::iterator_traits<decltype(std::begin(result))>::value_type Item;
		std::transform(array.begin(), array.end(), std::back_inserter(result), FromJson<Item>);
		return result;
	}

	template<typename T>
	static T Impl(const picojson::value& value, ...) {
		return NextFilter::template Apply<T>(value);
	}
};

} // detail

template<typename T>
inline
T FromJson(const picojson::value& value) {
	namespace detail = ::webdriverxx::detail;
	return
		detail::FromJsonContainerFilter<
		detail::FromJsonDefaultFilter
		>::template Apply<T>(value);
}

template<>
inline
std::string FromJson<std::string>(const picojson::value& value) {
	return value.to_str();
}

template<>
inline
bool FromJson<bool>(const picojson::value& value) {
	return value.evaluate_as_boolean();
}

template<>
inline
int FromJson<int>(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<double>(), "Value is not a number");
	return static_cast<int>(value.get<double>());
}

template<>
inline
unsigned FromJson<unsigned>(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<double>(), "Value is not a number");
	return static_cast<unsigned>(value.get<double>());
}

template<typename T>
inline
void OptionalFromJson(const picojson::value& json, T& value) {
	if (!json.is<picojson::null>())
		value = FromJson<T>(json);
}

///////////////////////////////////////////////////////////////////

inline
picojson::value ToJson(const Size& size) {
	return JsonObject()
		.With("width", size.width)
		.With("height", size.height)
		.Build();
}

template<>
inline
Size FromJson<Size>(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Size is not an object");
	Size result;
	result.width = FromJson<int>(value.get("width"));
	result.height = FromJson<int>(value.get("height"));
	return result;
}

inline
picojson::value ToJson(const Point& position) {
	return JsonObject()
		.With("x", position.x)
		.With("y", position.y)
		.Build();
}

template<>
inline
Point FromJson<Point>(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Point is not an object");
	Point result;
	result.x = FromJson<int>(value.get("x"));
	result.y = FromJson<int>(value.get("y"));
	return result;
}

inline
picojson::value ToJson(const Cookie& cookie) {
	JsonObject result;
	result.With("name", cookie.name);
	result.With("value", cookie.value);
	if (!cookie.path.empty()) result.With("path", cookie.path);
	if (!cookie.domain.empty()) result.With("domain", cookie.domain);
	if (cookie.secure) result.With("secure", true);
	if (cookie.http_only) result.With("httpOnly", true);
	if (cookie.expiry != Cookie::NoExpiry) result.With("expiry", cookie.expiry);
	return result.Build();
}

template<>
inline
Cookie FromJson<Cookie>(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Cookie is not an object");
	Cookie result;
	result.name = FromJson<std::string>(value.get("name"));
	result.value = FromJson<std::string>(value.get("value"));
	OptionalFromJson(value.get("path"), result.path);
	OptionalFromJson(value.get("domain"), result.domain);
	OptionalFromJson(value.get("secure"), result.secure);
	OptionalFromJson(value.get("httpOnly"), result.http_only);
	OptionalFromJson(value.get("expiry"), result.expiry);
	return result;
}

} // namespace webdriverxx

#endif
