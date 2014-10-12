#ifndef WEBDRIVERXX_WAIT_MATCH_H
#define WEBDRIVERXX_WAIT_MATCH_H

#include "wait.h"
#include "detail/to_string.h"
#include "detail/meta.h"

#ifdef WEBDRIVERXX_ENABLE_GMOCK_MATCHERS
#include <gmock/gmock-matchers.h>
#include <type_traits>
#include <sstream>
#endif // WEBDRIVERXX_ENABLE_GMOCK_MATCHERS

namespace webdriverxx {
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
auto CallMakeMatcherAdapter(M matcher) -> decltype(MakeMatcherAdapter<T,M>::Apply(ValueRef<M>())) {
	return MakeMatcherAdapter<T,M>::Apply(matcher);
}

} // detail

template<typename G, typename M>
inline
auto WaitForMatch(G getter, M matcher,
	Duration timeoutMs = 5000, Duration intervalMs = 50
	) -> decltype(getter()) {
	typedef decltype(getter()) T;
	auto adapter = detail::CallMakeMatcherAdapter<T>(matcher);
	return WaitForValue([&getter, &adapter]() -> T {
			const auto value = getter();
			if (!adapter.Apply(value))
				throw WebDriverException(adapter.DescribeMismatch(value));
			return value;
		}, timeoutMs, intervalMs);
}

#ifdef WEBDRIVERXX_ENABLE_GMOCK_MATCHERS

namespace detail {

template<typename T, typename M>
class GMockMatcherAdapter {
public:
	explicit GMockMatcherAdapter(::testing::Matcher<T> matcher) : matcher_(matcher) {}

	bool Apply(const T& value) const {
		return matcher_.Matches(value);
	}

	std::string DescribeMismatch(const T& value) const {
		std::ostringstream s;
		s << "Expected: ";
		matcher_.DescribeTo(&s);
		s << ", actual: " << ToString(value);
		const auto mismatch_details = GetMismatchDetails(value);
		if (!mismatch_details.empty())
			 s << ", " << mismatch_details;
		return s.str();
	}

private:
	std::string GetMismatchDetails(const T& value) const {
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

#endif // WEBDRIVERXX_ENABLE_GMOCK_MATCHERS

} // namespace webdriverxx

#endif
