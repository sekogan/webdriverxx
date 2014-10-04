[![Build status](https://secure.travis-ci.org/sekogan/sandbox.png)](http://travis-ci.org/sekogan/sandbox)

# Thread Safety #

- Webdriver++ objects are not thread safe. It is not safe to use
neither any single object nor different objects obtained from a single WebDriver
concurrently without synchronization. On the other side, Webdriver++ objects
don't use global variables so it is OK to use different instances of WebDriver
in different threads.

- The CURL library should be explicitly initialized if WebDrivers are used in
multiple threads. Call curl_global_init(CURL_GLOBAL_ALL); from \<curl/curl.h\>
once per process.

# How to build and run tests #

## All platforms ##

Prerequisites:
- [PhantomJS](http://phantomjs.org/)
- [node.js](http://nodejs.org/)
- [http-server](https://github.com/nodeapps/http-server)
- [CMake](http://www.cmake.org/)
- [git](http://git-scm.com/)

## Linux & Mac ##

Prerequisites:
- GCC or clang

    git clone http://github.com/sekogan/webdriverxx
    cd webdriverxx
    mkdir build
    cd build
    cmake ..
    cmake --build .
    phantomjs --webdriver=7777 &
    http-server ./test/pages --silent &
    ctest -V

## Windows ##

Prerequisites:
- Visual Studio 2010 or newer

    git clone http://github.com/sekogan/webdriverxx
    cd webdriverxx
    mkdir build
    cd build
    cmake ..
    cmake --build .
    start phantomjs --webdriver=7777
    start http-server ./test/pages
    ctest -V

## Testing with real browser ##

    selenium-server -p 4444 &
    ./webdriverxx --browser=<firefox|chrome|...>
