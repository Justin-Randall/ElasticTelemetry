// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_GetTimeStamp_hpp
#define _INCLUDED_GetTimeStamp_hpp

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string>

namespace Herald
{
	inline std::string getTimeStamp(const std::chrono::system_clock::time_point & timePoint)
	{
		std::time_t               currentTime     = std::chrono::system_clock::to_time_t(timePoint);
		std::chrono::milliseconds currentChronoMs = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint.time_since_epoch());

		struct tm timeInfo;
#if defined(_WIN32) || defined(_WIN64)
		// Windows-specific code using localtime_s
		localtime_s(&timeInfo, &currentTime);
#else
		// POSIX-specific code using localtime_r
		localtime_r(&currentTime, &timeInfo);
#endif

		std::stringstream timeStamp;
		timeStamp << std::put_time(&timeInfo, "%Y-%m-%dT%H:%M:%S") << "." << std::setfill('0') << std::setw(3)
		          << (currentChronoMs.count() % 1000) << std::put_time(&timeInfo, "%z");
		return timeStamp.str();
	}

	inline std::string getTimeStamp()
	{
		auto currentChronoTime = std::chrono::system_clock::now();
		return getTimeStamp(currentChronoTime);
	}
} // namespace Herald

namespace std
{
	inline std::string to_string(const std::chrono::system_clock::time_point & timePoint) { return Herald::getTimeStamp(timePoint); }
} // namespace std
#endif //_INCLUDED_GetTimeStamp_hpp