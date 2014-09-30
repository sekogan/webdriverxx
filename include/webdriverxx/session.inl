#include "conversions.h"
#include "detail/error_handling.h"

namespace webdriverxx {

inline
Session::Session(
	const detail::Resource& resource,
	const Capabilities& capabilities
	)
	: resource_(resource)
	, capabilities_(capabilities) {}

inline
void Session::DeleteSession() const {
	resource_.Delete();
}

inline
Capabilities Session::GetCapabilities() const {
	return capabilities_;
}

inline
std::string Session::GetBrowser() const {
	return capabilities_.GetString(capabilities::BrowserName);
}

inline
std::string Session::GetSource() const {
	return resource_.GetString("source");
}

inline
std::string Session::GetTitle() const {
	return resource_.GetString("title");
}

inline
std::string Session::GetUrl() const {
	return resource_.GetString("url");
}

inline
std::string Session::GetScreenshot() const {
	return resource_.GetString("screenshot");
}

inline
const Session& Session::SetTimeout(timeout::Type type, int milliseconds) {
	switch (type) {
	case timeout::ImplicitWait:
		return InternalSetTimeout("implicit", milliseconds);
	case timeout::PageLoad:
		return InternalSetTimeout("page load", milliseconds);
	case timeout::Script:
		return InternalSetTimeout("script", milliseconds);
	case timeout::AsyncScript:
		resource_.Post("timeouts/async_script",
			JsonObject().With("ms", milliseconds).Build());
		return *this;
	case timeout::ElementFind:
		resource_.Post("timeouts/implicit_wait",
			JsonObject().With("ms", milliseconds).Build());
		return *this;
	default:
		WEBDRIVERXX_CHECK(false, "Unknown timeout type");
	}
	return *this;
}

inline
Window Session::GetCurrentWindow() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return MakeWindow(resource_.GetString("window_handle"));
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
const Session& Session::CloseCurrentWindow() const {
	resource_.Delete("window");
	return *this;
}

inline
const Session& Session::Navigate(const std::string& url) const {
	resource_.Post("url", "url", url);
	return *this;
}

inline
const Session& Session::Forward() const {
	resource_.Post("forward");
	return *this;
}

inline
const Session& Session::Back() const {
	resource_.Post("back");
	return *this;
}

inline
const Session& Session::Refresh() const {
	resource_.Post("refresh");
	return *this;
}

inline
const Session& Session::Execute(const std::string& script, const JsArgs& args) const {
	InternalEval(script, args);
	return *this;
}

template<typename T>
inline
T Session::Eval(const std::string& script, const JsArgs& args) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return FromJson<T>(InternalEval(script, args));
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "script: " << script
		)
}

inline
Element Session::EvalElement(const std::string& script, const JsArgs& args) const {
	return MakeElement(Eval<detail::ElementRef>(script, args).ref);
}

inline
const Session& Session::ExecuteAsync(const std::string& script, const JsArgs& args) const {
	InternalEvalAsync(script, args);
	return *this;
}

template<typename T>
inline
T Session::EvalAsync(const std::string& script, const JsArgs& args) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return FromJson<T>(InternalEvalAsync(script, args));
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "script: " << script
		)
}

inline
Element Session::EvalElementAsync(const std::string& script, const JsArgs& args) const {
	return MakeElement(EvalAsync<detail::ElementRef>(script, args).ref);
}

inline
const Session& Session::SetFocusToWindow(const std::string& window_name_or_handle) const {
	resource_.Post("window", "name", window_name_or_handle);
	return *this;
}

inline
const Session& Session::SetFocusToWindow(const Window& window) const {
	SetFocusToWindow(window.GetHandle());
	return *this;
}

inline
const Session& Session::SetFocusToFrame(const Element& frame) const {
	return InternalSetFocusToFrame(ToJson(frame));
}

inline
const Session& Session::SetFocusToFrame(const std::string& id) const {
	return InternalSetFocusToFrame(ToJson(id));
}

inline
const Session& Session::SetFocusToFrame(int number) const {
	return InternalSetFocusToFrame(ToJson(number));
}

inline
const Session& Session::SetFocusToDefaultFrame() const {
	return InternalSetFocusToFrame(picojson::value());
}

inline
const Session& Session::SetFocusToParentFrame() const {
	resource_.Post("frame/parent");
	return *this;
}

inline
const Session& Session::InternalSetFocusToFrame(const picojson::value& id) const {
	resource_.Post("frame", JsonObject().With("id", id).Build());
	return *this;
}

inline
std::vector<Window> Session::GetWindows() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	const std::vector<std::string> handles =
		FromJsonArray<std::string>(
			resource_.Get("window_handles")
			);
	std::vector<Window> result;
	for (std::vector<std::string>::const_iterator it = handles.begin();
		it != handles.end(); ++it)
		result.push_back(MakeWindow(*it));
	return result;
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
Element Session::GetActiveElement() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return MakeElement(FromJson<detail::ElementRef>(resource_.Post("element/active")).ref);
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
Element Session::FindElement(const By& by) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return FindElement(by, resource_);
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "strategy: " << by.GetStrategy()
		<< ", value: " << by.GetValue()
		)
}

inline
std::vector<Element> Session::FindElements(const By& by) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return FindElements(by, resource_);
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "strategy: " << by.GetStrategy()
		<< ", value: " << by.GetValue()
		)
}

inline
std::vector<Cookie> Session::GetCookies() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return FromJson< std::vector<Cookie> >(resource_.Get("cookie"));
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
const Session& Session::SetCookie(const Cookie& cookie) const {
	resource_.Post("cookie", JsonObject()
		.With("cookie", ToJson(cookie)).Build());
	return *this;
}

inline
const Session& Session::DeleteCookies() const {
	resource_.Delete("cookie");
	return *this;
}

inline
const Session& Session::DeleteCookie(const std::string& name) const {
	resource_.Delete(std::string("cookie/") + name);
	return *this;
}

inline
const Session& Session::SendKeys(const char* keys) const {
	GetKeyboard().SendKeys(keys);
	return *this;
}

inline
const Session& Session::SendKeys(const std::string& keys) const {
	GetKeyboard().SendKeys(keys);
	return *this;
}

inline
const Session& Session::SendKeys(const Shortcut& shortcut) const {
	GetKeyboard().SendKeys(shortcut);
	return *this;
}

inline
Element Session::FindElement(
	const By& by,
	const detail::Resource& context
	) const {
	return MakeElement(FromJson<detail::ElementRef>(
		context.Post("element",
			JsonObject()
				.With("using", by.GetStrategy())
				.With("value", by.GetValue())
				.Build()
			)).ref);
}

inline
std::vector<Element> Session::FindElements(
	const By& by,
	const detail::Resource& context
	) const {
	const std::vector<detail::ElementRef> ids =
		FromJsonArray<detail::ElementRef>(
			context.Post("elements", JsonObject()
				.With("using", by.GetStrategy())
				.With("value", by.GetValue())
				.Build()
			));
	std::vector<Element> result;
	for (std::vector<detail::ElementRef>::const_iterator it = ids.begin();
		it != ids.end(); ++it)
		result.push_back(MakeElement(it->ref));
	return result;
}

inline
Window Session::MakeWindow(const std::string& handle) const {
	return Window(handle,
		resource_.GetSubResource("window").GetSubResource(handle)
		);
}	

inline
Element Session::MakeElement(const std::string& id) const {
	return Element(
		id,
		resource_.GetSubResource("element").GetSubResource(id),
		this
		);
}

inline
detail::Keyboard Session::GetKeyboard() const
{
	return detail::Keyboard(resource_, "keys");
}

inline
picojson::value Session::InternalEval(const std::string& script, const JsArgs& args) const {
	return InternalEval("execute", script, args);
}

inline
picojson::value Session::InternalEvalAsync(const std::string& script, const JsArgs& args) const {
	return InternalEval("execute_async", script, args);
}

inline
picojson::value Session::InternalEval(
	const std::string& command,
	const std::string& script,
	const JsArgs& args
	) const {
	return resource_.Post(command,
		JsonObject()
			.With("script", script)
			.With("args", args.args_)
			.Build()
		);
}

inline
const Session& Session::InternalSetTimeout(const std::string& type, int milliseconds) const {
	resource_.Post("timeouts",
		JsonObject()
			.With("type", type)
			.With("ms", milliseconds)
			.Build()
		);
	return *this;
}

} // namespace webdriverxx
