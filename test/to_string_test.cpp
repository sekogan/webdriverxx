#include <webdriverxx/detail/to_string.h>
#include <gtest/gtest.h>

using namespace webdriverxx;
using namespace webdriverxx::detail;

TEST(ToString, ConvertsIntegralTypes) {
	ASSERT_EQ("123", ToString(123));
	ASSERT_EQ("'z'", ToString('z'));
}

namespace custom {
	struct A {};

	struct B {};
	std::ostream& operator << (std::ostream& s, const B&) {
		return s << "B";
	}

	struct C {};
	std::string ToString(const C&) {
		return "C";
	}
} // namespace custom

TEST(ToString, ConvertsCustomTypes) {
	ASSERT_EQ("<non-printable>", ToString(custom::A()));
	ASSERT_EQ("B", ToString(custom::B()));
	ASSERT_EQ("C", ToString(custom::C()));
}

TEST(ToString, ConvertsStrings) {
	ASSERT_EQ("\"abc\"", ToString("abc"));
	ASSERT_EQ("\"abc\"", ToString(std::string("abc")));
	const char* abc = "abc";
	ASSERT_EQ("\"abc\"", ToString(abc));
	char a[] = "abc";
	ASSERT_EQ("\"abc\"", ToString(a));
}

TEST(ToString, ConvertsPointers) {
	int i = 123;
	ASSERT_EQ("*123", ToString(&i));
	const int* cpi = &i;
	ASSERT_EQ("**123", ToString(&cpi));
	const std::string s = "abc";
	ASSERT_EQ("*\"abc\"", ToString(&s));
}

TEST(ToString, ConvertsContainersOfIntegralTypes) {
	ASSERT_EQ("[]", ToString(std::vector<int>()));
	std::vector<int> v;
	v.push_back(123);
	v.push_back(456);
	ASSERT_EQ("*[123, 456]", ToString(&v));
	int a[3] = { 123, 456, 789 };
	ASSERT_EQ("[123, 456, 789]", ToString(a));
}

TEST(ToString, ConvertsContainersOfCustomTypes) {
	std::vector<custom::A> as(1);
	ASSERT_EQ("[<non-printable>]", ToString(as));
	std::vector<custom::B> bs(1);
	ASSERT_EQ("[B]", ToString(bs));
	std::vector<custom::C> cs(1);
	ASSERT_EQ("[C]", ToString(cs));
}

TEST(ToString, ConvertsContainersOfStrings) {
	const char* ss[] = { "abc", "def" };
	ASSERT_EQ("[\"abc\", \"def\"]", ToString(ss));
	ASSERT_EQ("*[\"abc\", \"def\"]", ToString(&ss));
}
