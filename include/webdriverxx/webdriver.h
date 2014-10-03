#ifndef WEBDRIVERXX_WEBDRIVER_H
#define WEBDRIVERXX_WEBDRIVER_H

#include "client.h"
#include "session.h"

namespace webdriverxx {

// The main class for interactions with a server. Automatically connects to a server,
// creates and deletes a session and gives access to session's API.
class WebDriver // copyable
	: public Client
	, public Session {
public:
	explicit WebDriver(
		const std::string& url = kDefaultUrl,
		const Capabilities& desired = Capabilities(),
		const Capabilities& required = Capabilities()
		)
		: Client(url)
		, Session(CreateSession(desired, required))
	{}
};

} // namespace webdriverxx

#endif
