#ifndef WEBDRIVERXX_ELEMENT_H
#define WEBDRIVERXX_ELEMENT_H

#include "detail/resource.h"
#include "detail/conversions.h"
#include <string>

namespace webdriverxx {

class Element { // copyable
public:
	explicit Element(const std::string& id, const detail::Resource& resource)
		: id_(id)
		, resource_(resource) {}

private:
	std::string id_;
	detail::Resource resource_;
};

} // namespace webdriverxx

#endif
