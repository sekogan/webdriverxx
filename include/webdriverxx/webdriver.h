#ifndef WEBDRIVERXX_WEBDRIVER_H
#define WEBDRIVERXX_WEBDRIVER_H

#include "capabilities.h"
#include "detail/resource.h"
#include "detail/http_connection.h"
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
	{}

	picojson::value GetStatus() const
	{
		return server_root_.Get("status").get("value");
	}

private:
	struct Session
	{
		detail::Resource session;
		Capabilities capabilities;

		Session(
			const detail::Resource& session,
			const Capabilities& capabilities
			)
			: session(session)
			, capabilities(capabilities)
		{}
	};

	Session CreateSession(
		const Capabilities& required,
		const Capabilities& desired
		) const
	{
		using namespace picojson;
		value requestJson = value(object());
		value& requiredJson = requestJson.get<object>()["requiredCapabilities"] = value(object());
		value& desiredJson = requestJson.get<object>()["desiredCapabilities"] = value(object());
		ToJson(required, requiredJson);
		ToJson(desired, desiredJson);

		const value& response = server_root_.Post("session", requestJson);

		Check(response.get("sessionId").is<std::string>(), "Cannot create session: ID is not a string");
		Check(response.get("value").is<object>(), "Cannot create session: capabilities is not an object");
		const std::string sessionId = response.get("sessionId").get<std::string>();
		
		Session session(
			server_root_.GetSubResource("session").GetSubResource(sessionId.c_str()),
			Capabilities()
			);
		FromJson(response.get("value"), session.capabilities);
		return session;
	}

	static
	void ToJson(const Capabilities& c, picojson::value& json)
	{
		picojson::object& object = json.get<picojson::object>();
		std::transform(c.items.begin(), c.items.end(), std::inserter(object, object.end()), ToJsonStringPair);
	}

	static
	picojson::object::value_type ToJsonStringPair(const std::pair<std::string, std::string>& pair)
	{
		return std::make_pair(pair.first, picojson::value(pair.second));
	}

	static
	void FromJson(const picojson::value& json, Capabilities& c)
	{
		const picojson::object& object = json.get<picojson::object>();
		std::transform(object.begin(), object.end(), std::inserter(c.items, c.items.end()), FromJsonStringPair);
	}

	static
	std::pair<std::string, std::string> FromJsonStringPair(const picojson::object::value_type& pair)
	{
		return std::make_pair(pair.first, pair.second.to_str());
	}

	static
	void Check(bool condition, const char* error_message)
	{
		if (!condition)
			throw WebDriverException(error_message);
	}

private:
	WebDriver(WebDriver&);
	WebDriver& operator=(WebDriver&);

private:
	const detail::HttpConnection http_connection_;
	const detail::Resource server_root_;
	const Session session_;
};

} // namespace webdriverxx

#endif
