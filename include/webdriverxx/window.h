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
		try {
			resource_.Get("size"); // TODO
			return Size();
		} catch (const std::exception&) {
			return detail::Rethrow("while getting window size", Size());
		}
	}

	void Maximize() const {
		try {
			resource_.Post("maximize");
		} catch (const std::exception&) {
			detail::Rethrow("while maximizing window");
		}
	}

private:
	detail::Resource resource_;
};

} // namespace webdriverxx

#endif
