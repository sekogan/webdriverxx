#ifndef WEBDRIVERXX_WEBDRIVER_H
#define WEBDRIVERXX_WEBDRIVER_H

#include "window.h"
#include "capabilities.h"
#include "types.h"
#include "detail/resource.h"
#include "detail/http_connection.h"
#include "detail/error_handling.h"
#include "detail/conversions.h"
#include "detail/builders.h"
#include <string>

namespace webdriverxx {

const char *const kDefaultUrl = "http://localhost:4444/wd/hub/";

class WebDriver {
public:
	explicit WebDriver(
		const std::string& url = kDefaultUrl,
		const Capabilities& required = Capabilities(),
		const Capabilities& desired = Capabilities()
		)
		: server_root_(&http_connection_, url)
		, session_(CreateSession(required, desired))
		, session_deleter_(session_.resource) {}

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

	const Capabilities& GetCapabilities() const {
		return session_.capabilities;
	}

	std::vector<Window> GetWindows() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		const std::vector<std::string> handles =
			detail::FromJsonArray<std::string>(
				session_.resource.Get("window_handles").get("value")
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
		return session_.resource.Get("url").get("value").to_str();
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	void Navigate(const std::string& url) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		session_.resource.Post("url", detail::JsonObject().With("url", url).Build());
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

private:
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
			server_root_.GetSubResource("session").GetSubResource(sessionId),
			detail::CapabilitiesAccess::Construct(response.get("value").get<picojson::object>())
			);
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	Window MakeWindow(const std::string& handle) const
	{
		return Window(session_.resource.GetSubResource("window").GetSubResource(handle));
	}	

private:
	WebDriver(WebDriver&);
	WebDriver& operator=(WebDriver&);

private:
	const detail::HttpConnection http_connection_;
	const detail::Resource server_root_;
	const Session session_;
	const detail::AutoResourceDeleter session_deleter_;
};

} // namespace webdriverxx

#endif
