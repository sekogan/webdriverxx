
# Webdriver++

A lightweight C++ client library for [Selenium Webdriver](http://www.seleniumhq.org/).

## A quick example
```cpp
#include <webdriverxx.h>
using namespace webdriverxx;

WebDriver firefox = StartFirefox();
firefox
    .Navigate("http://google.com")
    .FindElement(ByXPath(".//input[@name='q']"))
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

TODO

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

All interfaces use UTF-8 strings through std::string or char*.

### Thread safety

- Webdriver++ objects are not thread safe. It is not safe to use
neither any single object nor different objects obtained from a single WebDriver
concurrently without synchronization. On the other side, Webdriver++ objects
don't use global variables so it is OK to use different instances of WebDriver
in different threads.

- The CURL library should be explicitly initialized if several WebDrivers are used from
multiple threads. Call `curl_global_init(CURL_GLOBAL_ALL);` from `<curl/curl.h>`
once per process before using this library.

--------------------

Copyright &copy; 2014 Sergey Kogan.
Licensed under [The MIT license](https://github.com/sekogan/webdriverxx/blob/master/LICENSE).
