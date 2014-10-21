#ifndef WEBDRIVERXX_BROWSERS_CHROME_H
#define WEBDRIVERXX_BROWSERS_CHROME_H

#include "../capabilities.h"

namespace webdriverxx {

struct Chrome : Capabilities { // copyable
	Chrome(const Capabilities& defaults = Capabilities())
		: Capabilities(defaults) {
		SetBrowserName(browser::Chrome);
		SetVersion("");
		SetPlatform(platform::Any);
	}

	// TODO
};

} // namespace webdriverxx

#endif
