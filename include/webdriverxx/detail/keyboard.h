#ifndef WEBDRIVERXX_DETAIL_KEYBOARD_H
#define WEBDRIVERXX_DETAIL_KEYBOARD_H

#include "resource.h"
#include "../conversions.h"
#include "../keys.h"
#include <vector>

namespace webdriverxx {
namespace detail {

class Keyboard { // copyable
public:
	Keyboard(const Resource& resource, const std::string& command)
		: resource_(&resource)
		, command_(command)
	{}

	void SendKeys(const char* keys) const {
		SendKeys(Shortcut() << keys);
	}

	void SendKeys(const std::string& keys) const {
		SendKeys(Shortcut() << keys);
	}

	void SendKeys(const Shortcut& shortcut) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		resource_->Post(command_, JsonObject()
			.With("value", ToJson(shortcut.keys_)).Build());
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
