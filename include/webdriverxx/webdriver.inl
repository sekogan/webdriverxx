#include "detail/error_handling.h"
#include "detail/conversions.h"

namespace webdriverxx {

inline
BasicWebDriver::BasicWebDriver(const std::string& url)
	: server_root_(&http_connection_, url) {}

inline
picojson::object BasicWebDriver::GetStatus() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	const picojson::value value = server_root_.Get("status").get("value");
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Value is not an object");
	return value.get<picojson::object>();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
SessionsInformation BasicWebDriver::GetSessions() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return detail::FromJsonArray<SessionInformation>(
		server_root_.Get("sessions").get("value")
		);
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
BasicWebDriver::Session BasicWebDriver::CreateSession(
	const Capabilities& required,
	const Capabilities& desired
	) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	const picojson::value& response = server_root_.Post("session",
		detail::JsonObject()
			.With("requiredCapabilities", detail::CapabilitiesAccess::GetJsonObject(required))
			.With("desiredCapabilities", detail::CapabilitiesAccess::GetJsonObject(desired))
			.Build());

	WEBDRIVERXX_CHECK(response.get("sessionId").is<std::string>(), "Session ID is not a string");
	WEBDRIVERXX_CHECK(response.get("value").is<picojson::object>(), "Capabilities is not an object");
	
	const std::string& sessionId = response.get("sessionId").to_str();
	
	return Session(
		server_root_
			.GetSubResource<detail::Resource>("session")
			.GetSubResource(sessionId),
		detail::CapabilitiesAccess::Construct(response.get("value").get<picojson::object>())
		);
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
WebDriver::WebDriver(
	const std::string& url,
	const Capabilities& required,
	const Capabilities& desired
	)
	: BasicWebDriver(url)
	, session_(CreateSession(required, desired))
	, session_deleter_(session_.resource) {}

inline
const Capabilities& WebDriver::GetCapabilities() const {
	return session_.capabilities;
}

inline
Window WebDriver::GetCurrentWindow() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return MakeWindow(detail::FromJson<std::string>(
		session_.resource.Get("window_handle")
		));
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
void WebDriver::CloseCurrentWindow() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	session_.resource.Delete("window");
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
void WebDriver::SetFocusToWindow(const std::string& name_or_handle) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	session_.resource.Post("window",
		detail::JsonObject().With("name", name_or_handle).Build()
		);
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
std::vector<Window> WebDriver::GetWindows() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	const std::vector<std::string> handles =
		detail::FromJsonArray<std::string>(
			session_.resource.Get("window_handles")
			);
	std::vector<Window> result;
	for (std::vector<std::string>::const_iterator it = handles.begin();
		it != handles.end(); ++it)
		result.push_back(MakeWindow(*it));
	return result;
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
std::string WebDriver::GetUrl() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return session_.resource.Get("url").to_str();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
void WebDriver::Navigate(const std::string& url) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	session_.resource.Post("url", detail::JsonObject().With("url", url).Build());
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
Element WebDriver::FindElement(const By& by) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return FindElement(by, session_.resource);
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "strategy: " << by.GetStrategy()
		<< ", value: " << by.GetValue()
		)
}

inline
std::vector<Element> WebDriver::FindElements(const By& by) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	return FindElements(by, session_.resource);
	WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
		<< "strategy: " << by.GetStrategy()
		<< ", value: " << by.GetValue()
		)
}

inline
Element WebDriver::FindElement(
	const By& by,
	const detail::Resource& context
	) const {
	return MakeElement(detail::FromJson<detail::ElementId>(
		context.Post("element",
			detail::JsonObject()
				.With("using", by.GetStrategy())
				.With("value", by.GetValue())
				.Build()
			)).id);
}

inline
std::vector<Element> WebDriver::FindElements(
	const By& by,
	const detail::Resource& context
	) const {
	const std::vector<detail::ElementId> ids =
		detail::FromJsonArray<detail::ElementId>(
			context.Post("elements", detail::JsonObject()
				.With("using", by.GetStrategy())
				.With("value", by.GetValue())
				.Build()
			));
	std::vector<Element> result;
	for (std::vector<detail::ElementId>::const_iterator it = ids.begin();
		it != ids.end(); ++it)
		result.push_back(MakeElement(it->id));
	return result;
}

inline
Window WebDriver::MakeWindow(const std::string& handle) const {
	return Window(handle,
		session_.resource.GetSubResource("window").GetSubResource(handle)
		);
}	

inline
Element WebDriver::MakeElement(const std::string& id) const {
	return Element(
		id,
		session_.resource.GetSubResource("element").GetSubResource(id),
		this
		);
}	

} // namespace webdriverxx
