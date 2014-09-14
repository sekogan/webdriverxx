#ifndef WEBDRIVERXX_CAPABILITIES_H
#define WEBDRIVERXX_CAPABILITIES_H

#include <map>
#include <string>

namespace webdriverxx {

struct Capabilities
{
	std::map<std::string, std::string> items;
};

} // namespace webdriverxx

#endif
