#ifndef WEBDRIVERXX_WAIT_MATCH_H
#define WEBDRIVERXX_WAIT_MATCH_H

#include "wait.h"
#include "detail/to_string.h"
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
		s << ", actual: ";
		ToStream(value, s);
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
		return detail::Fmt() << "Value " << ToString(value) << " does not match predicate";
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

// Waits until a value returned by a getter satisfies a supplied matcher.
// Returns that value or throws exception on timeout.
// Getter is a function or function-like object that returns some copyable value.
// Matcher can be a predicate or a Google Mock matcher (if Google Mock matchers are enabled).
template<typename Getter, typename Matcher>
inline
auto WaitForMatch(
	Getter getter,
	Matcher matcher,
	Duration timeoutMs = 5000,
	Duration intervalMs = 50
	) -> decltype(getter()) {
	typedef decltype(getter()) Value;
	const auto& adapter = detail::SelectMakeMatcherAdapter<Value>(matcher,
		typename std::is_same<void,decltype(MakeMatcherAdapter<Value>(matcher))>::type());
	return detail::Wait<Value>([&getter, &adapter](std::string* description) -> std::unique_ptr<Value> {
			auto value_ptr = detail::TryToCallGetter<Value>(getter, description);
			if (value_ptr && !adapter.Apply(*value_ptr)) {
				if (description)
					*description = adapter.DescribeMismatch(*value_ptr);
				value_ptr.reset();
			}
			return value_ptr;
		}, timeoutMs, intervalMs);
}

} // namespace webdriverxx

#endif
