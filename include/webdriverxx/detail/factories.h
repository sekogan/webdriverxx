#ifndef WEBDRIVERXX_DETAIL_FACTORIES_H
#define WEBDRIVERXX_DETAIL_FACTORIES_H

#include "shared.h"
#include <string>

namespace webdriverxx {

class Element;

namespace detail {

class Finder;
class Resource;

struct IFinderFactory {
	virtual Finder MakeFinder(const Shared<Resource>& context) = 0;
	virtual ~IFinderFactory() {}

};

struct IElementFactory {
	virtual Element MakeElement(const std::string& id) = 0;
	virtual ~IElementFactory() {}
};

} // namespace detail
} // namespace webdriverxx

#endif
