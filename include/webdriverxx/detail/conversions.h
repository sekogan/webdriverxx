#ifndef WEBDRIVERXX_DETAIL_CONVERSIONS_H
#define WEBDRIVERXX_DETAIL_CONVERSIONS_H

#include "error_handling.h"
#include "../types.h"
#include <picojson.h>
#include <algorithm>

namespace webdriverxx {
namespace detail {

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
T FromJson(const picojson::value& value);

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
std::string FromJson<std::string>(const picojson::value& value) {
	return value.to_str();
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

} // namespace detail
} // namespace webdriverxx

#endif
