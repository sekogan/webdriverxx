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

namespace detail {

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

} // detail

inline
picojson::value CustomToJson(const char* value) {
	return picojson::value(value);
}

inline
picojson::value CustomToJson(char* value) {
	return ToJson(static_cast<const char*>(value));
}

inline
picojson::value CustomToJson(const std::string& value) {
	return ToJson(value.c_str());
}

inline
picojson::value CustomToJson(const picojson::value& value) {
	return value;
}

inline
picojson::value CustomToJson(const picojson::object& value) {
	return picojson::value(value);
}

inline
picojson::value CustomToJson(int value) {
	return picojson::value(static_cast<double>(value));
}

template<typename T>
inline
picojson::value CustomToJson(const T& value) {
	namespace detail = ::webdriverxx::detail;
	return
		detail::ToJsonContainerFilter<
		detail::ToJsonDefaultFilter
		>::template Apply(value);
}

template<typename T>
inline
picojson::value ToJson(const T& value) {
	return CustomToJson(value);
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
	static void Apply(const picojson::value& value, T& result) {
		result = value.get<T>();
	}
};

template<typename NextFilter>
struct FromJsonContainerFilter {
	template<typename T>
	static void Apply(const picojson::value& value, T& result) {
		Impl<T>(value, result, 0);
	}

private:
	template<typename T>
	static void Impl(const picojson::value& value, T& result, decltype(&*std::begin(detail::ValueRef<T>()))) {
		WEBDRIVERXX_CHECK(value.is<picojson::array>(), "Value is not an array");
		const picojson::array& array = value.get<picojson::array>();
		typedef typename std::iterator_traits<decltype(std::begin(result))>::value_type Item;
		std::transform(array.begin(), array.end(), std::back_inserter(result), FromJson<Item>);
	}

	template<typename T>
	static void Impl(const picojson::value& value, T& result, ...) {
		return NextFilter::template Apply(value, result);
	}
};

} // detail

inline
void CustomFromJson(const picojson::value& value, std::string& result) {
	result = value.to_str();
}

inline
void CustomFromJson(const picojson::value& value, bool& result) {
	result = value.evaluate_as_boolean();
}

inline
void CustomFromJson(const picojson::value& value, int& result) {
	WEBDRIVERXX_CHECK(value.is<double>(), "Value is not a number");
	result = static_cast<int>(value.get<double>());
}

inline
void CustomFromJson(const picojson::value& value, unsigned& result) {
	WEBDRIVERXX_CHECK(value.is<double>(), "Value is not a number");
	result = static_cast<unsigned>(value.get<double>());
}

template<typename T>
inline
void CustomFromJson(const picojson::value& value, T& result) {
	namespace detail = ::webdriverxx::detail;
	detail::FromJsonContainerFilter<
	detail::FromJsonDefaultFilter
	>::template Apply(value, result);
}

template<typename T>
inline
T FromJson(const picojson::value& value) {
	T result;
	CustomFromJson(value, result);
	return result;
}

template<typename T>
inline
T OptionalFromJson(const picojson::value& value, const T& default_value = T()) {
	return value.is<picojson::null>() ? default_value : FromJson<T>(value);
}

///////////////////////////////////////////////////////////////////

inline
picojson::value CustomToJson(const Size& size) {
	return JsonObject()
		.With("width", size.width)
		.With("height", size.height)
		.Build();
}

inline
void CustomFromJson(const picojson::value& value, Size& result) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Size is not an object");
	result.width = FromJson<int>(value.get("width"));
	result.height = FromJson<int>(value.get("height"));
}

inline
picojson::value CustomToJson(const Point& position) {
	return JsonObject()
		.With("x", position.x)
		.With("y", position.y)
		.Build();
}

inline
void CustomFromJson(const picojson::value& value, Point& result) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Point is not an object");
	result.x = FromJson<int>(value.get("x"));
	result.y = FromJson<int>(value.get("y"));
}

inline
picojson::value CustomToJson(const Cookie& cookie) {
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

inline
void CustomFromJson(const picojson::value& value, Cookie& result) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Cookie is not an object");
	result.name = FromJson<std::string>(value.get("name"));
	result.value = FromJson<std::string>(value.get("value"));
	result.path = OptionalFromJson<std::string>(value.get("path"));
	result.domain = OptionalFromJson<std::string>(value.get("domain"));
	result.secure = OptionalFromJson<bool>(value.get("secure"), false);
	result.http_only = OptionalFromJson<bool>(value.get("httpOnly"), false);
	result.expiry = OptionalFromJson<int>(value.get("expiry"), Cookie::NoExpiry);
}

} // namespace webdriverxx

#endif
