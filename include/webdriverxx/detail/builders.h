#ifndef WEBDRIVERXX_DETAIL_BUILDERS_H
#define WEBDRIVERXX_DETAIL_BUILDERS_H

#include <picojson.h>

namespace webdriverxx {
namespace detail {

class JsonObject { // copyable
public:
	JsonObject()
		: value_(picojson::object()) {}

	JsonObject& With(const std::string& name, const picojson::value& value) {
		value_.get<picojson::object>()[name] = value;
		return *this;
	}

	template<typename T>
	JsonObject& With(const std::string& name, const T& value) {
		value_.get<picojson::object>()[name] = picojson::value(value);
		return *this;
	}

	const picojson::value& Build() const {
		return value_;
	}

private:
	picojson::value value_;
};

} // namespace detail
} // namespace webdriverxx

#endif
