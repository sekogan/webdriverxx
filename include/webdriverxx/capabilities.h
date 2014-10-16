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
ConstValue GC = Chrome;
ConstValue GoogleChrome = "googlechrome";
ConstValue Firefox = "firefox";
ConstValue FF = Firefox;
ConstValue Firefox2 = "firefox2";
ConstValue Firefox3 = "firefox3";
ConstValue FirefoxChrome = "firefoxchrome";
ConstValue FirefoxProxy = "firefoxproxy";
ConstValue HtmlUnit = "htmlunit";
ConstValue IE = "internet explorer";
ConstValue IE_HTA="iehta";
ConstValue IExplore = "iexplore";
ConstValue IExploreProxy= "iexploreproxy";
ConstValue IPad = "iPad";
ConstValue IPhone = "iPhone";
ConstValue Mock = "mock";
ConstValue Opera = "opera";
ConstValue PhantomJS = "phantomjs";
ConstValue Phantom = PhantomJS;
ConstValue Safari = "safari";
ConstValue SafariProxy = "safariproxy";
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

#define WEBDRIVERXX_CAPABILITY_RONLY(name, id, type) \
	type Get##name() const { return GetOptional<type>(id); } \
	bool Has##name() { return Has(id); }

#define WEBDRIVERXX_CAPABILITY(name, id, type) \
	WEBDRIVERXX_CAPABILITY_RONLY(name, id, type) \
	Self& Set##name(const type& value) { Set(id, value); return *this; }

struct Proxy : JsonObject { // copyable
	Proxy() {}
	explicit Proxy(const picojson::object& object) : JsonObject(object) {}

	typedef Proxy Self;
	WEBDRIVERXX_CAPABILITY(ProxyType, "proxyType", proxy_type::Value)
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

	typedef ManualProxy Self;
	WEBDRIVERXX_CAPABILITY(NoProxyFor, "noProxy", std::string)
};

struct FtpProxy : ManualProxy { // copyable
	explicit FtpProxy(const std::string& address) { SetProxyAddress(address); }

	typedef FtpProxy Self;
	WEBDRIVERXX_CAPABILITY(ProxyAddress, "ftpProxy", std::string)
};

struct HttpProxy : ManualProxy { // copyable
	explicit HttpProxy(const std::string& address) { SetProxyAddress(address); }

	typedef HttpProxy Self;
	WEBDRIVERXX_CAPABILITY(ProxyAddress, "httpProxy", std::string)
};

struct SslProxy : ManualProxy { // copyable
	explicit SslProxy(const std::string& address) { SetProxyAddress(address); }

	typedef SslProxy Self;
	WEBDRIVERXX_CAPABILITY(ProxyAddress, "sslProxy", std::string)
};

struct SocksProxy : ManualProxy { // copyable
	explicit SocksProxy(const std::string& address) { SetProxyAddress(address); }

	typedef SocksProxy Self;
	WEBDRIVERXX_CAPABILITY(ProxyAddress, "socksProxy", std::string)
	WEBDRIVERXX_CAPABILITY(Username, "socksUsername", std::string)
	WEBDRIVERXX_CAPABILITY(Password, "socksPassword", std::string)
};

struct AutomaticProxyFromUrl : Proxy { // copyable
	explicit AutomaticProxyFromUrl(const std::string& url) {
		SetProxyType(proxy_type::Pac);
		SetAutoconfigUrl(url);
	}

	typedef AutomaticProxyFromUrl Self;
	WEBDRIVERXX_CAPABILITY(AutoconfigUrl, "proxyAutoconfigUrl", std::string)
};

// List of keys and values indicating features that server can or should provide.
struct Capabilities : JsonObject { // copyable
	Capabilities() {}
	explicit Capabilities(const picojson::object& object) : JsonObject(object) {}

	// Hardcoded capabilities are here just to add some sugar.
	// If a capability is not listed below use Get/Set/Has public members.

	typedef Capabilities Self;
	WEBDRIVERXX_CAPABILITY(BrowserName, "browserName", browser::Value)
	WEBDRIVERXX_CAPABILITY(Version, "version", std::string)
	WEBDRIVERXX_CAPABILITY(Platform, "platform", platform::Value)

	WEBDRIVERXX_CAPABILITY_RONLY(TakesScreenshot, "takesScreenshot", bool)
	WEBDRIVERXX_CAPABILITY_RONLY(HandlesAlerts, "handlesAlerts", bool)
	WEBDRIVERXX_CAPABILITY_RONLY(CssSelectorsEnabled, "cssSelectorsEnabled", bool)

	WEBDRIVERXX_CAPABILITY(JavascriptEnabled, "javascriptEnabled", bool)
	WEBDRIVERXX_CAPABILITY(DatabaseEnabled, "databaseEnabled", bool)
	WEBDRIVERXX_CAPABILITY(LocationContextEnabled, "locationContextEnabled", bool)
	WEBDRIVERXX_CAPABILITY(ApplicationCacheEnabled, "applicationCacheEnabled", bool)
	WEBDRIVERXX_CAPABILITY(BrowserConnectionEnabled, "browserConnectionEnabled", bool)
	WEBDRIVERXX_CAPABILITY(WebStorageEnabled, "webStorageEnabled", bool)
	WEBDRIVERXX_CAPABILITY(AcceptSslCerts, "acceptSslCerts", bool)
	WEBDRIVERXX_CAPABILITY(Rotatable, "rotatable", bool)
	WEBDRIVERXX_CAPABILITY(NativeEvents, "nativeEvents", bool)
	WEBDRIVERXX_CAPABILITY(Proxy, "proxy", Proxy)
	WEBDRIVERXX_CAPABILITY(UnexpectedAlertBehaviour, "unexpectedAlertBehaviour", unexpected_alert_behaviour::Value)
	WEBDRIVERXX_CAPABILITY(ElementScrollBehavior, "elementScrollBehavior", int)

	WEBDRIVERXX_CAPABILITY_RONLY(SessionId, "webdriver.remote.sessionid", std::string)
	WEBDRIVERXX_CAPABILITY(QuietExceptions, "webdriver.remote.quietExceptions", bool)
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
