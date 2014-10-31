#include "conversions.h"
#include "detail/finder.h"
#include "detail/error_handling.h"

namespace webdriverxx {

inline
picojson::value CustomToJson(const Element& element) {
	detail::ElementRef ref = { element.GetRef() };
	return ToJson(ref);
}

inline
Element::Element() {}

inline
Element::Element(
	const std::string& ref,
	const detail::Shared<detail::Resource>& resource,
	const detail::Shared<detail::IFinderFactory>& factory
	)
	: ref_(ref)
	, resource_(resource)
	, factory_(factory)
{}

inline
std::string Element::GetRef() const {
	return ref_;
}

inline
bool Element::IsDisplayed() const {
	return GetResource().GetBool("displayed");
}

inline
bool Element::IsEnabled() const {
	return GetResource().GetBool("enabled");
}

inline
bool Element::IsSelected() const {
	return GetResource().GetBool("selected");
}

inline
Point Element::GetLocation() const {
	return GetResource().GetValue<Point>("location");
}

inline
Point Element::GetLocationInView() const {
	return GetResource().GetValue<Point>("location_in_view");
}

inline
Size Element::GetSize() const {
	return GetResource().GetValue<Size>("size");
}

inline
std::string Element::GetAttribute(const std::string& name) const {
	return GetResource().GetString(std::string("attribute/") + name);
}

inline
std::string Element::GetCssProperty(const std::string& name) const {
	return GetResource().GetString(std::string("css/") + name);
}

inline
std::string Element::GetTagName() const {
	return GetResource().GetString("name");
}
inline
std::string Element::GetText() const {
	return GetResource().GetString("text");
}

inline
Element Element::FindElement(const By& by) const {
	return factory_->MakeFinder(&GetResource()).FindElement(by);
}

inline
std::vector<Element> Element::FindElements(const By& by) const {
	return factory_->MakeFinder(&GetResource()).FindElements(by);
}

inline
const Element& Element::Clear() const {
	GetResource().Post("clear");
	return *this;
}

inline
const Element& Element::Click() const {
	GetResource().Post("click");
	return *this;
}

inline
const Element& Element::Submit() const {
	GetResource().Post("submit");
	return *this;
}

inline
const Element& Element::SendKeys(const std::string& keys) const {
	GetKeyboard().SendKeys(keys);
	return *this;
}

inline
const Element& Element::SendKeys(const Shortcut& shortcut) const {
	GetKeyboard().SendKeys(shortcut);
	return *this;
}

inline
bool Element::Equals(const Element& other) const {
	return GetResource().GetBool(std::string("equals/") + other.ref_);
}

inline
bool Element::operator != (const Element& other) const {
	return ref_ != other.ref_;
}

inline
bool Element::operator == (const Element& other) const {
	return ref_ == other.ref_;
}

inline
bool Element::operator < (const Element& other) const {
	return ref_ < other.ref_;
}

inline
detail::Resource& Element::GetResource() const {
	WEBDRIVERXX_CHECK(resource_, "Attempt to use empty element");
	return *resource_;
}

inline
detail::Keyboard Element::GetKeyboard() const
{
	return detail::Keyboard(&GetResource(), "value");
}

} // namespace webdriverxx
