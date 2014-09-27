#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>

using namespace webdriverxx;

class TestJsExecutor : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		Environment::Instance().GetDriver()->Navigate(
			Environment::Instance().GetTestPageUrl("js.html")
			);
	}

	TestJsExecutor() : driver(Environment::Instance().GetDriver()) {}

	WebDriver* driver;
};

TEST_F(TestJsExecutor, ExecutesSimpleScript) {
	driver->Execute("document.title = 'abc'");
	ASSERT_EQ("abc", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassStringArgument) {
	driver->Execute("document.title = arguments[0]", JsArgs() << std::string("abc"));
	ASSERT_EQ("abc", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassStringLiteralArgument) {
	driver->Execute("document.title = arguments[0]", JsArgs() << "abc");
	ASSERT_EQ("abc", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassIntegerArgument) {
	driver->Execute("document.title = String(arguments[0] + 21)",
		JsArgs() << 21);
	ASSERT_EQ("42", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassBooleanArgument) {
	driver->Execute("document.title = String(typeof(arguments[0]) === 'boolean' ? arguments[0] : false)",
		JsArgs() << true);
	ASSERT_EQ("true", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassMoreThanOneArgument) {
	driver->Execute("document.title = arguments[0] + ',' + arguments[1]",
		JsArgs() << "abc" << "def");
	ASSERT_EQ("abc,def", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassElement) {
	Element e = driver->FindElement(ByTagName("input")).Clear();
	ASSERT_EQ("", e.GetAttribute("value"));
	driver->Execute("arguments[0].value = arguments[1]",
		JsArgs() << e << "abc");
	ASSERT_EQ("abc", e.GetAttribute("value"));
}

TEST_F(TestJsExecutor, CanPassArray) {
	std::vector<int> numbers;
	numbers.push_back(123);
	numbers.push_back(321);
	driver->Execute("document.title = arguments[0][0] + arguments[0][1]",
		JsArgs() << numbers);
	ASSERT_EQ("444", driver->GetTitle());
}

struct CustomObject {
	std::string string;
	int number;
};

namespace webdriverxx {
namespace detail {

template<>
inline
picojson::value ToJson(const CustomObject& value) {
	return JsonObject()
		.With("string", value.string)
		.With("number", value.number)
		.Build();
}

} // namespace detail
} // namespace webdriverxx

TEST_F(TestJsExecutor, CanPassCustomObject) {
	CustomObject o = { "abc", 123 };
	driver->Execute("o = arguments[0]; document.title = (o.string + 'def') + (o.number + 1)",
		JsArgs() << o);
	ASSERT_EQ("abcdef124", driver->GetTitle());
}
