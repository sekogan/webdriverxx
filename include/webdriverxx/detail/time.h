#ifndef WEBDRIVERXX_DETAIL_TIME_H
#define WEBDRIVERXX_DETAIL_TIME_H

#include "error_handling.h"
#include "../types.h"

#ifdef _WIN32

#include <windows.h>

#else

#include <chrono>
#include <thread>

#endif

namespace webdriverxx {
namespace detail {

TimePoint Now() {
	#ifdef _WIN32
		FILETIME time;
		::GetSystemTimeAsFileTime(&time);
		return static_cast<TimePoint>(time.dwHighDateTime) << 32
			+ time.dwLowDateTime;
	#else
		using namespace std::chrono;
		return static_cast<TimePoint>(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				steady_clock::now().time_since_epoch()
				).count()
			);
	#endif
}

void Sleep(Duration milliseconds) {
	#ifdef _WIN32
		::Sleep(static_cast<DWORD>(milliseconds));
	#else
		std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	#endif
}

} // namespace detail
} // namespace webdriverxx

#endif
