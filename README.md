[![Build status](https://secure.travis-ci.org/sekogan/sandbox.png)](http://travis-ci.org/sekogan/sandbox)

# Thread Safety #

- Webdriver++ objects are not thread safe. It is not safe to use
neither any single object nor different objects obtained from a single WebDriver
concurrently without synchronization. On the other side Webdriver++ objects
don't use global variables, it is OK to use different instances of WebDriver
in different threads.

- The CURL library should be explicitly initialized if WebDriver is used in
multiple threads. Call curl_global_init(CURL_GLOBAL_ALL); from \<curl/curl.h\>
once per process.

# How to build tests #

## Linux & Mac ##

    mkdir build
    cd build
    cmake ..
    cmake --build .
    ctest

## Windows ##

    mkdir build
    cd build
    cmake -G "Visual Studio 10" ..

To run tests with PhantomJS:

    phantomjs --webdriver=7777 --webdriver-loglevel=WARN &
    ./webdriverxx

To run tests with Selenium server:

    selenium-server -p 4444 &
    ./webdriverxx --browser=<firefox|chrome|...>




