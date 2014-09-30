#ifndef WEBDRIVERXX_WEBDRIVER_H
#define WEBDRIVERXX_WEBDRIVER_H

#include "server.h"
#include "session.h"

namespace webdriverxx {

// The main class for interactions with a server. Automatically connects to a server,
// creates and deletes a session and gives access to session's API.
class WebDriver // noncopyable
	: public Server
	, public Session {
public:
	explicit WebDriver(
		const std::string& url = kDefaultUrl,
		const Capabilities& desired = Capabilities(),
		const Capabilities& required = Capabilities()
		);
	~WebDriver();
};

inline
WebDriver::WebDriver(
	const std::string& url,
	const Capabilities& desired,
	const Capabilities& required
	)
	: Server(url)
	, Session(CreateSession(desired, required)) {}

inline
WebDriver::~WebDriver() {
	try {
		DeleteSession();
	} catch (const std::exception&) {}
}

} // namespace webdriverxx

#endif
