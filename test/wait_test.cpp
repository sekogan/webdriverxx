#include <webdriverxx/wait.h>
#include <gtest/gtest.h>

namespace test {

using namespace webdriverxx;
using namespace webdriverxx::detail;

int FunctionGetter() { return 123; }

struct FunctorGetter {
	int operator () () const {
		return 456;
	}
};

TEST(WaitForValue, CanBeUsedWithFunctionFunctorAndLambda) {
	ASSERT_EQ(123, WaitForValue(FunctionGetter));
	ASSERT_EQ(456, WaitForValue(FunctorGetter()));
	ASSERT_EQ(789, WaitForValue([]{ return 789; }));
}

TEST(WaitForValue, DoesNotWaitIfValueIsReturned) {
	Duration timeout = 1000;
	const TimePoint start = Now();
	WaitForValue(FunctionGetter, timeout);
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
	WaitForValue([&counter]() -> int {
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
		std::string message = e.what();
		const auto npos = std::string::npos;
		ASSERT_NE(npos, message.find("abc"));
		ASSERT_NE(npos, message.find("imeout"));
	}
}

TEST(WaitUntil, DoesNotWaitIfValueNotFalsy) {
	Duration timeout = 1000;
	const TimePoint start = Now();
	WaitUntil([]{ return true; }, timeout);
	ASSERT_TRUE(Now() - start < timeout/2);
}

TEST(WaitUntil, CallsGetterOnce) {
	int counter = 0;
	WaitUntil([&counter]{ return ++counter; });
	ASSERT_EQ(1, counter);
}

TEST(WaitUntil, ThrowsExceptionOnTimeout) {
	Duration timeout = 0;
	ASSERT_THROW(WaitUntil([]() -> bool { throw std::exception(); }, timeout), WebDriverException);
}

TEST(WaitUntil, ThrowsExceptionOnTimeout2) {
	Duration timeout = 0;
	ASSERT_THROW(WaitUntil([]{ return false; }, timeout), WebDriverException);
}

} // namespace test
