// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_ILogTransformer_HPP
#define _INCLUDED_ILogTransformer_HPP

#include "Herald/ILogWriter.hpp"
#include "Herald/LogEntry.hpp"
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace Herald
{
	class ILogTransformer
	{
	  protected:
		virtual ~ILogTransformer() = default;

	  public:
		// can be used either standalone, or as part of a builder chain

		/// @brief Adds a callback to the chain of callbacks that will be called when a log message is generated
		/// @param callback
		/// @return *this ILogTransformer instance for chaining
		virtual ILogTransformer & attachLogWriterCallback(const std::function<void(const std::string &)> & callback) = 0;

		/// @brief Adds a log writer to the chain of callbacks that will be called when a log messaage is generated
		/// @param writer
		/// @return
		virtual ILogTransformer & attachLogWriter(const ILogWriterPtr writer) = 0;

		/// @brief Add a static, global header to the logger that may be included with each log
		/// message. For example, a logger that logs to a network server might include a
		/// header with a unique identifier for the client.
		/// @param key
		/// @param value
		/// @return *this ILogTransformer instance for chaining
		virtual ILogTransformer & addHeader(const std::string & key, const std::string & value) = 0;

		/// @brief Log a message with a log level and variadic metadata for structured logging
		/// @param entry entry can contain a log level, message, and metadata. Use the LogEntry constructor to create an entr for directly
		/// calling this on the logger, or use one of the the Herald::log() functions to create an entry.
		virtual void log(const LogEntry & entry) = 0;

		/// @brief remove a callback from the chain of callbacks that will be called when a log message is generated. Absolutely call this
		/// if a member function callback points to an object that is about to be destroyed.
		/// @param callback
		virtual void removeLogCallback(const std::function<void(const std::string &)> & callback) = 0;

		/// @brief Remove a static, global log message header from the logger
		/// @param key
		virtual void removeHeader(const std::string & key) = 0;
	};

	using ILogTransformerPtr = std::shared_ptr<ILogTransformer>;
} // namespace Herald

#endif //_INCLUDED_ILogTransformer_HPP
