// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_LogEntry_HPP
#define _INCLUDED_LogEntry_HPP

#include "Herald/LogLevels.hpp"
#include <map>
#include <string>

namespace std
{
	inline std::string to_string(const char * value) { return std::string(value); }
	inline std::string to_string(const std::string & value) { return value; }
} // namespace std

namespace Herald
{
	struct LogEntry {
		LogEntry(const LogLevels logLevel, const std::string & message)
		    : logLevel(logLevel)
		    , message(message)
		{
		}
		LogEntry(const std::string & message)
		    : logLevel(LogLevels::Info)
		    , message(message)
		{
		}

		/// @brief Variadict constructor for structured logging
		/// @tparam ...Args Must be pairs of KeyType, ValueType, KeyType, ValueType, ...
		/// @param logLevel log level for the message (Analysis, Trace, Debug, Info, Warning, Error, Fatal)
		/// @param message
		/// @param ...args comma-separated list of key value pairs.
		template <typename... Args>
		LogEntry(const LogLevels logLevel, const std::string & message, Args... args)
		    : logLevel(logLevel)
		    , message(message)
		{
			addMetadata(args...);
		}

		template <typename KeyType, typename ValueType, typename... Args>
		void addMetadata(const KeyType & key, const ValueType & value, Args... args)
		{
			const std::string keyStr = std::to_string(key);
			metadata[keyStr]         = std::to_string(value);
			addMetadata(args...);
		}

		void addMetadata() {}

		LogLevels                          logLevel;
		std::string                        message;
		std::map<std::string, std::string> metadata;
	};
} // namespace Herald

#endif //_INCLUDED_LogEntry_HPP
