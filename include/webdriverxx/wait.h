#ifndef WEBDRIVERXX_WAIT_H
#define WEBDRIVERXX_WAIT_H

#include "detail/error_handling.h"
#include "detail/time.h"
#include <string>

namespace webdriverxx {

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

template<typename G>
inline
auto WaitUntil(G getter,
	Duration timeoutMs = 5000, Duration intervalMs = 50
	) -> decltype(getter()) {
	return WaitForValue([&getter]() -> decltype(getter()) {
			const auto value = getter();
			if (!value)
				throw WebDriverException("Value is falsy");
			return value;
		}, timeoutMs, intervalMs);
}

} // namespace webdriverxx

#endif
