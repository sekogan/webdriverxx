#define USE_GMOCK_MATCHERS

#include <webdriverxx/wait.h>
#include <gtest/gtest.h>

using namespace webdriverxx;
using namespace webdriverxx::detail;

int FunctionGetter() { return 123; }

struct FunctorGetter {
	int operator () () const {
		return 456;
	}
};

TEST(WaitFor, CanBeUsedWithFunctionFunctorAndLambda) {
	ASSERT_EQ(123, WaitFor(FunctionGetter));
	ASSERT_EQ(456, WaitFor(FunctorGetter()));
	ASSERT_EQ(789, WaitFor([]{ return 789; }));
}

TEST(WaitFor, DoesNotSleepIfValueIsReturned) {
	Duration timeout = 1000;
	const TimePoint start = Now();
	WaitFor(FunctionGetter, timeout);
	ASSERT_TRUE(Now() - start < timeout/2);
}

TEST(WaitFor, CallsGetterOnce) {
	int counter = 0;
	WaitFor([&counter]{ return ++counter; });
	ASSERT_EQ(1, counter);
}

TEST(WaitFor, ThrowsExceptionOnTimeout) {
	Duration timeout = 0;
	ASSERT_THROW(WaitFor([]{ throw std::exception(); }, timeout), WebDriverException);
}

TEST(WaitFor, CallsGetterUntilItSucceeds) {
	Duration timeout = 1000;
	Duration interval = 0;
	int counter = 0;
	WaitFor([&counter]() -> int {
		if (++counter < 10) throw std::exception();
		return counter;
	}, timeout, interval);
	ASSERT_EQ(10, counter);
}

TEST(WaitFor, PassesErrorMessageFromGetter) {
	Duration timeout = 0;
	try {
		WaitFor([]{ throw std::runtime_error("abc"); }, timeout);
		FAIL();
	} catch (const WebDriverException& e) {
		ASSERT_TRUE(std::string(e.what()).find("abc") != std::string::npos);
		ASSERT_TRUE(std::string(e.what()).find("Timeout") != std::string::npos);
	}
}

bool FunctionMatcher(int) { return true; }

struct FunctorMatcher {
	bool operator () (int) const {
		return true;
	}
};

TEST(WaitUntil, CanBeUsedWithFunctionFunctorAndLambda) {
	ASSERT_EQ(123, WaitUntil([]{ return 123; }, FunctionMatcher));
	ASSERT_EQ(123, WaitUntil([]{ return 123; }, FunctorMatcher()));
	ASSERT_EQ(123, WaitUntil([]{ return 123; }, [](int){ return true; }));
}

TEST(WaitUntil, ReturnsMatchedValue) {
	ASSERT_EQ(123, WaitUntil([]{ return 123; }, [](int){ return true; }));
}

TEST(WaitUntil, DoesNotWaitIfValueIsMatched) {
	Duration timeout = 1000;
	const TimePoint start = Now();
	WaitUntil([]{ return 0; }, [](int){ return true; }, timeout);
	ASSERT_TRUE(Now() - start < timeout/2);
}

TEST(WaitUntil, WaitsUntilValueIsMatched) {
	Duration timeout = 1000;
	Duration interval = 0;
	int counter = 0;
	WaitUntil([]{ return 0; }, [&counter](int){ return ++counter == 10; }, timeout, interval);
	ASSERT_EQ(10, counter);
}

TEST(WaitUntil, ThrowsExceptionOnTimeout) {
	Duration timeout = 0;
	ASSERT_THROW(WaitUntil([]{ return 0; }, [](int){ return false; }, timeout), WebDriverException);
}

TEST(WaitUntil, CanUseGMockMatchers) {
	using namespace ::testing;
	ASSERT_EQ(123, WaitUntil([]{ return 123; }, Eq(123)));
	ASSERT_EQ(123, WaitUntil([]{ return 123; }, 123));
	ASSERT_EQ("abc", WaitUntil([]{ return std::string("abc"); }, "abc"));
	ASSERT_EQ("abc", WaitUntil([]{ return std::string("abc"); }, Eq("abc")));
	ASSERT_EQ(123, WaitUntil([]{ return 123; }, _));
	ASSERT_EQ(123, WaitUntil([]{ return 123; }, An<int>()));
	std::vector<int> v(1, 123);
	ASSERT_EQ(v, WaitUntil([&v]{ return v; }, Contains(123)));
	ASSERT_EQ(v, WaitUntil([&v]{ return v; }, Not(Contains(456))));
	Duration timeout = 0;
	ASSERT_THROW(WaitUntil([&v]{ return v; }, Not(Contains(123)), timeout), WebDriverException);
}
