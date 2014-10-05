#include "../element.h"
#include "../conversions.h"

namespace webdriverxx {
namespace detail {

inline
Finder::Finder(
	const Shared<Resource>& context,
	const Shared<IElementFactory>& factory
	)
	: context_(context)
	, factory_(factory)
{}

inline
Element Finder::FindElement(const By& by) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return factory_->MakeElement(FromJson<detail::ElementRef>(
		context_->Post("element", JsonObject()
			.With("using", by.GetStrategy())
			.With("value", by.GetValue())
			.Build()
		)).ref);
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "context: " << context_->GetUrl()
		<< ", strategy: " << by.GetStrategy()
		<< ", value: " << by.GetValue()
		)
}

inline
std::vector<Element> Finder::FindElements(const By& by) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	const std::vector<detail::ElementRef> ids =
		FromJsonArray<detail::ElementRef>(
			context_->Post("elements", JsonObject()
				.With("using", by.GetStrategy())
				.With("value", by.GetValue())
				.Build()
			));
	std::vector<Element> result;
	for (std::vector<detail::ElementRef>::const_iterator it = ids.begin();
		it != ids.end(); ++it)
		result.push_back(factory_->MakeElement(it->ref));
	return result;
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "context: " << context_->GetUrl()
		<< ", strategy: " << by.GetStrategy()
		<< ", value: " << by.GetValue()
		)
}

} // namespace detail
} // namespace webdriverxx
