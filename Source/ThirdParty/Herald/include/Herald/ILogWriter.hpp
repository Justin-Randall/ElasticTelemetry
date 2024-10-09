// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_ILogWriter_HPP
#define _INCLUDED_ILogWriter_HPP

#include <memory>
#include <string>

namespace Herald
{
	class ILogWriter
	{
	  public:
		ILogWriter()          = default;
		virtual ~ILogWriter() = default;

		/// @brief Loggers can be configured with key value pairs. For example, a file path for a file logger or a server address for a
		/// network logger
		/// @param key
		/// @param value
		/// @return *this ILogTransformer instance for chaining
		virtual ILogWriter & addConfigPair(const std::string & key, const std::string & value) = 0;

		virtual void write(const std::string & msg) = 0;
	};

	using ILogWriterPtr = std::shared_ptr<ILogWriter>;

} // namespace Herald

#endif //_INCLUDED_ILogWriter_HPP
