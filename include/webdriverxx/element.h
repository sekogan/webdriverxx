#ifndef WEBDRIVERXX_ELEMENT_H
#define WEBDRIVERXX_ELEMENT_H

#include "by.h"
#include "detail/resource.h"
#include "detail/conversions.h"
#include <string>
#include <vector>

namespace webdriverxx {
namespace detail {

class Finder;

} // namespace detail

class Element { // copyable
public:
	Element(
		const std::string& id,
		const detail::Resource& resource,
		const detail::Finder* finder
		);

	bool operator != (const Element&) const;
	Element FindElement(const By& by) const;
	std::vector<Element> FindElements(const By& by) const;

private:
	std::string id_;
	detail::Resource resource_;
	const detail::Finder* finder_;
};

} // namespace webdriverxx

#include "element.inl"

#endif
