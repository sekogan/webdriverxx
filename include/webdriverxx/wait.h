#ifndef WEBDRIVERXX_WAIT_H
#define WEBDRIVERXX_WAIT_H

#include "detail/error_handling.h"
#include "detail/time.h"
#include <string>

#ifdef USE_GMOCK_MATCHERS
#include <gmock/gmock-matchers.h>
#include <type_traits>
#include <sstream>
#endif // USE_GMOCK_MATCHERS

namespace webdriverxx {

template<typename G>
inline
auto WaitFor(
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

namespace detail {

template<typename T, typename P>
class PredicateMatcherAdapter {
public:
	explicit PredicateMatcherAdapter(P predicate) : predicate_(predicate) {}

	bool Apply(const T& value) const {
		return predicate_(value);
	}

	std::string DescribeMismatch(const T& value) const {
		return detail::Fmt() << "Value " << value << " does not match predicate";
	}

private:
	P predicate_;
};

} // detail

template<class T, class M, class Enable = void>
struct MakeMatcherAdapter {
	static detail::PredicateMatcherAdapter<T,M> Apply(M predicate) {
		return detail::PredicateMatcherAdapter<T,M>(predicate);
	}
};

namespace detail {

template <class T, class M>
inline
auto ApplyMakeMatcherAdapter(M matcher) -> decltype(MakeMatcherAdapter<T,M>::Apply(*static_cast<M*>(nullptr))) {
	return MakeMatcherAdapter<T,M>::Apply(matcher);
}

} // detail

template<typename G, typename M>
inline
auto WaitUntil(G getter, M matcher,
	Duration timeoutMs = 5000, Duration intervalMs = 50
	) -> decltype(getter()) {
	typedef decltype(getter()) T;
	auto adapter = detail::ApplyMakeMatcherAdapter<T>(matcher);
	return WaitFor([&getter, &adapter]() -> T {
			const auto value = getter();
			if (!adapter.Apply(value))
				throw WebDriverException(adapter.DescribeMismatch(value));
			return value;
		}, timeoutMs, intervalMs);
}

#ifdef USE_GMOCK_MATCHERS

namespace detail {

template<typename T, typename M>
class GMockMatcherAdapter {
public:
	explicit GMockMatcherAdapter(::testing::Matcher<T> matcher) : matcher_(matcher) {}

	bool Apply(T value) const {
		return matcher_.Matches(value);
	}

	std::string DescribeMismatch(T value) const {
		std::ostringstream s;
		matcher_.ExplainMatchResultTo(value, &s);
		return s.str();
	}

private:
	::testing::Matcher<T> matcher_;
};

} // detail

template<class T, class M>
struct MakeMatcherAdapter<T, M, typename std::enable_if<std::is_convertible<M,::testing::Matcher<T>>::value>::type> {
	static detail::GMockMatcherAdapter<T,M> Apply(M matcher) {
		return detail::GMockMatcherAdapter<T,M>(matcher);
	}
};

#endif // USE_GMOCK_MATCHERS

} // namespace webdriverxx

#endif
