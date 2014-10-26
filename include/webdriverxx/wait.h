#ifndef WEBDRIVERXX_WAIT_H
#define WEBDRIVERXX_WAIT_H

#include "detail/error_handling.h"
#include "detail/time.h"
#include "detail/to_string.h"
#include <string>

namespace webdriverxx {

// Waits for a value returned by a supplied getter.
// Returns that value or throws exception on timeout.
// Getter is a function or function-like object that returns some copyable value.
template<typename G>
inline
auto WaitForValue(
	G getter,
	Duration timeoutMs = 5000, Duration intervalMs = 50
	) -> decltype(getter()) {
	const TimePoint timeout = detail::Now() + timeoutMs;
	for (;;) {
		try {
			return getter();
		} catch (const std::exception& e) {
			if (detail::Now() >= timeout) {
				throw WebDriverException(detail::Fmt()
					<< "Timeout after " << timeoutMs << "ms of waiting, last attempt returned: "
					<< e.what()
					);
			}
		}
		detail::Sleep(intervalMs);
	}
}

// Waits until a truthy value is returned by a supplied getter.
// Returns that value or throws exception on timeout.
// Getter is a function or function-like object that returns some copyable value.
template<typename G>
inline
auto WaitUntil(
	G getter,
	Duration timeoutMs = 5000, Duration intervalMs = 50
	) -> decltype(getter()) {
	return WaitForValue([&getter]() -> decltype(getter()) {
			const auto value = getter();
			if (!value)
				throw WebDriverException(
					detail::Fmt() << "Value " << detail::ToString(value) << " is falsy"
					);
			return value;
		}, timeoutMs, intervalMs);
}

} // namespace webdriverxx

#endif
