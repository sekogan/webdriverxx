#ifndef WEBDRIVERXX_SESSION_H
#define WEBDRIVERXX_SESSION_H

#include "element.h"
#include "window.h"
#include "by.h"
#include "capabilities.h"
#include "keys.h"
#include "js_args.h"
#include "detail/finder.h"
#include "detail/resource.h"
#include "detail/keyboard.h"
#include <picojson.h>
#include <string>

namespace webdriverxx {

class Server;

class Session : private detail::Finder { // copyable
public:	
	// All sessions that are explicitly created with Server::CreateSession()
	// should be explicitly deleted.
	void DeleteSession() const;

	Capabilities GetCapabilities() const;
	std::string GetBrowser() const;
	const Session& SetTimeout(timeout::Type type, int milliseconds);

	std::string GetSource() const;
	std::string GetTitle() const;
	std::string GetUrl() const;
	std::string GetScreenshot() const; // Base64 PNG

	const Session& Navigate(const std::string& url) const;
	const Session& Forward() const;
	const Session& Back() const;
	const Session& Refresh() const;

	const Session& Execute(const std::string& script, const JsArgs& args = JsArgs()) const;
	template<typename T>
	T Eval(const std::string& script, const JsArgs& args = JsArgs()) const;
	Element EvalElement(const std::string& script, const JsArgs& args = JsArgs()) const;
	const Session& ExecuteAsync(const std::string& script, const JsArgs& args = JsArgs()) const;
	template<typename T>
	T EvalAsync(const std::string& script, const JsArgs& args = JsArgs()) const;
	Element EvalElementAsync(const std::string& script, const JsArgs& args = JsArgs()) const;

	const Session& SetFocusToFrame(const Element& frame) const;
	const Session& SetFocusToFrame(const std::string& id) const;
	const Session& SetFocusToFrame(int number) const;
	const Session& SetFocusToDefaultFrame() const;
	const Session& SetFocusToParentFrame() const;

	std::vector<Window> GetWindows() const;
	Window GetCurrentWindow() const;
	const Session& CloseCurrentWindow() const;
	const Session& SetFocusToWindow(const std::string& window_name) const;
	const Session& SetFocusToWindow(const Window& window) const;
	
	Element GetActiveElement() const;

	Element FindElement(const By& by) const;
	std::vector<Element> FindElements(const By& by) const;

	std::vector<Cookie> GetCookies() const;
	const Session& SetCookie(const Cookie& cookie) const;
	const Session& DeleteCookies() const;
	const Session& DeleteCookie(const std::string& name) const;

	const Session& SendKeys(const char* keys) const;
	const Session& SendKeys(const std::string& keys) const;
	const Session& SendKeys(const Shortcut& shortcut) const;

	virtual ~Session() {}

private:
	friend class Server; // Only Server can create Sessions

	Session(
		const detail::Resource& resource,
		const Capabilities& capabilities
		);

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
	detail::Keyboard GetKeyboard() const;
	picojson::value InternalEval(const std::string& script, const JsArgs& args) const;
	picojson::value InternalEvalAsync(const std::string& script, const JsArgs& args) const;
	picojson::value InternalEval(const std::string& command, const std::string& script,
		const JsArgs& args) const;
	const Session& InternalSetFocusToFrame(const picojson::value& id) const;
	const Session& InternalSetTimeout(const std::string& type, int milliseconds) const;

private:
	detail::Resource resource_;
	Capabilities capabilities_;
};

} // namespace webdriverxx

#include "session.inl"

#endif
