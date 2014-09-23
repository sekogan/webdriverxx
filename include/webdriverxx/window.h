#ifndef WEBDRIVERXX_WINDOW_H
#define WEBDRIVERXX_WINDOW_H

#include "types.h"
#include "detail/resource.h"
#include "detail/conversions.h"
#include <string>

namespace webdriverxx {

class Window { // copyable
public:
	Window(const std::string& handle, const detail::Resource& resource)
		: handle_(handle)
		, resource_(resource) {}

	std::string GetHandle() const {
		return handle_;
	}

	Size GetSize() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		return detail::FromJson<Size>(resource_.Get("size"));
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	void SetSize(const Size& size) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		resource_.Post("size", detail::ToJson(size));
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	Point GetPosition() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		return detail::FromJson<Point>(resource_.Get("position"));
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	void SetPosition(const Point& position) const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		resource_.Post("position", detail::ToJson(position));
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

	void Maximize() const {
		WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN()
		resource_.Post("maximize");
		WEBDRIVERXX_FUNCTION_CONTEXT_END()
	}

private:
	std::string handle_;
	detail::Resource resource_;
};

} // namespace webdriverxx

#endif
