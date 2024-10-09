// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_LogTransformerFactory_HPP
#define _INCLUDED_LogTransformerFactory_HPP

#include "Herald/ILogTransformerBuilder.hpp"

namespace Herald
{
	/// @brief creates a standard log transformer that tranform log inputs, combined with headers or additional fields and sends the
	/// transformed output to the attached log writer callbacks as a single-line text message. No default writers are added to the log
	/// transformer. These should be added during the build using ILogTransformerBuilder::attachLogWriterCallback(). No default headers are
	/// added, these should be added using ILogTransformerBuilder::addHeader().
	/// @return
	ILogTransformerBuilderPtr createSimpleLogTransformerBuilder();
} // namespace Herald

#endif //_INCLUDED_LogTransformerFactory_HPP
