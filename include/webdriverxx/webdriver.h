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
		const Capabilities& required = Capabilities(),
		const Capabilities& desired = Capabilities()
		);
	~WebDriver();
};

inline
WebDriver::WebDriver(
	const std::string& url,
	const Capabilities& required,
	const Capabilities& desired
	)
	: Server(url)
	, Session(CreateSession(required, desired)) {}

inline
WebDriver::~WebDriver() {
	try {
		DeleteSession();
	} catch (const std::exception&) {}
}

} // namespace webdriverxx

#endif
