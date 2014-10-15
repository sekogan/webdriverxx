
# Webdriver++

A lightweight C++ client library for [Selenium Webdriver](http://www.seleniumhq.org/).

## A quick example
```cpp
#include <webdriverxx.h>
using namespace webdriverxx;

WebDriver firefox = Firefox().Start();
firefox
    .Navigate("http://google.com")
    .FindElement(ByCss("input[name=q]"))
    .SendKeys("Hello, world!")
    .Submit();
```

## Features

- Chainable commands.
- Value-like objects compatible with STL containers.
- Header-only.
- Lightweight dependencies:
    - [libcurl](http://curl.haxx.se/libcurl/),
    - [picojson](https://github.com/kazuho/picojson).
- Can be used with any testing framework.
- Linux, Mac and Windows.
- clang (3.4), GCC (4.6) and Visual Studio (2010).

## More examples

`#include <webdriverxx/webdriver.h>` and `using namespace webdriverxx`
are assumed in all examples.

### Use proxy

```cpp
WebDriver ie = InternetExplorer().SetProxy(
	SocksProxy("127.0.0.1:3128")
		.SetUsername("user")
		.SetPassword("12345")
		.SetNoProxyFor("custom.host")
	).Start();
```

```cpp
WebDriver ff = Firefox().SetProxy(DirectConnection()).Start();
```

### Navigate browser

```cpp
driver
	.Navigate("http://facebook.com")
	.Navigate("http://twitter.com")
	.Back()
	.Forward()
	.Refresh();
```

### Find elements

```cpp
// Throws exception if no match is found in the document
Element menu = driver.FindElement(ById("menu"));

// Returns empty vector if no such elements
// The search is performed inside the the menu element
std::vector<Element> items = menu.FindElements(ByClass("item"));
```

### Send keyboard input

```cpp
// Sends text input or a shortcut to the element
driver.FindElement(ByTag("input")).SendKeys("Hello, world!");

// Sends text input or a shortcut to the active window
driver.SendKeys(Shortcut() << keys::Control << "t");
```

### Execute Javascript

```cpp
// Simple script, no parameters
driver.Execute("console.log('Hi there!')");

// A script with one parameter
driver.Execute("document.title = arguments[0]", JsArgs() << "Cowabunga!");

// A script with more than one parameter
driver.Execute("document.title = arguments[0] + '-' + arguments[1]",
		JsArgs() << "Beep" << "beep");

// Arrays or containers can also be used as parameters
const char* ss[] = { "Yabba", "dabba", "doo" };
driver.Execute("document.title = arguments[0].join(', ')", JsArgs() << ss);

// Even an Element can be passed to a script
auto element = driver.FindElement(ByTag("input"));
driver.Execute("arguments[0].value = 'That was nuts!'", JsArgs() << element);
```

### Get something from Javascript

```cpp
// Scalar types
auto title = driver.Eval<std::string>("return document.title")
auto number = driver.Eval<int>("return 123");
auto another_number = driver.Eval<double>("return 123.5");
auto flag = driver.Eval<bool>("return true");

// Containers (all std::back_inserter compatible)
std::vector<std::string> v = driver.Eval<std::vector<std::string>>(
		"return [ 'abc', 'def' ]"
		);

// Elements!
Element document_element = driver.Eval<Element>("return document.documentElement");
```

### [Wait implicitly](http://selenium-python.readthedocs.org/en/latest/waits.html) for asynchronous operations

```cpp
driver.SetImplicitTimeoutMs(5000);

// Should poll the DOM for 5 seconds before throwing an exception.
auto element = driver.FindElement(ByName("async_element"));
```

### [Wait explicitly](http://selenium-python.readthedocs.org/en/latest/waits.html) for asynchronous operations

```cpp
#include <webdriverxx/wait.h>

auto find_element = [&]{ return driver.FindElement(ById("async_element")); };
Element element = WaitForValue(find_element);
```

```cpp
#include <webdriverxx/wait.h>

auto element_is_selected = [&]{
	return driver.FindElement(ById("asynchronously_loaded_element")).IsSelected();
	};
WaitUntil(element_is_selected);
```

### Use matchers from [Google Mock](https://code.google.com/p/googlemock/) for waiting

```cpp
#define WEBDRIVERXX_ENABLE_GMOCK_MATCHERS
#include <webdriverxx/wait_match.h>

driver.Navigate("http://initial_url.host.net");
auto url = [&]{ return driver.GetUrl(); };
using namespace ::testing;
auto final_url = WaitForMatch(url, HasSubstr("some_magic"));
```

## How to build and run tests

### All platforms

Prerequisites for building and running tests:
- [CMake](http://www.cmake.org/)
- [PhantomJS](http://phantomjs.org/)
- [node.js](http://nodejs.org/)
- [http-server](https://github.com/nodeapps/http-server)

### Linux & Mac

Prerequisites:
- GCC or clang

```bash
git clone http://github.com/sekogan/webdriverxx
cd webdriverxx
mkdir build
cd build
cmake ..
cmake --build .
phantomjs --webdriver=7777 &
http-server ./test/pages --silent &
ctest -V
```

### Windows

Prerequisites:
- Visual Studio 2010 or newer

```bash
git clone http://github.com/sekogan/webdriverxx
cd webdriverxx
mkdir build
cd build
cmake ..
cmake --build .
start phantomjs --webdriver=7777
start http-server ./test/pages
ctest -V
```

### Testing with real browsers

Prerequisites:
- [Selenium Server](http://www.seleniumhq.org/download/)

```bash
selenium-server -p 4444 &
./webdriverxx --browser=<firefox|chrome|...>
```

## Advanced topics

### Unicode

The library is designed to be encoding-agnostic. It doesn't make
any assumptions about encodings. All strings are transferred
as is, without modifications.

The WebDriver protocol is based on UTF-8, so all strings passed
to the library/received from the library should be/are encoded
using UTF-8.

### Thread safety

- Webdriver++ objects are not thread safe. It is not safe to use
neither any single object nor different objects obtained from a single WebDriver
concurrently without synchronization. On the other side, Webdriver++ objects
don't use global variables so it is OK to use different instances of WebDriver
in different threads.

- The CURL library should be explicitly initialized if several WebDrivers are used from
multiple threads. Call `curl_global_init(CURL_GLOBAL_ALL);` from `<curl/curl.h>`
once per process before using this library.

### Transfer objects between C++ and Javascript

```cpp
namespace custom {

struct Object {
	std::string string;
	int number;
};

// Conversion functions should be in the same namespace as the object
picojson::value CustomToJson(const Object& value) {
	return JsonObject()
		.Set("string", value.string)
		.Set("number", value.number);
}

void CustomFromJson(const picojson::value& value, Object& result) {
	assert(value.is<picojson::object>());
	result.string = FromJson<std::string>(value.get("string"));
	result.number = FromJson<int>(value.get("number"));
}

} // namespace custom

custom::Object o1 = { "abc", 123 };
driver.Execute("var o1 = arguments[0];", JsArgs() << o1);
custom::Object o1_copy = driver.Eval<custom::Object>("return o1");
custom::Object o2 = driver.Eval<custom::Object>("return { string: 'abc', number: 123 }");
```

--------------------

Copyright &copy; 2014 Sergey Kogan.
Licensed under [The MIT license](https://github.com/sekogan/webdriverxx/blob/master/LICENSE).
