// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_FieWriterFactory_hpp
#define _INCLUDED_FieWriterFactory_hpp

#include "Herald/ILogWriterBuilder.hpp"

namespace Herald
{
	ILogWriterBuilderPtr createFileWriterBuilder();
}

#endif //_INCLUDED_FieWriterFactory_hpp
