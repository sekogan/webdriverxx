#ifndef WEBDRIVERXX_TYPES_H
#define WEBDRIVERXX_TYPES_H

namespace webdriverxx {

struct Size {
	int width;
	int height;
	Size() : width(0), height(0) {}
};

struct Point {
	int x;
	int y;
	Point() : x(0), y(0) {}
};

namespace timeout {
enum Type {
	ImplicitWait,
	PageLoad,
	Script,
	AsyncScript,
	ElementFind
};
} // namespace timeout

namespace browser {
const char *const Android = "android";
const char *const Chrome = "chrome";
const char *const GC = Chrome;
const char *const GoogleChrome = "googlechrome";
const char *const Firefox = "firefox";
const char *const FF = Firefox;
const char *const Firefox2 = "firefox2";
const char *const Firefox3 = "firefox3";
const char *const FirefoxChrome = "firefoxchrome";
const char *const FirefoxProxy = "firefoxproxy";
const char *const HtmlUnit = "htmlunit";
const char *const IE = "internet explorer";
const char *const IE_HTA="iehta";
const char *const IExplore = "iexplore";
const char *const IExploreProxy= "iexploreproxy";
const char *const IPad = "iPad";
const char *const IPhone = "iPhone";
const char *const Mock = "mock";
const char *const Opera = "opera";
const char *const PhantomJS = "phantomjs";
const char *const Phantom = PhantomJS;
const char *const Safari = "safari";
const char *const SafariProxy = "safariproxy";
} // namespace browser

namespace capabilities {
const char *const BrowserName = "browserName";
// TODO
} // namespace browser

} // namespace webdriverxx

#endif
