#ifndef WEBDRIVERXX_DETAIL_ERROR_HANDLING_H
#define WEBDRIVERXX_DETAIL_ERROR_HANDLING_H

#include "../errors.h"
#include <string>
#include <sstream>

namespace webdriverxx {
namespace detail {

class Fmt {
public:
	template<typename T>
	Fmt& operator << (const T& value) {
		stream_ << value;
		return *this;
	}

	operator std::string() const {
		return stream_.str();
	}

private:
	std::ostringstream stream_;
};

inline
void Check(bool invariant, const char* error_message) {
	if (!invariant)
		throw WebDriverException(error_message);
}

inline
void Rethrow(const std::string& context) {
	try {
		throw;
	} catch (const std::exception& e) {
		throw WebDriverException(context + " -> " + e.what());
	}
}

// For functions that return something to suppress compiler warning
template<typename ReturnType>
inline
ReturnType Rethrow(const std::string& context, const ReturnType& return_value) {
	Rethrow(context);
	return return_value;
}

} // namespace detail
} // namespace webdriverxx

#endif
