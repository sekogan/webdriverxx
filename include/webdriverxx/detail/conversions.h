#ifndef WEBDRIVERXX_DETAIL_CONVERSIONS_H
#define WEBDRIVERXX_DETAIL_CONVERSIONS_H

#include "error_handling.h"
#include "../types.h"
#include <picojson.h>
#include <algorithm>

namespace webdriverxx {
namespace detail {

template<typename T>
inline
T FromJson(const picojson::value& value);

template<>
inline
SessionInformation FromJson<SessionInformation>(const picojson::value& value)
{
	detail::Check(value.is<picojson::object>(), "Session information is not an object");
	SessionInformation result;
	result.id = value.get("sessionId").to_str();
	if (value.get("capabilities").is<picojson::object>())
		result.capabilities = Capabilities(value.get("capabilities").get<picojson::object>());
	return result;
}

template<>
inline
SessionsInformation FromJson<SessionsInformation>(const picojson::value& value)
{
	detail::Check(value.is<picojson::array>(), "Sessions information is not an array");
	const picojson::array& array = value.get<picojson::array>();
	SessionsInformation result;
	result.reserve(array.size());
	std::transform(array.begin(), array.end(), std::back_inserter(result),
		FromJson<SessionInformation>);
	return result;
}

} // namespace detail
} // namespace webdriverxx

#endif
