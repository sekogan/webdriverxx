#ifndef WEBDRIVERXX_ELEMENT_H
#define WEBDRIVERXX_ELEMENT_H

#include "by.h"
#include "types.h"
#include "detail/resource.h"
#include "detail/conversions.h"
#include "detail/keyboard.h"
#include <string>
#include <vector>

namespace webdriverxx {
namespace detail {

struct Finder;

} // namespace detail

class Element { // copyable
public:
	Element(
		const std::string& id,
		const detail::Resource& resource,
		const detail::Finder* finder
		);

	bool IsDisplayed() const;
	bool IsEnabled() const;
	bool IsSelected() const;
	Point GetLocation() const;
	Point GetLocationInView() const;
	Size GetSize() const;
	std::string GetAttribute(const std::string& name) const;
	std::string GetCssProperty(const std::string& name) const;
	std::string GetTagName() const;
	std::string GetText() const;

	Element FindElement(const By& by) const;
	std::vector<Element> FindElements(const By& by) const;

	const Element& Clear() const;
	const Element& Click() const;
	const Element& Submit() const;

	const Element& SendKeys(const char* keys) const;
	const Element& SendKeys(const std::string& keys) const;
	template<class IterableStringList>
	const Element& SendKeys(const IterableStringList& keys) const;

	bool Equals(const Element& other) const;
	bool operator != (const Element& other) const;
	bool operator == (const Element& other) const;

private:
	detail::Keyboard GetKeyboard() const;

private:
	std::string id_;
	detail::Resource resource_;
	const detail::Finder* finder_;
};

} // namespace webdriverxx

#include "element.inl"

#endif
