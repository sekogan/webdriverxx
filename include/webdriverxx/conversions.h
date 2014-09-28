#ifndef WEBDRIVERXX_CONVERSIONS_H
#define WEBDRIVERXX_CONVERSIONS_H

#include "types.h"
#include "detail/types.h"
#include "detail/error_handling.h"
#include <picojson.h>
#include <algorithm>

namespace webdriverxx {

template<typename T>
struct ToJsonImpl {
	static picojson::value ToJson(const T& value) {
		return picojson::value(value);
	}
};

// Do not specialize or overload ToJson(). Add specializations
// for ToJsonImpl instead.
template<typename T>
inline
picojson::value ToJson(const T& value) {
	return ToJsonImpl<T>::ToJson(value);
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
	static picojson::value ToJson(const std::vector<T>& value) {
		return ToJsonArray<T>(value);
	}
};

template<>
struct ToJsonImpl<picojson::value> {
	static picojson::value ToJson(const picojson::value& value) {
		return value;
	}
};

template<>
struct ToJsonImpl<int> {
	static picojson::value ToJson(int value) {
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
inline
T FromJson(const picojson::value& value);

template<>
inline
std::string FromJson(const picojson::value& value) {
	return value.to_str();
}

template<>
inline
bool FromJson(const picojson::value& value) {
	return value.evaluate_as_boolean();
}

template<>
inline
int FromJson(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<double>(), "Value is not a number");
	return static_cast<int>(value.get<double>());
}

template<>
inline
SessionInformation FromJson(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Session information is not an object");
	SessionInformation result;
	result.id = value.get("sessionId").to_str();
	if (value.get("capabilities").is<picojson::object>())
		result.capabilities = detail::CapabilitiesAccess::Construct(value.get("capabilities").get<picojson::object>());
	return result;
}

template<>
inline
Size FromJson(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Size is not an object");
	Size result;
	result.width = FromJson<int>(value.get("width"));
	result.height = FromJson<int>(value.get("height"));
	return result;
}

template<>
struct ToJsonImpl<Size> {
	static picojson::value ToJson(const Size& size) {
		return JsonObject()
			.With("width", size.width)
			.With("height", size.height)
			.Build();
	}
};

template<>
inline
Point FromJson(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Point is not an object");
	Point result;
	result.x = FromJson<int>(value.get("x"));
	result.y = FromJson<int>(value.get("y"));
	return result;
}

template<>
struct ToJsonImpl<Point> {
	static picojson::value ToJson(const Point& position) {
		return JsonObject()
			.With("x", position.x)
			.With("y", position.y)
			.Build();
	}
};

template<>
inline
detail::ElementRef FromJson(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "ElementRef is not an object");
	detail::ElementRef result;
	result.ref = FromJson<std::string>(value.get("ELEMENT"));
	return result;
}

template<>
struct ToJsonImpl<detail::ElementRef> {
	static picojson::value ToJson(const detail::ElementRef& ref) {
		return JsonObject()
			.With("ELEMENT", ref.ref)
			.Build();
	}
};

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

} // namespace webdriverxx

#endif
