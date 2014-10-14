#include "conversions.h"
#include "detail/error_handling.h"
#include "detail/types.h"
#include <algorithm>

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
const Session& Session::SetTimeoutMs(timeout::Type type, int milliseconds) {
	resource_->Post("timeouts",
		JsonObject()
			.With("type", type)
			.With("ms", milliseconds)
			.Build()
		);
	return *this;
}

inline
const Session& Session::SetImplicitTimeoutMs(int milliseconds) {
	resource_->Post("timeouts/implicit_wait",
		JsonObject().With("ms", milliseconds).Build());
	return *this;	
}

inline
const Session& Session::SetAsyncScriptTimeoutMs(int milliseconds) {
	resource_->Post("timeouts/async_script",
		JsonObject().With("ms", milliseconds).Build());
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
	InternalEvalJsonValue("execute", script, args);
	return *this;
}

template<typename T>
inline
T Session::Eval(const std::string& script, const JsArgs& args) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	T result = T();
	InternalEval("execute", script, args, result);
	return result;
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "script: " << script
		)
}

inline
const Session& Session::ExecuteAsync(const std::string& script, const JsArgs& args) const {
	InternalEvalJsonValue("execute_async", script, args);
	return *this;
}

template<typename T>
inline
T Session::EvalAsync(const std::string& script, const JsArgs& args) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	T result;
	InternalEval("execute_async", script, args, result);
	return result;
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "script: " << script
		)
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
	const auto handles =
		FromJson<std::vector<std::string>>(
			resource_->Get("window_handles")
			);
	std::vector<Window> result;
	result.reserve(handles.size());
	std::transform(handles.begin(), handles.end(), std::back_inserter(result),
		[this](const std::string& window_handle){
			return MakeWindow(window_handle);
		});
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
	return FromJson<std::vector<Cookie>>(resource_->Get("cookie"));
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

template<typename T>
inline
void Session::InternalEval(const std::string& webdriver_command,
	const std::string& script, const JsArgs& args,
	T& result) const {
	result = FromJson<T>(InternalEvalJsonValue(webdriver_command, script, args));
}

inline
void Session::InternalEval(const std::string& webdriver_command,
	const std::string& script, const JsArgs& args,
	Element& result) const {
	detail::ElementRef element_ref;
	InternalEval(webdriver_command, script, args, element_ref);
	result = factory_->MakeElement(element_ref.ref);
}

inline
picojson::value Session::InternalEvalJsonValue(
	const std::string& webdriver_command,
	const std::string& script,
	const JsArgs& args
	) const {
	return resource_->Post(webdriver_command,
		JsonObject()
			.With("script", script)
			.With("args", args.args_)
			.Build()
		);
}

} // namespace webdriverxx
