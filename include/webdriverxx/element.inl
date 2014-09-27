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
bool Element::IsDisplayed() const {
	return resource_.GetBool("displayed");
}

inline
bool Element::IsEnabled() const {
	return resource_.GetBool("enabled");
}

inline
bool Element::IsSelected() const {
	return resource_.GetBool("selected");
}

inline
Point Element::GetLocation() const {
	return resource_.GetValue<Point>("location");
}

inline
Point Element::GetLocationInView() const {
	return resource_.GetValue<Point>("location_in_view");
}

inline
Size Element::GetSize() const {
	return resource_.GetValue<Size>("size");
}

inline
std::string Element::GetAttribute(const std::string& name) const {
	return resource_.GetString(std::string("attribute/") + name);
}

inline
std::string Element::GetCssProperty(const std::string& name) const {
	return resource_.GetString(std::string("css/") + name);
}

inline
std::string Element::GetTagName() const {
	return resource_.GetString("name");
}
inline
std::string Element::GetText() const {
	return resource_.GetString("text");
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
const Element& Element::Clear() const {
	resource_.Post("clear");
	return *this;
}

inline
const Element& Element::Click() const {
	resource_.Post("click");
	return *this;
}

inline
const Element& Element::Submit() const {
	resource_.Post("submit");
	return *this;
}

inline
const Element& Element::SendKeys(const char* keys) const {
	GetKeyboard().SendKeys(keys);
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
	return resource_.GetBool(std::string("equals/") + other.id_);
}

inline
bool Element::operator != (const Element& other) const {
	return !Equals(other);
}

inline
bool Element::operator == (const Element& other) const {
	return Equals(other);
}

inline
detail::Keyboard Element::GetKeyboard() const
{
	return detail::Keyboard(resource_, "value");
}

} // namespace webdriverxx
