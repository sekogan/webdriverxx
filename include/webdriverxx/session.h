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
	std::string GetSource() const;
	std::string GetTitle() const;
	std::string GetUrl() const;
	std::vector<Window> GetWindows() const;
	Window GetCurrentWindow() const;

	const Session& Navigate(const std::string& url) const;
	const Session& Forward() const;
	const Session& Back() const;
	const Session& Refresh() const;

	const Session& Execute(const std::string& script, const JsArgs& args = JsArgs()) const;
	template<typename T>
	T Eval(const std::string& script, const JsArgs& args = JsArgs()) const;
	Element EvalElement(const std::string& script, const JsArgs& args = JsArgs()) const;

	const Session& CloseCurrentWindow() const;
	const Session& SetFocusToWindow(const std::string& name_or_handle) const;
	
	Element FindElement(const By& by) const;
	std::vector<Element> FindElements(const By& by) const;

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

private:
	detail::Resource resource_;
	Capabilities capabilities_;
};

} // namespace webdriverxx

#include "session.inl"

#endif
