// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_LogTransformerBuilder_HPP
#define _INCLUDED_LogTransformerBuilder_HPP

#include "Herald/ILogTransformer.hpp"
#include <memory>
#include <string>

namespace Herald
{
	class ILogTransformerBuilder
	{
	  public:
		virtual ILogTransformerBuilder & attachLogWriterCallback(const std::function<void(const std::string &)> & callback) = 0;
		virtual ILogTransformerBuilder & attachLogWriter(const ILogWriterPtr writer)                                        = 0;
		virtual ILogTransformerBuilder & addHeader(const std::string & key, const std::string & value)                      = 0;
		virtual ILogTransformerPtr       build()                                                                            = 0;
		virtual ~ILogTransformerBuilder()                                                                                   = default;
	};

	using ILogTransformerBuilderPtr = std::unique_ptr<ILogTransformerBuilder>;
} // namespace Herald

#endif //_INCLUDED_LogTransformerBuilder_HPP