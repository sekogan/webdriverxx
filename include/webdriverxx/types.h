#ifndef WEBDRIVERXX_TYPES_H
#define WEBDRIVERXX_TYPES_H

namespace webdriverxx {

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
