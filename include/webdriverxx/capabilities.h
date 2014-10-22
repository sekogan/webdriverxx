#ifndef WEBDRIVERXX_CAPABILITIES_H
#define WEBDRIVERXX_CAPABILITIES_H

#include "conversions.h"
#include <picojson.h>
#include <string>

namespace webdriverxx {

namespace browser {
typedef std::string Value;
typedef const char* const ConstValue;
ConstValue Android = "android";
ConstValue Chrome = "chrome";
ConstValue Firefox = "firefox";
ConstValue HtmlUnit = "htmlunit";
ConstValue InternetExplorer = "internet explorer";
ConstValue IPhone = "iPhone";
ConstValue IPad = "iPad";
ConstValue Mock = "mock";
ConstValue Opera = "opera";
ConstValue Safari = "safari";
ConstValue Phantom = "phantomjs";
} // namespace browser

namespace platform {
typedef std::string Value;
typedef const char* const ConstValue;
ConstValue Any = "ANY";
ConstValue Windows = "WINDOWS";
ConstValue Xp = "XP";
ConstValue Vista = "VISTA";
ConstValue Mac = "MAC";
ConstValue Linux = "LINUX";
ConstValue Unix = "UNIX";
ConstValue Android = "ANDROID";
} // namespace platform

namespace unexpected_alert_behaviour {
typedef std::string Value;
typedef const char* const ConstValue;
ConstValue Accept = "accept";
ConstValue Dismiss = "dismiss";
ConstValue Ignore = "ignore";
} // namespace unexpected_alert_behaviour

namespace proxy_type {
typedef std::string Value;
typedef const char* const ConstValue;
ConstValue Direct = "direct";
ConstValue Manual = "manual"; // Manual proxy settings configured, e.g. setting a proxy for HTTP, a proxy for FTP
ConstValue Pac = "pac"; // Proxy autoconfiguration from a URL
ConstValue Autodetect = "autodetect"; // Proxy autodetection, probably with WPAD
ConstValue System = "system"; // Use system settings
} // namespace proxy_type

#define WEBDRIVERXX_PROPERTY_RONLY(name, id, type) \
	type Get##name() const { return GetOptional<type>(id); } \
	bool Has##name() { return Has(id); }

#define WEBDRIVERXX_PROPERTY(name, id, type) \
	WEBDRIVERXX_PROPERTY_RONLY(name, id, type) \
	auto Set##name(const type& value) -> decltype(*this) { Set(id, value); return *this; }

struct Proxy : JsonObject { // copyable
	Proxy() {}
	explicit Proxy(const picojson::object& object) : JsonObject(object) {}

	WEBDRIVERXX_PROPERTY(ProxyType, "proxyType", proxy_type::Value)
};

struct DirectConnection : Proxy { // copyable
	DirectConnection() { SetProxyType(proxy_type::Direct); }
};

struct AutodetectProxy : Proxy { // copyable
	AutodetectProxy() { SetProxyType(proxy_type::Autodetect); }
};

struct SystemProxy : Proxy { // copyable
	SystemProxy() { SetProxyType(proxy_type::System); }
};

struct ManualProxy : Proxy { // copyable
	ManualProxy() { SetProxyType(proxy_type::Manual); }

	WEBDRIVERXX_PROPERTY(NoProxyFor, "noProxy", std::string)
};

struct FtpProxy : ManualProxy { // copyable
	explicit FtpProxy(const std::string& address) { SetProxyAddress(address); }

	WEBDRIVERXX_PROPERTY(ProxyAddress, "ftpProxy", std::string)
};

struct HttpProxy : ManualProxy { // copyable
	explicit HttpProxy(const std::string& address) { SetProxyAddress(address); }

	WEBDRIVERXX_PROPERTY(ProxyAddress, "httpProxy", std::string)
};

struct SslProxy : ManualProxy { // copyable
	explicit SslProxy(const std::string& address) { SetProxyAddress(address); }

	WEBDRIVERXX_PROPERTY(ProxyAddress, "sslProxy", std::string)
};

struct SocksProxy : ManualProxy { // copyable
	explicit SocksProxy(const std::string& address) { SetProxyAddress(address); }

	WEBDRIVERXX_PROPERTY(ProxyAddress, "socksProxy", std::string)
	WEBDRIVERXX_PROPERTY(Username, "socksUsername", std::string)
	WEBDRIVERXX_PROPERTY(Password, "socksPassword", std::string)
};

struct AutomaticProxyFromUrl : Proxy { // copyable
	explicit AutomaticProxyFromUrl(const std::string& url) {
		SetProxyType(proxy_type::Pac);
		SetAutoconfigUrl(url);
	}

	WEBDRIVERXX_PROPERTY(AutoconfigUrl, "proxyAutoconfigUrl", std::string)
};

// List of keys and values indicating features that server can or should provide.
struct Capabilities : JsonObject { // copyable
	Capabilities() {}
	explicit Capabilities(const picojson::object& object) : JsonObject(object) {}

	// Hardcoded capabilities are here just to add some sugar.
	// If a capability is not listed below use Get/Set/Has public members.

	WEBDRIVERXX_PROPERTY(BrowserName, "browserName", browser::Value)
	WEBDRIVERXX_PROPERTY(Version, "version", std::string)
	WEBDRIVERXX_PROPERTY(Platform, "platform", platform::Value)

	WEBDRIVERXX_PROPERTY_RONLY(TakesScreenshot, "takesScreenshot", bool)
	WEBDRIVERXX_PROPERTY_RONLY(HandlesAlerts, "handlesAlerts", bool)
	WEBDRIVERXX_PROPERTY_RONLY(CssSelectorsEnabled, "cssSelectorsEnabled", bool)

	WEBDRIVERXX_PROPERTY(JavascriptEnabled, "javascriptEnabled", bool)
	WEBDRIVERXX_PROPERTY(DatabaseEnabled, "databaseEnabled", bool)
	WEBDRIVERXX_PROPERTY(LocationContextEnabled, "locationContextEnabled", bool)
	WEBDRIVERXX_PROPERTY(ApplicationCacheEnabled, "applicationCacheEnabled", bool)
	WEBDRIVERXX_PROPERTY(BrowserConnectionEnabled, "browserConnectionEnabled", bool)
	WEBDRIVERXX_PROPERTY(WebStorageEnabled, "webStorageEnabled", bool)
	WEBDRIVERXX_PROPERTY(AcceptSslCerts, "acceptSslCerts", bool)
	WEBDRIVERXX_PROPERTY(Rotatable, "rotatable", bool)
	WEBDRIVERXX_PROPERTY(NativeEvents, "nativeEvents", bool)
	WEBDRIVERXX_PROPERTY(Proxy, "proxy", Proxy)
	WEBDRIVERXX_PROPERTY(UnexpectedAlertBehaviour, "unexpectedAlertBehaviour", unexpected_alert_behaviour::Value)
	WEBDRIVERXX_PROPERTY(ElementScrollBehavior, "elementScrollBehavior", int)

	WEBDRIVERXX_PROPERTY_RONLY(SessionId, "webdriver.remote.sessionid", std::string)
	WEBDRIVERXX_PROPERTY(QuietExceptions, "webdriver.remote.quietExceptions", bool)
};

inline
picojson::value CustomToJson(const Proxy& value) {
	return static_cast<picojson::value>(value);
}

inline
picojson::value CustomToJson(const Capabilities& value) {
	return static_cast<picojson::value>(value);
}

inline
void CustomFromJson(const picojson::value& value, Capabilities& result) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Capabilities is not an object");
	result = Capabilities(value.get<picojson::object>());
}

} // namespace webdriverxx

#endif
