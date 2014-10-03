#include "conversions.h"
#include "detail/shared.h"
#include "detail/error_handling.h"

namespace webdriverxx {
namespace detail {

struct CapabilitiesAccess {
	static
	Capabilities Construct(const picojson::object& object) {
		return Capabilities(object);
	}

	static
	const picojson::object& GetJsonObject(const Capabilities& capabilities) {
		return capabilities.GetJsonObject();
	}
};

struct SessionRef {
	std::string id;
	Capabilities capabilities;
};

} // namespace detail

template<>
struct FromJsonImpl<detail::SessionRef> {
	static detail::SessionRef Convert(const picojson::value& value) {
		WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Session information is not an object");
		detail::SessionRef result;
		result.id = value.get("sessionId").to_str();
		if (value.get("capabilities").is<picojson::object>())
			result.capabilities = detail::CapabilitiesAccess::Construct(value.get("capabilities").get<picojson::object>());
		return result;
	}
};

inline
Client::Client(const std::string& url)
	: resource_(detail::Shared<detail::IHttpClient>(new detail::HttpConnection), url)
{}

inline
picojson::object Client::GetStatus() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	const picojson::value value = resource_.Get("status").get("value");
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "Value is not an object");
	return value.get<picojson::object>();
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
std::vector<Session> Client::GetSessions() const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	const std::vector<detail::SessionRef> sessions =
		FromJsonArray<detail::SessionRef>(
			resource_.Get("sessions").get("value")
			);
	std::vector<Session> result;
	for (std::vector<detail::SessionRef>::const_iterator it = sessions.begin();
		it != sessions.end(); ++it)
		result.push_back(MakeSession(it->id, it->capabilities, Session::IsObserver));
	return result;
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
Session Client::CreateSession(
	const Capabilities& desired,
	const Capabilities& required
	) const {
	WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
	const picojson::value& response = resource_.Post("session",
		JsonObject()
			.With("desiredCapabilities", detail::CapabilitiesAccess::GetJsonObject(desired))
			.With("requiredCapabilities", detail::CapabilitiesAccess::GetJsonObject(required))
			.Build()
			);

	WEBDRIVERXX_CHECK(response.get("sessionId").is<std::string>(), "Session ID is not a string");
	WEBDRIVERXX_CHECK(response.get("value").is<picojson::object>(), "Capabilities is not an object");
	
	const std::string sessionId = response.get("sessionId").to_str();
	const Capabilities capabilities = detail::CapabilitiesAccess::Construct(
		response.get("value").get<picojson::object>()
		);
	
	return MakeSession(sessionId, capabilities, Session::IsOwner);
	WEBDRIVERXX_FUNCTION_CONTEXT_END()
}

inline
Session Client::MakeSession(
	const std::string& id,
	const Capabilities& capabilities,
	Session::Ownership mode
	) const {
	return Session(
		resource_
			.GetSubResource<detail::Resource>("session")
			.GetSubResource(id),
		capabilities,
		mode
		);
}

} // namespace webdriverxx
