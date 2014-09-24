#ifndef WEBDRIVERXX_WEBDRIVER_H
#define WEBDRIVERXX_WEBDRIVER_H

#include "element.h"
#include "window.h"
#include "by.h"
#include "capabilities.h"
#include "types.h"
#include "detail/finder.h"
#include "detail/resource.h"
#include "detail/http_connection.h"
#include <string>

namespace webdriverxx {

const char *const kDefaultUrl = "http://localhost:4444/wd/hub/";

// Gives access to some API functions but doesn't create session
class BasicWebDriver { // noncopyable
public:
	explicit BasicWebDriver(const std::string& url = kDefaultUrl);

	picojson::object GetStatus() const;
	SessionsInformation GetSessions() const;

protected:
	struct Session {
		detail::Resource resource;
		Capabilities capabilities;

		explicit Session(
			const detail::Resource& resource,
			const Capabilities& capabilities = Capabilities()
			)
			: resource(resource)
			, capabilities(capabilities) {}
	};

	Session CreateSession(
		const Capabilities& required,
		const Capabilities& desired
		) const;

private:
	BasicWebDriver(BasicWebDriver&);
	BasicWebDriver& operator=(BasicWebDriver&);

private:
	const detail::HttpConnection http_connection_;
	const detail::ServerRoot server_root_;
};

class WebDriver // noncopyable
	: public BasicWebDriver
	, private detail::Finder {
public:
	explicit WebDriver(
		const std::string& url = kDefaultUrl,
		const Capabilities& required = Capabilities(),
		const Capabilities& desired = Capabilities()
		);

	const Capabilities& GetCapabilities() const;
	Window GetCurrentWindow() const;
	const WebDriver& CloseCurrentWindow() const;
	const WebDriver& SetFocusToWindow(const std::string& name_or_handle) const;
	std::vector<Window> GetWindows() const;
	std::string GetUrl() const;
	const WebDriver& Navigate(const std::string& url) const;
	Element FindElement(const By& by) const;
	std::vector<Element> FindElements(const By& by) const;
	const WebDriver& SendKeys(const char* keys) const;
	const WebDriver& SendKeys(const std::string& keys) const;
	template<class IterableStringList>
	const WebDriver& SendKeys(const IterableStringList& keys) const;

private:
	Element FindElement(
		const By& by,
		const detail::Resource& context
		) const;
	std::vector<Element> FindElements(
		const By& by,
		const detail::Resource& context
		) const;
	Window MakeWindow(const std::string& handle) const;
	Element MakeElement(const std::string& id) const;

private:
	const Session session_;
	const detail::AutoResourceDeleter session_deleter_;
};

} // namespace webdriverxx

#include "webdriver.inl"

#endif
