#ifndef WEBDRIVERXX_DETAIL_TYPES_H
#define WEBDRIVERXX_DETAIL_TYPES_H

#include "../conversions.h"
#include "../capabilities.h"
#include <picojson.h>
#include <string>

namespace webdriverxx {
namespace detail {

struct SessionRef {
	std::string id;
	Capabilities capabilities;
};

struct ElementRef {
	std::string ref;
};

inline
picojson::value CustomToJson(const ElementRef& ref) {
	return JsonObject()
		.Set("ELEMENT", ref.ref)
		;
}

inline
void CustomFromJson(const picojson::value& value, ElementRef& result) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "ElementRef is not an object");
	result.ref = FromJson<std::string>(value.get("ELEMENT"));
}

inline
void CustomFromJson(const picojson::value& value, SessionRef& result) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Session information is not an object");
	result.id = value.get("sessionId").to_str();
	if (value.get("capabilities").is<picojson::object>())
		result.capabilities = Capabilities(value.get("capabilities").get<picojson::object>());
}

} // namespace detail
} // namespace webdriverxx

#endif
