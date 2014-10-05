#ifndef WEBDRIVERXX_CLIENT_H
#define WEBDRIVERXX_CLIENT_H

#include "session.h"
#include "capabilities.h"
#include "detail/resource.h"
#include "detail/http_connection.h"
#include <picojson.h>
#include <string>
#include <vector>

namespace webdriverxx {

const char *const kDefaultUrl = "http://localhost:4444/wd/hub/";

// Gives low level access to server's resources. You normally should not use it. 
class Client { // copyable
public:
	explicit Client(const std::string& url = kDefaultUrl);
	virtual ~Client() {}

	picojson::object GetStatus() const;

	// Returns existing sessions.
	std::vector<Session> GetSessions() const;

	// Creates new session.
	Session CreateSession(
		const Capabilities& desired,
		const Capabilities& required
		) const;

private:
	Session MakeSession(
		const std::string& id,
		const Capabilities& capabilities,
		detail::Resource::Ownership mode
		) const;

private:
	detail::Shared<detail::Resource> resource_;
};

} // namespace webdriverxx

#include "client.inl"

#endif
