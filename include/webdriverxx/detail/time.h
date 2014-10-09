#ifndef WEBDRIVERXX_DETAIL_TIME_H
#define WEBDRIVERXX_DETAIL_TIME_H

#include "error_handling.h"
#include "../types.h"

#ifdef _WIN32

#include <windows.h>

#else

#include <time.h>
#include <sys/time.h>

#endif

namespace webdriverxx {
namespace detail {

TimePoint Now() {
	#ifdef _WIN32
		FILETIME time;
		::GetSystemTimeAsFileTime(&time);
		return ((static_cast<TimePoint>(time.dwHighDateTime) << 32)
			+ time.dwLowDateTime)/10000;
	#else
		timeval time = {};
		WEBDRIVERXX_CHECK(0 == gettimeofday(&time, nullptr), "gettimeofday failure");
		return static_cast<TimePoint>(time.tv_sec)*1000 + time.tv_usec/1000;
	#endif
}

void Sleep(Duration milliseconds) {
	#ifdef _WIN32
		::Sleep(static_cast<DWORD>(milliseconds));
	#else
		timespec time = { static_cast<time_t>(milliseconds/1000),
			static_cast<long>(milliseconds%1000)*1000000 };
		while (nanosleep(&time, &time)) {}
	#endif
}

} // namespace detail
} // namespace webdriverxx

#endif
