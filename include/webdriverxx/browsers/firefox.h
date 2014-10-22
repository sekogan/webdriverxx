#ifndef WEBDRIVERXX_BROWSERS_FIREFOX_H
#define WEBDRIVERXX_BROWSERS_FIREFOX_H

#include "../capabilities.h"

namespace webdriverxx {
namespace firefox {

namespace log_level {
typedef std::string Value;
typedef const char* const ConstValue;
ConstValue Off = "OFF";
ConstValue Severe = "SEVERE";
ConstValue Warning = "WARNING";
ConstValue Info = "INFO";
ConstValue Config = "CONFIG";
ConstValue Fine = "FINE";
ConstValue Finer = "FINER";
ConstValue Finest = "FINEST";
ConstValue All = "ALL";
} // namespace log_level

struct Logging : JsonObject {
	WEBDRIVERXX_PROPERTY(Level, "driver", log_level::Value)
};

} // namespace firefox

struct Firefox : Capabilities { // copyable
	Firefox(const Capabilities& defaults = Capabilities())
		: Capabilities(defaults) {
		SetBrowserName(browser::Firefox);
		SetVersion("");
		SetPlatform(platform::Any);
	}

	// Profile is a profile folder, zipped and base64 encoded.
	// TODO: add FirefoxProfile 
	WEBDRIVERXX_PROPERTY(Profile, "firefox_profile", std::string)
	WEBDRIVERXX_PROPERTY(Logging, "loggingPrefs", firefox::Logging)
	WEBDRIVERXX_PROPERTY(FirefoxBinary, "firefox_binary", std::string)
};

namespace firefox {

inline
picojson::value CustomToJson(const Logging& value) {
	return static_cast<picojson::value>(value);
}

} // namespace firefox
} // namespace webdriverxx

#endif
