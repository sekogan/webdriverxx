#ifndef WEBDRIVERXX_DETAIL_CONVERSIONS_H
#define WEBDRIVERXX_DETAIL_CONVERSIONS_H

#include "error_handling.h"
#include "../types.h"
#include <picojson.h>
#include <algorithm>

namespace webdriverxx {
namespace detail {

struct ElementId {
	std::string id;
};

class JsonObject { // copyable
public:
	JsonObject()
		: value_(picojson::object()) {}

	JsonObject& With(const std::string& name, const picojson::value& value) {
		value_.get<picojson::object>()[name] = value;
		return *this;
	}

	JsonObject& With(const std::string& name, int value) {
		value_.get<picojson::object>()[name] = picojson::value(static_cast<double>(value));
		return *this;
	}

	template<typename T>
	JsonObject& With(const std::string& name, const T& value) {
		value_.get<picojson::object>()[name] = picojson::value(value);
		return *this;
	}

	const picojson::value& Build() const {
		return value_;
	}

private:
	picojson::value value_;
};

struct CapabilitiesAccess {
	static
	Capabilities Construct(const picojson::object& object) {
		return Capabilities(object);
	}

	static
	const picojson::object& GetJsonObject(const Capabilities& capabilities) {
		return capabilities.GetJsonObject();
	}
};

template<typename T>
inline
picojson::value ToJson(const T& value) {
	return picojson::value(value);
}

template<typename T>
inline
T FromJson(const picojson::value& value);

template<>
inline
std::string FromJson<std::string>(const picojson::value& value) {
	return value.to_str();
}

template<>
inline
int FromJson<int>(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<double>(), "Value is not a number");
	return value.get<double>();
}

template<>
inline
SessionInformation FromJson<SessionInformation>(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Session information is not an object");
	SessionInformation result;
	result.id = value.get("sessionId").to_str();
	if (value.get("capabilities").is<picojson::object>())
		result.capabilities = CapabilitiesAccess::Construct(value.get("capabilities").get<picojson::object>());
	return result;
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
picojson::value ToJson(const Size& size)
{
	return JsonObject()
		.With("width", size.width)
		.With("height", size.height)
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
picojson::value ToJson(const Point& position)
{
	return JsonObject()
		.With("x", position.x)
		.With("y", position.y)
		.Build();
}

template<>
inline
ElementId FromJson<ElementId>(const picojson::value& value) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Element is not an object");
	WEBDRIVERXX_CHECK(value.get("ELEMENT").is<std::string>(), "Element ID is not a string");
	ElementId result;
	result.id = value.get("ELEMENT").to_str();
	return result;
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

template<class Iterable>
inline
picojson::value ToJsonArray(const Iterable& src) {
	picojson::value result = picojson::value(picojson::array());
	picojson::array& dst = result.get<picojson::array>();
	std::transform(src.begin(), src.end(), std::back_inserter(dst), ToJson<std::string>);
	return result;
}

} // namespace detail
} // namespace webdriverxx

#endif
