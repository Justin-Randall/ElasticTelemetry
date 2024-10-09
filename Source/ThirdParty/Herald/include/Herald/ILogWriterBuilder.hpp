// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_ILogWriterBuilder_HPP
#define _INCLUDED_ILogWriterBuilder_HPP

#include "Herald/ILogWriter.hpp"
#include <memory>
#include <string>

namespace Herald
{
	class ILogWriterBuilder
	{
	  public:
		virtual ILogWriterBuilder & addConfigPair(const std::string & key, const std::string & value) = 0;
		virtual ILogWriterPtr       build()                                                           = 0;
		virtual ~ILogWriterBuilder()                                                                  = default;
	};

	using ILogWriterBuilderPtr = std::unique_ptr<ILogWriterBuilder>;
} // namespace Herald

#endif //_INCLUDED_ILogWriterBuilder_HPP
