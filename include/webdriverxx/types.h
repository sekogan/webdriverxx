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

struct Size {
	int width;
	int height;
	Size() : width(0), height(0) {}
};

struct Point {
	int x;
	int y;
	Point() : x(0), y(0) {}
};

} // namespace webdriverxx

#endif
