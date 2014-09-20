#ifndef WEBDRIVERXX_TYPES_H
#define WEBDRIVERXX_TYPES_H

#include "capabilities.h"
#include <string>
#include <vector>

namespace webdriverxx {

struct SessionInformation {
	std::string id;
	Capabilities capabilities;
};
typedef std::vector<SessionInformation> SessionsInformation;

} // namespace webdriverxx

#endif
