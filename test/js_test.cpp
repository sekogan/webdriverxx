#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>
#include <list>

namespace test {

using namespace webdriverxx;

class TestJsExecutor : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		GetDriver().Navigate(GetTestPageUrl("js.html"));
	}

	TestJsExecutor() : driver(GetDriver()) {}

	WebDriver driver;
};

TEST_F(TestJsExecutor, ExecutesSimpleScript) {
	driver.Execute("document.title = 'abc'");
	ASSERT_EQ("abc", driver.GetTitle());
}

TEST_F(TestJsExecutor, CanPassStringArgument) {
	driver.Execute("document.title = arguments[0]", JsArgs() << std::string("abc"));
	ASSERT_EQ("abc", driver.GetTitle());
}

TEST_F(TestJsExecutor, CanPassStringLiteralArgument) {
	driver.Execute("document.title = arguments[0]", JsArgs() << "abc");
	ASSERT_EQ("abc", driver.GetTitle());
}

TEST_F(TestJsExecutor, CanPassNumberArgument) {
	driver.Execute("document.title = String(arguments[0] + 21)", JsArgs() << 21);
	ASSERT_EQ("42", driver.GetTitle());
	driver.Execute("document.title = String(arguments[0] + 21)", JsArgs() << 21.5);
	ASSERT_EQ("42.5", driver.GetTitle());
}

TEST_F(TestJsExecutor, CanPassBooleanArgument) {
	driver.Execute("a = arguments; document.title = [ typeof(a[0]), a[0], typeof(a[1]), a[1] ].join(',')",
		JsArgs() << true << false);
	ASSERT_EQ("boolean,true,boolean,false", driver.GetTitle());
}

TEST_F(TestJsExecutor, CanPassMoreThanOneArgument) {
	driver.Execute("document.title = arguments[0] + ',' + arguments[1]",
		JsArgs() << "abc" << "def");
	ASSERT_EQ("abc,def", driver.GetTitle());
}

TEST_F(TestJsExecutor, CanPassElement) {
	Element e = driver.FindElement(ByTag("input")).Clear();
	ASSERT_EQ("", e.GetAttribute("value"));
	driver.Execute("arguments[0].value = arguments[1]",
		JsArgs() << e << "abc");
	ASSERT_EQ("abc", e.GetAttribute("value"));
}

TEST_F(TestJsExecutor, CanPassArray) {
	std::vector<int> numbers;
	numbers.push_back(123);
	numbers.push_back(321);
	driver.Execute("document.title = arguments[0][0] + arguments[0][1]",
		JsArgs() << numbers);
	ASSERT_EQ("444", driver.GetTitle());
}

TEST_F(TestJsExecutor, CanPassOtherContainers) {
	std::list<int> numbers;
	numbers.push_back(123);
	numbers.push_back(321);
	driver.Execute("document.title = arguments[0][0] + arguments[0][1]",
		JsArgs() << numbers);
	ASSERT_EQ("444", driver.GetTitle());
}

TEST_F(TestJsExecutor, CanPassCArray) {
	const char* colors[] = { "red", "green", "blue" };
	driver.Execute("document.title = arguments[0].reverse().join(', ')",
		JsArgs() << colors);
	ASSERT_EQ("blue, green, red", driver.GetTitle());
}

namespace custom {

struct Object {
	std::string string;
	int number;
};

picojson::value CustomToJson(const Object& value) {
	return JsonObject()
		.Set("string", value.string)
		.Set("number", value.number);
}

void CustomFromJson(const picojson::value& value, Object& result) {
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "custom::Object is not an object");
	result.string = FromJson<std::string>(value.get("string"));
	result.number = FromJson<int>(value.get("number"));
}

} // custom

TEST_F(TestJsExecutor, CanPassCustomObject) {
	custom::Object o = { "abc", 123 };
	driver.Execute("o = arguments[0]; document.title = (o.string + 'def') + (o.number + 1)",
		JsArgs() << o);
	ASSERT_EQ("abcdef124", driver.GetTitle());
}

///////////////////////////////////////////////////////////////////////////

TEST_F(TestJsExecutor, EvalsString) {
	ASSERT_EQ("abc", driver.Eval<std::string>("return 'abc'"));
}

TEST_F(TestJsExecutor, EvalsNumber) {
	ASSERT_EQ(123, driver.Eval<int>("return 123"));
	ASSERT_EQ(123.5, driver.Eval<double>("return 123.5"));
}

TEST_F(TestJsExecutor, EvalsBoolean) {
	ASSERT_TRUE(true == driver.Eval<bool>("return true"));
	ASSERT_TRUE(false == driver.Eval<bool>("return false"));
}

TEST_F(TestJsExecutor, EvalsElement) {
	Element e = driver.FindElement(ByTag("input"));
	ASSERT_EQ(e, driver.Eval<Element>("return document.getElementsByTagName('input')[0]"));
}

TEST_F(TestJsExecutor, EvalsCustomObject) {
	custom::Object o = driver.Eval<custom::Object>("return { string: 'abc', number: 123 }");
	ASSERT_EQ("abc", o.string);
	ASSERT_EQ(123, o.number);
}

TEST_F(TestJsExecutor, EvalsArrayOfStrings) {
	std::vector<std::string> v = driver.Eval<std::vector<std::string>>(
		"return [ 'abc', 'def' ]"
		);
	ASSERT_EQ(2u, v.size());
	ASSERT_EQ("abc", v[0]);
	ASSERT_EQ("def", v[1]);
}

TEST_F(TestJsExecutor, EvalsArrayOfNumbers) {
	std::vector<int> v = driver.Eval<std::vector<int>>(
		"return [ 123, 456 ]"
		);
	ASSERT_EQ(2u, v.size());
	ASSERT_EQ(123, v[0]);
	ASSERT_EQ(456, v[1]);
}

///////////////////////////////////////////////////////////////////////////

// Makes asynchronous script from synchronous one
std::string AsyncScript(const std::string& script) {
	return std::string()
		+ "var args = Array.prototype.slice.call(arguments, 0);"
		+ "var callback = args.pop();"
		+ "setTimeout(function(){"
			+ "var result = (function(){" + script + "}).apply(this, args);"
			+ "callback(result)"
		+ "}, 0);"
		;
}

TEST_F(TestJsExecutor, ExecutesSimpleAsyncScript) {
	if (driver.GetCapabilities().GetBrowserName() == browser::Phantom) return; // Crashes PhantomJS 1.9.7
	driver.ExecuteAsync(AsyncScript("document.title = 'abc'"));
	ASSERT_EQ("abc", driver.GetTitle());
}

TEST_F(TestJsExecutor, PassesArgumentsToAsyncScript) {
	if (driver.GetCapabilities().GetBrowserName() == browser::Phantom) return; // Crashes PhantomJS 1.9.7
	driver.ExecuteAsync(AsyncScript("document.title = JSON.stringify(Array.prototype.slice.call(arguments, 0))"),
		JsArgs() << std::string("abc") << 123 << true);
	ASSERT_EQ("[\"abc\",123,true]", driver.GetTitle());
}

TEST_F(TestJsExecutor, ReturnsValueFromAsyncScript) {
	if (driver.GetCapabilities().GetBrowserName() == browser::Phantom) return; // Crashes PhantomJS 1.9.7
	ASSERT_EQ(123, driver.EvalAsync<int>(AsyncScript("return 123")));
}

TEST_F(TestJsExecutor, ReturnsElementFromAsyncScript) {
	if (driver.GetCapabilities().GetBrowserName() == browser::Phantom) return; // Crashes PhantomJS 1.9.7
	Element e = driver.FindElement(ByTag("input"));
	ASSERT_EQ(e, driver.EvalAsync<Element>(AsyncScript(
		"return document.getElementsByTagName('input')[0]")));
}

} // namespace test
