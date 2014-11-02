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

template<typename T>
bool BoolCast(T value) {
	return !!value;
}

} // namespace detail
} // namespace webdriverxx

#if __cplusplus >= 201103L
	#define WEBDRIVERXX_CURRENT_FUNCTION __func__
#else
	#define WEBDRIVERXX_CURRENT_FUNCTION __FUNCTION__
#endif

#define WEBDRIVERXX_FUNCTION_CONTEXT_BEGIN() \
	try {

#define WEBDRIVERXX_FUNCTION_CONTEXT_END() \
	} catch (const std::exception& e) { \
		throw ::webdriverxx::WebDriverException(std::string(e.what()) \
			+ " called from " + WEBDRIVERXX_CURRENT_FUNCTION); \
	}

#define WEBDRIVERXX_FUNCTION_CONTEXT_END_EX(details) \
	} catch (const std::exception& e) { \
		throw ::webdriverxx::WebDriverException(std::string(e.what()) \
			+ " called from " + WEBDRIVERXX_CURRENT_FUNCTION \
			+ " (" + std::string(details) + ")"); \
	}

#define WEBDRIVERXX_THROW(message) \
	throw ::webdriverxx::WebDriverException(::webdriverxx::detail::Fmt() \
		<< std::string(message) \
		<< " at line " << __LINE__ \
		<< ", file " << __FILE__ \
		)

#define WEBDRIVERXX_CHECK(pred, message) \
	for (;!detail::BoolCast(pred);) \
		WEBDRIVERXX_THROW(message)

#endif
