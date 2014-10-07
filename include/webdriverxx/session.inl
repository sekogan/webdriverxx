#include "conversions.h"
#include "detail/error_handling.h"
#include "detail/types.h"

namespace webdriverxx {

inline
Session::Session(const detail::Shared<detail::Resource>& resource)
	: resource_(resource)
	, factory_(new detail::SessionFactory(resource))
{}

inline
void Session::DeleteSession() const {
	resource_->Delete();
}

inline
Capabilities Session::GetCapabilities() const {
	return detail::CapabilitiesAccess::Construct(
		resource_->Get().get<picojson::object>()
		);
}

inline
std::string Session::GetSource() const {
	return resource_->GetString("source");
}

inline
std::string Session::GetTitle() const {
	return resource_->GetString("title");
}

inline
std::string Session::GetUrl() const {
	return resource_->GetString("url");
}

inline
std::string Session::GetScreenshot() const {
	return resource_->GetString("screenshot");
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
		resource_->Post("timeouts/async_script",
			JsonObject().With("ms", milliseconds).Build());
		return *this;
	case timeout::ElementFind:
		resource_->Post("timeouts/implicit_wait",
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
	return MakeWindow(resource_->GetString("window_handle"));
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
const Session& Session::CloseCurrentWindow() const {
	resource_->Delete("window");
	return *this;
}

inline
const Session& Session::Navigate(const std::string& url) const {
	resource_->Post("url", "url", url);
	return *this;
}

inline
const Session& Session::Get(const std::string& url) const {
	return Navigate(url);
}

inline
const Session& Session::Forward() const {
	resource_->Post("forward");
	return *this;
}

inline
const Session& Session::Back() const {
	resource_->Post("back");
	return *this;
}

inline
const Session& Session::Refresh() const {
	resource_->Post("refresh");
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
	return factory_->MakeElement(Eval<detail::ElementRef>(script, args).ref);
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
	return factory_->MakeElement(EvalAsync<detail::ElementRef>(script, args).ref);
}

inline
const Session& Session::SetFocusToWindow(const std::string& window_name_or_handle) const {
	resource_->Post("window", "name", window_name_or_handle);
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
	resource_->Post("frame/parent");
	return *this;
}

inline
const Session& Session::InternalSetFocusToFrame(const picojson::value& id) const {
	resource_->Post("frame", JsonObject().With("id", id).Build());
	return *this;
}

inline
std::vector<Window> Session::GetWindows() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	const std::vector<std::string> handles =
		FromJsonArray<std::string>(
			resource_->Get("window_handles")
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
	return factory_->MakeElement(FromJson<detail::ElementRef>(resource_->Post("element/active")).ref);
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
Element Session::FindElement(const By& by) const {
	return factory_->MakeFinder(resource_).FindElement(by);
}

inline
std::vector<Element> Session::FindElements(const By& by) const {
	return factory_->MakeFinder(resource_).FindElements(by);
}

inline
std::vector<Cookie> Session::GetCookies() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return FromJson< std::vector<Cookie> >(resource_->Get("cookie"));
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
const Session& Session::SetCookie(const Cookie& cookie) const {
	resource_->Post("cookie", JsonObject()
		.With("cookie", ToJson(cookie)).Build());
	return *this;
}

inline
const Session& Session::DeleteCookies() const {
	resource_->Delete("cookie");
	return *this;
}

inline
const Session& Session::DeleteCookie(const std::string& name) const {
	resource_->Delete(std::string("cookie/") + name);
	return *this;
}

inline
std::string Session::GetAlertText() const {
	return resource_->GetString("alert_text");
}

inline
const Session& Session::SendKeysToAlert(const std::string& text) const {
	resource_->Post("alert_text", "text", text);
	return *this;
}

inline
const Session& Session::AcceptAlert() const {
	resource_->Post("accept_alert");
	return *this;
}

inline
const Session& Session::DismissAlert() const {
	resource_->Post("dismiss_alert");
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
Window Session::MakeWindow(const std::string& handle) const {
	return Window(handle,
		detail::MakeSubResource(resource_, "window", handle)
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
	return resource_->Post(command,
		JsonObject()
			.With("script", script)
			.With("args", args.args_)
			.Build()
		);
}

inline
const Session& Session::InternalSetTimeout(const std::string& type, int milliseconds) const {
	resource_->Post("timeouts",
		JsonObject()
			.With("type", type)
			.With("ms", milliseconds)
			.Build()
		);
	return *this;
}

} // namespace webdriverxx
