// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_LogLevels_HPP
#define _INCLUDED_LogLevels_HPP

#include <atomic>
#include <map>
#include <string>

namespace Herald
{
	enum struct LogLevels : uint32_t {
		Analysis = (1 << 0),
		Trace    = (1 << 1),
		Debug    = (1 << 2),
		Info     = (1 << 3),
		Warning  = (1 << 4),
		Error    = (1 << 5),
		Fatal    = (1 << 6)
	};

	const std::map<LogLevels, std::string> logTypeNames = {
	    {(LogLevels::Analysis), "Analysis"}, {(LogLevels::Debug), "Debug"}, {(LogLevels::Info), "Info"},  {(LogLevels::Warning), "Warning"},
	    {(LogLevels::Error), "Error"},       {(LogLevels::Fatal), "Fatal"}, {(LogLevels::Trace), "Trace"}};

	inline std::atomic<uint32_t> enabledLogLevels(0xFFFFFFFF);

	inline void disableAllLogLevels() { enabledLogLevels.store(0, std::memory_order_relaxed); }
	inline void disableLogLevel(LogLevels level) { enabledLogLevels.fetch_and(~static_cast<uint32_t>(level), std::memory_order_relaxed); }
	inline void enableAllLogLevels() { enabledLogLevels.store(0xFFFFFFFF, std::memory_order_relaxed); }
	inline void enableLogLevel(LogLevels level) { enabledLogLevels.fetch_or(static_cast<uint32_t>(level), std::memory_order_relaxed); }

	inline uint32_t getEnabledLogLevels() { return enabledLogLevels.load(std::memory_order_relaxed); }
	inline void     setLogLevels(uint32_t levels) { enabledLogLevels.store(levels, std::memory_order_relaxed); }

	inline bool isLogLevelEnabled(LogLevels level)
	{
		return (enabledLogLevels.load(std::memory_order_relaxed) & static_cast<uint32_t>(level)) != 0;
	}

} // namespace Herald

#endif //_INCLUDED_LogLevels_HPP
