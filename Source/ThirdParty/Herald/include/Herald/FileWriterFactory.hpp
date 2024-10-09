#ifndef _INCLUDED_FieWriterFactory_hpp
#define _INCLUDED_FieWriterFactory_hpp

#include "Herald/ILogWriterBuilder.hpp"

namespace Herald
{
	ILogWriterBuilderPtr createFileWriterBuilder();
}

#endif //_INCLUDED_FieWriterFactory_hpp
