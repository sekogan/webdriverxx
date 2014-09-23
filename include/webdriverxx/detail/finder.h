#ifndef WEBDRIVERXX_DETAIL_FINDER_H
#define WEBDRIVERXX_DETAIL_FINDER_H

#include "resource.h"
#include "../element.h"
#include <vector>

namespace webdriverxx {

class By;

namespace detail {

struct Finder {
	virtual Element FindElement(
		const By& by,
		const Resource& context
		) const = 0;
	virtual std::vector<Element> FindElements(
		const By& by,
		const Resource& context
		) const = 0;
protected:
	~Finder() {}
};

} // namespace detail
} // namespace webdriverxx

#endif
