#ifndef WEBDRIVERXX_WAIT_H
#define WEBDRIVERXX_WAIT_H

#include "detail/error_handling.h"
#include "detail/time.h"
#include <string>

namespace webdriverxx {

template<typename Getter>
inline
auto WaitForValue(
	Getter getter,
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

template<typename Matcher>
class MatcherAdapter {
public:
	explicit MatcherAdapter(Matcher matcher) : matcher_(matcher) {}

	template<typename Value>
	bool Apply(const Value& value) const {
		return matcher_(value);
	}

	template<typename Value>
	std::string DescribeMismatch(const Value& value) {
		return detail::Fmt() << "Value " << value << " does not match predicate";
	}

private:
	Matcher matcher_;
};

template<typename Getter, typename Matcher>
inline
auto WaitForMatch(Getter getter, Matcher matcher,
	Duration timeoutMs = 5000, Duration intervalMs = 50
	) -> decltype(getter()) {
	MatcherAdapter<Matcher> adapter(matcher);
	return WaitForValue([&getter, &adapter]() -> decltype(getter()) {
			const auto value = getter();
			if (!adapter.Apply(value))
				throw WebDriverException(adapter.DescribeMismatch(value));
			return value;
		}, timeoutMs, intervalMs);
}

} // namespace webdriverxx

#endif
