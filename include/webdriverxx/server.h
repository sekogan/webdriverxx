#ifndef WEBDRIVERXX_SERVER_H
#define WEBDRIVERXX_SERVER_H

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
class Server { // noncopyable
public:
	explicit Server(const std::string& url = kDefaultUrl);
	virtual ~Server() {}

	picojson::object GetStatus() const;

	// Returns existing sessions.
	std::vector<Session> GetSessions() const;

	// Creates new session. All sessions created with this member should be
	// deleted with Session::DeleteSession().
	Session CreateSession(
		const Capabilities& desired,
		const Capabilities& required
		) const;

private:
	Session MakeSession(const std::string& id, const Capabilities& capabilities) const;

	Server(Server&);
	Server& operator=(Server&);

private:
	const detail::HttpConnection http_connection_;
	const detail::RootResource resource_;
};

} // namespace webdriverxx

#include "server.inl"

#endif
