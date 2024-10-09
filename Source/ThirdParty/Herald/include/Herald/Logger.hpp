// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_HeraldLogger_hpp
#define _INCLUDED_HeraldLogger_hpp

#include "Herald/ILogTransformer.hpp"
#include "Herald/ILogTransformerBuilder.hpp"

namespace Herald
{
	/// @brief log a message with a log level and variadic metadata for structured logging
	/// @tparam ...Args Must be KeyType, ValueType, KeyType, ValueType, ...
	/// @param logger ILogTransformer instance
	/// @param logLevel LogLevels enum value (Analysis, Trace, Debug, Info, Warning, Error, Fatal)
	/// @param msg message to log
	/// @param ...args comma separated list of key value pairs
	template <typename... Args> void log(ILogTransformer & logger, const LogLevels logLevel, const std::string & msg, Args... args)
	{
		if (!isLogLevelEnabled(logLevel))
			return;

		LogEntry entry(logLevel, msg, args...);
		logger.log(entry);
	}

	/// @brief log a message with a log level
	/// @param logger ILogTransformer instance
	/// @param logLevel LogLevels enum value (Analysis, Trace, Debug, Info, Warning, Error, Fatal)
	/// @param msg message to log
	inline void log(ILogTransformer & logger, const LogLevels logLevel, const std::string & msg)
	{
		if (!isLogLevelEnabled(logLevel))
			return;

		logger.log(LogEntry(logLevel, msg));
	}

} // namespace Herald
#endif //_INCLUDED_HeraldLogger_hpp
