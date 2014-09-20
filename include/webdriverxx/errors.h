#ifndef WEBDRIVERXX_ERRORS_H
#define WEBDRIVERXX_ERRORS_H

#include <stdexcept>
#include <string>

namespace webdriverxx {

struct WebDriverException : std::runtime_error {
	explicit WebDriverException(const std::string& message)
		: std::runtime_error(message) {}
};

} // namespace webdriverxx

#endif
