#include <webdriverxx/wait_match.h>
#include <gtest/gtest.h>

using namespace webdriverxx;
using namespace webdriverxx::detail;

bool FunctionMatcher(int) { return true; }

struct FunctorMatcher {
	bool operator () (int) const {
		return true;
	}
};

TEST(WaitForMatch, CanBeUsedWithFunctionFunctorAndLambda) {
	ASSERT_EQ(123, WaitForMatch([]{ return 123; }, FunctionMatcher));
	ASSERT_EQ(123, WaitForMatch([]{ return 123; }, FunctorMatcher()));
	ASSERT_EQ(123, WaitForMatch([]{ return 123; }, [](int){ return true; }));
}

TEST(WaitForMatch, ReturnsMatchedValue) {
	ASSERT_EQ(123, WaitForMatch([]{ return 123; }, [](int){ return true; }));
}

TEST(WaitForMatch, DoesNotWaitIfValueIsMatched) {
	Duration timeout = 1000;
	const TimePoint start = Now();
	WaitForMatch([]{ return 0; }, [](int){ return true; }, timeout);
	ASSERT_TRUE(Now() - start < timeout/2);
}

TEST(WaitForMatch, WaitsUntilValueIsMatched) {
	Duration timeout = 1000;
	Duration interval = 0;
	int counter = 0;
	WaitForMatch([]{ return 0; }, [&counter](int){ return ++counter == 10; }, timeout, interval);
	ASSERT_EQ(10, counter);
}

TEST(WaitForMatch, ThrowsExceptionOnTimeout) {
	Duration timeout = 0;
	ASSERT_THROW(WaitForMatch([]{ return 0; }, [](int){ return false; }, timeout), WebDriverException);
}

TEST(WaitForMatch, ExplainsTimeout) {
	try {
		Duration timeout = 0;
		WaitForMatch([]{ return 0; }, [](int){ return false; }, timeout);
		FAIL();
	} catch (const std::exception& e) {
		std::string message = e.what();
		const auto npos = std::string::npos;
		ASSERT_NE(npos, message.find("imeout"));
	}
}

TEST(WaitForMatch, CanUseGMockMatchers) {
	using namespace ::testing;
	ASSERT_EQ(123, WaitForMatch([]{ return 123; }, Eq(123)));
	ASSERT_EQ(123, WaitForMatch([]{ return 123; }, 123));
	ASSERT_EQ("abc", WaitForMatch([]{ return std::string("abc"); }, "abc"));
	ASSERT_EQ("abc", WaitForMatch([]{ return std::string("abc"); }, Eq("abc")));
	ASSERT_EQ(123, WaitForMatch([]{ return 123; }, _));
	ASSERT_EQ(123, WaitForMatch([]{ return 123; }, An<int>()));
	std::vector<int> v(1, 123);
	ASSERT_EQ(v, WaitForMatch([&v]{ return v; }, Contains(123)));
	ASSERT_EQ(v, WaitForMatch([&v]{ return v; }, Not(Contains(456))));
	Duration timeout = 0;
	ASSERT_THROW(WaitForMatch([&v]{ return v; }, Not(Contains(123)), timeout), WebDriverException);
}

TEST(WaitForMatch, ExplainsGMockMatcherMismatch) {
	try {
		Duration timeout = 0;
		using namespace ::testing;
		WaitForMatch([]{ return 123; }, Eq(456), timeout);
		FAIL();
	} catch (const std::exception& e) {
		std::string message = e.what();
		const auto npos = std::string::npos;
		ASSERT_NE(npos, message.find("123"));
		ASSERT_NE(npos, message.find("456"));
		ASSERT_NE(npos, message.find("imeout"));
	}
}

struct type1 {};
struct type2 {};

int foo(type1)
{
	return 1;
}

template<typename T>
int bar() {
	return foo(T());
}

TEST(WaitForMatch, ADL) {
	ASSERT_EQ(1, bar<type1>());
	ASSERT_EQ(2, bar<type2>());
}

int foo(type2)
{
	return 2;
}
