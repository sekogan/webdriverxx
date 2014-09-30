#ifndef WEBDRIVERXX_CONVERSIONS_H
#define WEBDRIVERXX_CONVERSIONS_H

#include "types.h"
#include "detail/error_handling.h"
#include <picojson.h>
#include <algorithm>

namespace webdriverxx {

template<typename T>
struct ToJsonImpl {
	static picojson::value Convert(const T& value) {
		return picojson::value(value);
	}
};

// Do not specialize or overload ToJson(). Add specializations
// for ToJsonImpl instead.
template<typename T>
inline
picojson::value ToJson(const T& value) {
	return ToJsonImpl<T>::Convert(value);
}

template<typename Item, class Iterable>
inline
picojson::value ToJsonArray(const Iterable& src) {
	picojson::value result = picojson::value(picojson::array());
	picojson::array& dst = result.get<picojson::array>();
	std::transform(src.begin(), src.end(), std::back_inserter(dst), ToJson<Item>);
	return result;
}

template<typename T>
struct ToJsonImpl< std::vector<T> > {
	static picojson::value Convert(const std::vector<T>& value) {
		return ToJsonArray<T>(value);
	}
};

template<>
struct ToJsonImpl<picojson::value> {
	static picojson::value Convert(const picojson::value& value) {
		return value;
	}
};

template<>
struct ToJsonImpl<int> {
	static picojson::value Convert(int value) {
		return picojson::value(static_cast<double>(value));
	}
};

template<>
struct ToJsonImpl<unsigned> {
	static picojson::value Convert(unsigned value) {
		return picojson::value(static_cast<double>(value));
	}
};

class JsonObject { // copyable
public:
	JsonObject()
		: value_(picojson::object()) {}

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
struct FromJsonImpl {
	static T Convert(const picojson::value& value) {
		return value.get<T>();
	}
};

// Do not specialize or overload FromJson(). Add specializations
// for FromJsonImpl instead.
template<typename T>
inline
T FromJson(const picojson::value& value) {
	return FromJsonImpl<T>::Convert(value);
}

template<typename T>
inline
std::vector<T> FromJsonArray(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<picojson::array>(), "Value is not an array");
	const picojson::array& array = value.get<picojson::array>();
	std::vector<T> result;
	result.reserve(array.size());
	std::transform(array.begin(), array.end(), std::back_inserter(result), FromJson<T>);
	return result;
}

template<>
struct FromJsonImpl<std::string> {
	static std::string Convert(const picojson::value& value) {
		return value.to_str();
	}
};

template<>
struct FromJsonImpl<bool> {
	static bool Convert(const picojson::value& value) {
		return value.evaluate_as_boolean();
	}
};

template<>
struct FromJsonImpl<int> {
	static int Convert(const picojson::value& value) {
		WEBDRIVERXX_CHECK(value.is<double>(), "Value is not a number");
		return static_cast<int>(value.get<double>());
	}
};

template<>
struct FromJsonImpl<unsigned> {
	static unsigned Convert(const picojson::value& value) {
		WEBDRIVERXX_CHECK(value.is<double>(), "Value is not a number");
		return static_cast<unsigned>(value.get<double>());
	}
};

template<typename T>
struct FromJsonImpl< std::vector<T> > {
	static std::vector<T> Convert(const picojson::value& value) {
		return FromJsonArray<T>(value);
	}
};

template<typename T>
inline
void OptionalFromJson(const picojson::value& json, T& value) {
	if (!json.is<picojson::null>())
		value = FromJson<T>(json);
}

///////////////////////////////////////////////////////////////////

template<>
struct ToJsonImpl<Size> {
	static picojson::value Convert(const Size& size) {
		return JsonObject()
			.With("width", size.width)
			.With("height", size.height)
			.Build();
	}
};

template<>
struct FromJsonImpl<Size> {
	static Size Convert(const picojson::value& value) {
		WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Size is not an object");
		Size result;
		result.width = FromJson<int>(value.get("width"));
		result.height = FromJson<int>(value.get("height"));
		return result;
	}
};

template<>
struct ToJsonImpl<Point> {
	static picojson::value Convert(const Point& position) {
		return JsonObject()
			.With("x", position.x)
			.With("y", position.y)
			.Build();
	}
};

template<>
struct FromJsonImpl<Point> {
	static Point Convert(const picojson::value& value) {
		WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Point is not an object");
		Point result;
		result.x = FromJson<int>(value.get("x"));
		result.y = FromJson<int>(value.get("y"));
		return result;
	}
};

template<>
struct ToJsonImpl<Cookie> {
	static picojson::value Convert(const Cookie& cookie) {
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
};

template<>
struct FromJsonImpl<Cookie> {
	static Cookie Convert(const picojson::value& value) {
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
};

} // namespace webdriverxx

#endif
