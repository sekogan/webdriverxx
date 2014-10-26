#ifndef WEBDRIVERXX_WAIT_H
#define WEBDRIVERXX_WAIT_H

#include "detail/error_handling.h"
#include "detail/time.h"
#include "detail/to_string.h"
#include <string>
#include <memory>

namespace webdriverxx {
namespace detail {

template<typename Value, typename DescriptiveGetter>
Value Wait(
	DescriptiveGetter getter,
	Duration timeoutMs = 5000,
	Duration intervalMs = 50
	) {
	const TimePoint timeout = detail::Now() + timeoutMs;
	for (;;) {
		const auto value_ptr = getter(nullptr);
		if (value_ptr)
			return *value_ptr;
		if (detail::Now() >= timeout) {
			std::string description;
			const auto value_ptr = getter(&description);
			if (value_ptr)
				return *value_ptr;
			throw WebDriverException(detail::Fmt()
				<< "Timeout after " << timeoutMs << "ms of waiting, last attempt returned: "
				<< description
				);
		}
		detail::Sleep(intervalMs);
	}
}

template<typename Value, typename Getter>
std::unique_ptr<Value> TryToCallGetter(Getter getter, std::string* description) {
	std::unique_ptr<Value> value_ptr;
	try {
		value_ptr.reset(new Value(getter()));
	} catch (const std::exception& e) {
		if (description)
			*description = e.what();
	}
	return value_ptr;
}

} // namespace detail

// Waits for a value returned by a supplied getter.
// Returns that value or throws exception on timeout.
// Getter is a function or function-like object that returns some copyable value.
template<typename Getter>
inline
auto WaitForValue(
	Getter getter,
	Duration timeoutMs = 5000,
	Duration intervalMs = 50
	) -> decltype(getter()) {
	typedef decltype(getter()) Value;
	return detail::Wait<Value>(
		[&getter](std::string* description) {
			return detail::TryToCallGetter<Value>(getter, description);
		},
		timeoutMs, intervalMs);
}

// Waits until a truthy value is returned by a supplied getter.
// Returns that value or throws exception on timeout.
// Getter is a function or function-like object that returns some copyable value.
template<typename Getter>
inline
auto WaitUntil(
	Getter getter,
	Duration timeoutMs = 5000,
	Duration intervalMs = 50
	) -> decltype(getter()) {
	typedef decltype(getter()) Value;
	return detail::Wait<Value>(
		[&getter](std::string* description) -> std::unique_ptr<Value> {
			auto value_ptr = detail::TryToCallGetter<Value>(getter, description);
			if (!value_ptr || !!*value_ptr)
				return value_ptr;
			if (description)
				*description = "Value is falsy";
			value_ptr.reset();
			return value_ptr;
		}, timeoutMs, intervalMs);
}

} // namespace webdriverxx

#endif
