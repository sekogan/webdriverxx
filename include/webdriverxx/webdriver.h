#ifndef WEBDRIVERXX_WEBDRIVER_H
#define WEBDRIVERXX_WEBDRIVER_H

#include "capabilities.h"
#include "detail/resource.h"
#include "detail/http_connection.h"
#include "detail/error_handling.h"
#include <string>
#include <algorithm>

namespace webdriverxx {

const char *const kDefaultUrl = "http://localhost:4444/wd/hub/";

class WebDriver
{
public:
	explicit WebDriver(
		const std::string& url = kDefaultUrl,
		const Capabilities& required = Capabilities(),
		const Capabilities& desired = Capabilities()
		)
		: server_root_(&http_connection_, url)
		, session_(CreateSession(required, desired))
		, session_deleter_(session_.resource)
	{}

	picojson::object GetStatus() const
	{
		try
		{
			const picojson::value& value = server_root_.Get("status").get("value");
			detail::Check(value.is<picojson::object>(), "Value is not an object");
			return server_root_.Get("status").get("value").get<picojson::object>();
		}
		catch (std::exception&)
		{
			return detail::Rethrow("Cannot get status", picojson::object());
		}
	}

private:
	struct Session
	{
		detail::Resource resource;
		Capabilities capabilities;

		explicit Session(
			const detail::Resource& resource,
			const Capabilities& capabilities = Capabilities()
			)
			: resource(resource)
			, capabilities(capabilities)
		{}
	};

	Session CreateSession(
		const Capabilities& required,
		const Capabilities& desired
		) const
	{
		try
		{
			using namespace picojson;
			value requestJson = value(object());
			requestJson.get<object>()["requiredCapabilities"] = value(required.Get());
			requestJson.get<object>()["desiredCapabilities"] = value(desired.Get());

			const value& response = server_root_.Post("session", requestJson);

			detail::Check(response.get("sessionId").is<std::string>(), "Session ID is not a string");
			detail::Check(response.get("value").is<object>(), "Capabilities is not an object");
			
			const std::string& sessionId = response.get("sessionId").to_str();
			
			return Session(
				server_root_.GetSubResource("session").GetSubResource(sessionId),
				Capabilities(response.get("value").get<object>())
				);
		}
		catch (const std::exception&)
		{
			return detail::Rethrow("Cannot create session", Session(server_root_));
		}
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
