#include "environment.h"
#include <webdriverxx/wait.h>
#include <gtest/gtest.h>
#include <type_traits>

using namespace webdriverxx;

template<
	typename Getter, typename Matcher,
	typename Result = typename std::result_of<Getter()>::type
	>
inline
Result WaitFor(Getter getter, Matcher,
	int timeoutMilliseconds = 5000, int intervalMilliseconds = 50
	) {
	return getter();
}

int Function() { return 123; }

struct Functor {
	int operator () () const {
		return 456;
	}
};

TEST(Wait, DeducesResultType) {
	int matcher = 0;
	ASSERT_EQ(123, WaitFor(Function, matcher));
	ASSERT_EQ(456, WaitFor(Functor(), matcher));
	ASSERT_EQ(789, WaitFor([]{ return 789; }, matcher));
}
