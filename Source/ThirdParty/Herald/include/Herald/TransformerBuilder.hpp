// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_TransformerBuilder_hpp
#define _INCLUDED_TransformerBuilder_hpp

#include "Herald/BaseLogTransformerBuilder.hpp"

namespace Herald
{
	/// @brief Helper class to build a simple transformer.
	/// @tparam TransformerType
	/// @details This class is a helper class to build a simple transformer inherited from BaseLogTransformer.
	///     It is used by createTransformerBuilder(), which is also templated.
	/// @example
	///     auto writer = Herald::createWriterBuilder<StringLogWriter>()->build();
	///     auto transformer = Herald::createTransformerBuilder<LogTransformer>()->attachLogWriter(writer)->build();
	template <typename TransformerType> class TransformerBuilder : public BaseLogTransformerBuilder
	{
		virtual ILogTransformerPtr build() override
		{
			auto logger = std::make_shared<TransformerType>();
			applyBuildConfigurations(logger);
			return logger;
		}
	};

	template <typename TransformerType> Herald::ILogTransformerBuilderPtr createTransformerBuilder()
	{
		return std::make_unique<TransformerBuilder<TransformerType>>();
	}
} // namespace Herald

#endif //_INCLUDED_TransformerBuilder_hpp
