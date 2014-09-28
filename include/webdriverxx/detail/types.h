#ifndef WEBDRIVERXX_DETAIL_TYPES_H
#define WEBDRIVERXX_DETAIL_TYPES_H

#include "../capabilities.h"
#include <picojson.h>
#include <string>

namespace webdriverxx {
namespace detail {

struct ElementRef {
	std::string ref;
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

} // namespace detail
} // namespace webdriverxx

#endif
