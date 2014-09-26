#ifndef WEBDRIVERXX_DETAIL_KEYBOARD_H
#define WEBDRIVERXX_DETAIL_KEYBOARD_H

#include "resource.h"
#include "conversions.h"
#include <vector>

namespace webdriverxx {
namespace detail {

class Keyboard { // copyable
public:
	Keyboard(const Resource& resource, const std::string& command)
		: resource_(&resource)
		, command_(command) {}

	void SendKeys(const char* keys) const {
		SendKeys(std::string(keys));
	}

	void SendKeys(const std::string& keys) const {
		std::vector<std::string> list(1, keys);
		SendKeys(list);
	}

	template<class IterableStringList>
	void SendKeys(const IterableStringList& keys) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		resource_->Post(command_, detail::JsonObject()
			.With("value", detail::ToJsonArray(keys)).Build());
		WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(Fmt()
			<< "command: " << command_
			<< ", resource: " << resource_->GetUrl()
			)
	}

private:
	const Resource* resource_;
	std::string command_;
};

} // namespace detail
} // namespace webdriverxx

#endif
