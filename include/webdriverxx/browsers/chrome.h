#ifndef WEBDRIVERXX_BROWSERS_CHROME_H
#define WEBDRIVERXX_BROWSERS_CHROME_H

#include "../capabilities.h"

namespace webdriverxx {
namespace chrome {

struct PerfLoggingPrefs : JsonObject {
	WEBDRIVERXX_PROPERTY(EnableNetwork, "enableNetwork", bool)
	WEBDRIVERXX_PROPERTY(enablePage, "enablePage", bool)
	WEBDRIVERXX_PROPERTY(enableTimeline, "enableTimeline", bool)
	WEBDRIVERXX_PROPERTY(tracingCategories, "tracingCategories", std::string)
	WEBDRIVERXX_PROPERTY(bufferUsageReportingInterval, "bufferUsageReportingInterval", int)
};

} // namespace chrome

struct Chrome : Capabilities { // copyable
	Chrome(const Capabilities& defaults = Capabilities())
		: Capabilities(defaults) {
		SetBrowserName(browser::Chrome);
		SetVersion("");
		SetPlatform(platform::Any);
	}

	// See https://sites.google.com/a/chromium.org/chromedriver/capabilities for details
	WEBDRIVERXX_PROPERTY(LoggingPrefs, "loggingPrefs", LoggingPrefs)
	WEBDRIVERXX_PROPERTY(Args, "args", std::vector<std::string>)
	WEBDRIVERXX_PROPERTY(Binary, "binary", std::string)
	// Each extension is a base64-encoded .crx file
	WEBDRIVERXX_PROPERTY(Extensions, "extensions", std::vector<std::string>)
	WEBDRIVERXX_PROPERTY(LocalState, "localState", JsonObject)
	WEBDRIVERXX_PROPERTY(Prefs, "prefs", JsonObject)
	WEBDRIVERXX_PROPERTY(Detach, "detach", bool)
	WEBDRIVERXX_PROPERTY(DebuggerAddress, "debuggerAddress", std::string)
	WEBDRIVERXX_PROPERTY(ExcludeSwitches, "excludeSwitches", std::vector<std::string>)
	WEBDRIVERXX_PROPERTY(MinidumpPath, "minidumpPath", std::string)
	WEBDRIVERXX_PROPERTY(MobileEmulation, "mobileEmulation", JsonObject)
	WEBDRIVERXX_PROPERTY(PerfLoggingPrefs, "perfLoggingPrefs", chrome::PerfLoggingPrefs)
};

} // namespace webdriverxx

#endif
