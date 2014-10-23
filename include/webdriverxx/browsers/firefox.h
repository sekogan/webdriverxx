#ifndef WEBDRIVERXX_BROWSERS_FIREFOX_H
#define WEBDRIVERXX_BROWSERS_FIREFOX_H

#include "../capabilities.h"

namespace webdriverxx {

struct Firefox : Capabilities { // copyable
	Firefox(const Capabilities& defaults = Capabilities())
		: Capabilities(defaults) {
		SetBrowserName(browser::Firefox);
		SetVersion("");
		SetPlatform(platform::Any);
	}

	WEBDRIVERXX_PROPERTIES_BEGIN(Firefox)
	// Profile is a profile folder, zipped and base64 encoded.
	// TODO: add FirefoxProfile 
	WEBDRIVERXX_PROPERTY(Profile, "firefox_profile", std::string)
	WEBDRIVERXX_PROPERTY(LoggingPrefs, "loggingPrefs", LoggingPrefs)
	WEBDRIVERXX_PROPERTY(FirefoxBinary, "firefox_binary", std::string)
	WEBDRIVERXX_PROPERTIES_END()
};

} // namespace webdriverxx

#endif
