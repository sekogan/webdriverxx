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
#include "detail/error_handling.h"
#include "detail/conversions.h"
#include <string>

namespace webdriverxx {

const char *const kDefaultUrl = "http://localhost:4444/wd/hub/";

// Gives access to some API functions but doesn't create session
class BasicWebDriver {
public:
	explicit BasicWebDriver(const std::string& url = kDefaultUrl)
		: server_root_(&http_connection_, url) {}

	picojson::object GetStatus() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		const picojson::value value = server_root_.Get("status").get("value");
		WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Value is not an object");
		return value.get<picojson::object>();
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}
	
	SessionsInformation GetSessions() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		return detail::FromJsonArray<SessionInformation>(
			server_root_.Get("sessions").get("value")
			);
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

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

private:
	BasicWebDriver(BasicWebDriver&);
	BasicWebDriver& operator=(BasicWebDriver&);

private:
	const detail::HttpConnection http_connection_;
	const detail::ServerRoot server_root_;
};

class WebDriver
	: public BasicWebDriver
	, private detail::Finder {
public:
	explicit WebDriver(
		const std::string& url = kDefaultUrl,
		const Capabilities& required = Capabilities(),
		const Capabilities& desired = Capabilities()
		)
		: BasicWebDriver(url)
		, session_(CreateSession(required, desired))
		, session_deleter_(session_.resource) {}

	const Capabilities& GetCapabilities() const {
		return session_.capabilities;
	}

	Window GetCurrentWindow() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		return MakeWindow(detail::FromJson<std::string>(
			session_.resource.Get("window_handle")
			));
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	void CloseCurrentWindow() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		session_.resource.Delete("window");
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	void SetFocusToWindow(const std::string& name_or_handle) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		session_.resource.Post("window",
			detail::JsonObject().With("name", name_or_handle).Build()
			);
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	std::vector<Window> GetWindows() const {
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

	std::string GetUrl() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		return session_.resource.Get("url").to_str();
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	void Navigate(const std::string& url) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		session_.resource.Post("url", detail::JsonObject().With("url", url).Build());
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	Element FindElement(const By& by) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		return FindElement(by, session_.resource);
		WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
			<< "strategy: " << by.GetStrategy()
			<< ", value: " << by.GetValue()
			)
	}

	std::vector<Element> FindElements(const By& by) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		return FindElements(by, session_.resource);
		WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(detail::Fmt()
			<< "strategy: " << by.GetStrategy()
			<< ", value: " << by.GetValue()
			)
	}

private:
	Element FindElement(
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

	std::vector<Element> FindElements(
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

	Window MakeWindow(const std::string& handle) const {
		return Window(handle,
			session_.resource.GetSubResource("window").GetSubResource(handle)
			);
	}	

	Element MakeElement(const std::string& id) const {
		return Element(
			id,
			session_.resource.GetSubResource("element").GetSubResource(id),
			this
			);
	}	

private:
	const Session session_;
	const detail::AutoResourceDeleter session_deleter_;
};

} // namespace webdriverxx

#endif
