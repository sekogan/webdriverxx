#include "environment.h"
#include <webdriverxx/wait.h>
#include <gtest/gtest.h>

using namespace webdriverxx;
using namespace webdriverxx::detail;

int Function() { return 123; }

struct Functor {
	int operator () () const {
		return 456;
	}
};

TEST(WaitForValue, CanBeUsedWithFunctionFunctorAndLambda) {
	Duration timeout = 0;
	ASSERT_EQ(123, WaitForValue(Function, timeout));
	ASSERT_EQ(456, WaitForValue(Functor(), timeout));
	ASSERT_EQ(789, WaitForValue([]{ return 789; }, timeout));
}

TEST(WaitForValue, DoesNotSleepIfValueIsReturned) {
	Duration timeout = 1000;
	const TimePoint start = Now();
	WaitForValue(Function, timeout);
	ASSERT_TRUE(Now() - start < timeout/2);
}

TEST(WaitForValue, CallsGetterOnce) {
	int counter = 0;
	WaitForValue([&counter]{ return ++counter; });
	ASSERT_EQ(1, counter);
}

TEST(WaitForValue, ThrowsExceptionOnTimeout) {
	Duration timeout = 0;
	ASSERT_THROW(WaitForValue([]{ throw std::exception(); }, timeout), WebDriverException);
}

TEST(WaitForValue, CallsGetterUntilItSucceeds) {
	Duration timeout = 1000;
	Duration interval = 0;
	int counter = 0;
	WaitForValue([&counter]{
		if (++counter < 10) throw std::exception();
		return counter;
	}, timeout, interval);
	ASSERT_EQ(10, counter);
}

TEST(WaitForValue, PassesErrorMessageFromGetter) {
	Duration timeout = 0;
	try {
		WaitForValue([]{ throw std::runtime_error("abc"); }, timeout);
		FAIL();
	} catch (const WebDriverException& e) {
		ASSERT_TRUE(std::string(e.what()).find("abc") != std::string::npos);
		ASSERT_TRUE(std::string(e.what()).find("Timeout") != std::string::npos);
	}
}

TEST(WaitForMatch, ReturnsMatchedValue) {
	auto value = WaitForMatch([]{ return 123; }, [](int){ return true; });
	ASSERT_EQ(123, value);
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
	Duration interval = 0;
	ASSERT_THROW(WaitForMatch([]{ return 0; }, [](int){ return false; }, timeout, interval),
		WebDriverException);
}
