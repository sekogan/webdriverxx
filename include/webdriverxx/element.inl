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
bool Element::operator != (const Element& other) const {
	return !Equals(other);
}

inline
bool Element::operator == (const Element& other) const {
	return Equals(other);
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

inline
const Element& Element::Click() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	resource_.Post("click");
	return *this;
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
const Element& Element::Submit() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	resource_.Post("submit");
	return *this;
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
std::string Element::GetText() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return resource_.Get("text").to_str();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
const Element& Element::SendKeys(const char* keys) const {
	return SendKeys(std::string(keys));
}

inline
const Element& Element::SendKeys(const std::string& keys) const {
	std::vector<std::string> list(1, keys);
	return SendKeys(list);
}

template<class IterableStringList>
inline
const Element& Element::SendKeys(const IterableStringList& keys) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	resource_.Post("value", detail::JsonObject()
		.With("value", detail::ToJsonArray(keys)).Build());
	return *this;
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
std::string Element::GetTagName() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return resource_.Get("name").to_str();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
const Element& Element::Clear() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	resource_.Post("clear");
	return *this;
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
bool Element::IsSelected() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return resource_.Get("selected").evaluate_as_boolean();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
bool Element::IsEnabled() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return resource_.Get("enabled").evaluate_as_boolean();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
bool Element::Equals(const Element& other) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return resource_.Get(std::string("equals/") + other.id_).evaluate_as_boolean();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
std::string Element::GetAttribute(const std::string& name) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return resource_.Get(std::string("attribute/") + name).to_str();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
bool Element::IsDisplayed() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return resource_.Get("displayed").evaluate_as_boolean();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
Point Element::GetLocation() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return detail::FromJson<Point>(resource_.Get("location"));
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
Point Element::GetLocationInView() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return detail::FromJson<Point>(resource_.Get("location_in_view"));
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
Size Element::GetSize() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return detail::FromJson<Size>(resource_.Get("size"));
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
std::string Element::GetCssProperty(const std::string& name) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return resource_.Get(std::string("css/") + name).to_str();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

} // namespace webdriverxx
