#ifndef WEBDRIVERXX_DETAIL_KEYBOARD_H
#define WEBDRIVERXX_DETAIL_KEYBOARD_H

#include "resource.h"
#include "../conversions.h"
#include "../keys.h"

namespace webdriverxx {
namespace detail {

class Keyboard { // copyable
public:
	Keyboard(const Shared<Resource>& resource, const std::string& command)
		: resource_(resource)
		, command_(command)
	{}

	const Keyboard& SendKeys(const std::string& keys) const {
		return SendKeys(Shortcut() << keys);
	}

	const Keyboard& SendKeys(const Shortcut& shortcut) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		resource_->Post(command_, JsonObject()
			.With("value", ToJson(shortcut.keys_)).Build());
		return *this;
		WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(Fmt()
			<< "command: " << command_
			<< ", resource: " << resource_->GetUrl()
			)
	}

private:
	Shared<Resource> resource_;
	std::string command_;
};

} // namespace detail
} // namespace webdriverxx

#endif
