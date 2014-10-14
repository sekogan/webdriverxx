#ifndef WEBDRIVERXX_WAIT_MATCH_H
#define WEBDRIVERXX_WAIT_MATCH_H

#include "wait.h"
#include "detail/to_string.h"
#include "detail/meta.h"
#include <type_traits>

#ifdef WEBDRIVERXX_ENABLE_GMOCK_MATCHERS

#include <gmock/gmock-matchers.h>
#include <sstream>

namespace webdriverxx {
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
inline
detail::GMockMatcherAdapter<T,M> MakeMatcherAdapter(const M& matcher, typename std::enable_if<std::is_convertible<M,::testing::Matcher<T>>::value>::type* = nullptr) {
	return detail::GMockMatcherAdapter<T,M>(matcher);
};

} // namespace webdriverxx

#endif // WEBDRIVERXX_ENABLE_GMOCK_MATCHERS

namespace webdriverxx {
namespace detail {

template<typename T, typename P>
class PredicateMatcherAdapter {
public:
	explicit PredicateMatcherAdapter(P& predicate) : predicate_(&predicate) {}

	bool Apply(const T& value) const {
		return (*predicate_)(value);
	}

	std::string DescribeMismatch(const T& value) const {
		return detail::Fmt() << "Value " << value << " does not match predicate";
	}

private:
	P* predicate_;
};

} // detail

template<typename T>
void MakeMatcherAdapter(...);

namespace detail {

template<typename T, typename M>
inline
PredicateMatcherAdapter<T,M> SelectMakeMatcherAdapter(M& matcher, std::true_type /*no_custom_adapters*/) {
	return PredicateMatcherAdapter<T,M>(matcher);
}

template<typename T, typename M>
inline
auto SelectMakeMatcherAdapter(const M& matcher, std::false_type /*no_custom_adapters*/) -> decltype(MakeMatcherAdapter<T>(matcher)) {
	return MakeMatcherAdapter<T>(matcher);
}

} // detail

template<typename G, typename M>
inline
auto WaitForMatch(const G& getter, const M& matcher,
	Duration timeoutMs = 5000, Duration intervalMs = 50
	) -> decltype(getter()) {
	typedef decltype(getter()) T;
	const auto& adapter = detail::SelectMakeMatcherAdapter<T>(matcher, typename std::is_same<void,decltype(MakeMatcherAdapter<T>(matcher))>::type());
	return WaitForValue([&getter, &adapter]() -> T {
			const auto value = getter();
			if (!adapter.Apply(value))
				throw WebDriverException(adapter.DescribeMismatch(value));
			return value;
		}, timeoutMs, intervalMs);
}

} // namespace webdriverxx

#endif
