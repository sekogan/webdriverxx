
# Webdriver++

A lightweight C++ client library for [Selenium Webdriver](http://www.seleniumhq.org/).

## A quick example
```cpp
#include <webdriverxx.h>
using namespace webdriverxx;

WebDriver firefox = StartFirefox();
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
- Linux, Mac and Windows.
- clang (3.4), GCC (4.6) and Visual Studio (2010).

## More examples

### Wait implicitly for asynchronous operations

Explanation of implicit/explicit waits can be found [here](http://selenium-python.readthedocs.org/en/latest/waits.html).

```cpp
driver.SetImplicitTimeoutMs(5000);
Element element = driver.FindElement(ByName("akela")); // Should poll DOM for 5 seconds before throwing exception
```

### Wait explicitly for asynchronous operations

Explanation of implicit/explicit waits can be found [here](http://selenium-python.readthedocs.org/en/latest/waits.html).

```cpp
#include <webdriverxx/wait.h> // or <webdriverxx.h>

auto find_element = [&]{ return driver.FindElement(ById("asynchronously_loaded_element")); };
Element element = WaitForValue(find_element);
```

```cpp
#include <webdriverxx/wait.h> // or <webdriverxx.h>

auto element_is_selected = [&]{ return driver.FindElement(ById("asynchronously_loaded_element")).IsSelected(); };
WaitUntil(element_is_selected);
```

### Use matchers from [Google Mock](https://code.google.com/p/googlemock/) for waiting

```cpp
#define WEBDRIVERXX_ENABLE_GMOCK_MATCHERS
#include <webdriverxx/wait_match.h> // or <webdriverxx.h>

driver.Navigate("http://initial_url.host.net")
auto get_url = [&]{ return driver.GetUrl(); }
using namespace ::testing;
WaitForMatch(get_url, HasSubstr("some_substring_of_an_url_after_redirects"));
```

## How to build and run tests

### All platforms

Prerequisites:
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

picojson::value ToJson(const Object& value) { // should be in the same namespace
	return JsonObject()
		.With("string", value.string)
		.With("number", value.number)
		.Build();
}

void FromJson2(const picojson::value& value, Object& result) { // should be in the same namespace
	WEBDRIVERXX_CHECK(value.is<picojson::object>(), "custom::Object is not an object");
	result.string = FromJson<std::string>(value.get("string"));
	result.number = FromJson<int>(value.get("number"));
}

} // custom

custom::Object o1 = { "abc", 123 };
driver.Execute("var o1 = arguments[0];", JsArgs() << o1);
custom::Object o1_copy = driver.Eval<custom::Object>("return o1");
custom::Object o2 = driver.Eval<custom::Object>("return { string: 'abc', number: 123 }");
```

--------------------

Copyright &copy; 2014 Sergey Kogan.
Licensed under [The MIT license](https://github.com/sekogan/webdriverxx/blob/master/LICENSE).
