#ifndef WEBDRIVERXX_DETAIL_TYPES_H
#define WEBDRIVERXX_DETAIL_TYPES_H

#include "../conversions.h"
#include "../capabilities.h"
#include <picojson.h>
#include <string>

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
		.With("ELEMENT", ref.ref)
		.Build();
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
		result.capabilities = CapabilitiesAccess::Construct(value.get("capabilities").get<picojson::object>());
}

} // namespace detail
} // namespace webdriverxx

#endif
