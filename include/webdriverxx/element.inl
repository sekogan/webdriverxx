#include "detail/finder.h"
#include "detail/error_handling.h"

namespace webdriverxx {

inline
Element::Element(
	const std::string& id,
	const detail::Resource& resource,
	const detail::Finder* finder
	)
	: id_(id)
	, resource_(resource)
	, finder_(finder) {}

inline
bool Element::operator != (const Element& e) const {
	return id_ != e.id_;
}

inline
Element Element::FindElement(const By& by) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return finder_->FindElement(by, resource_);
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "strategy: " << by.GetStrategy()
		<< ", value: " << by.GetValue()
		)
}

inline
std::vector<Element> Element::FindElements(const By& by) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return finder_->FindElements(by, resource_);
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "strategy: " << by.GetStrategy()
		<< ", value: " << by.GetValue()
		)
}

} // namespace webdriverxx
