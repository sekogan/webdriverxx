#ifndef WEBDRIVERXX_WINDOW_H
#define WEBDRIVERXX_WINDOW_H

#include "detail/resource.h"

namespace webdriverxx {

struct Size {
	int width;
	int height;
};

class Window { // copyable
public:
	explicit Window(const detail::Resource& resource)
		: resource_(resource) {}

	Size GetSize() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		resource_.Get("size"); // TODO
		return Size();
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	void Maximize() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		resource_.Post("maximize");
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

private:
	detail::Resource resource_;
};

} // namespace webdriverxx

#endif
