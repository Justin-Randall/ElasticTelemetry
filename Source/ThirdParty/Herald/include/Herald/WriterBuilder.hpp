// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_WriterBuilder_hpp
#define _INCLUDED_WriterBuilder_hpp

#include "Herald/ILogWriterBuilder.hpp"
#include <vector>

namespace Herald
{
	template <typename WriterType> class LogWriterBuilder : public ILogWriterBuilder
	{
	  public:
		LogWriterBuilder() = default;
		virtual ~LogWriterBuilder() override {}

		virtual ILogWriterBuilder & addConfigPair(const std::string & key, const std::string & value) override
		{
			ConfigPairs.push_back({key, value});
			return *this;
		}

		virtual ILogWriterPtr build() override
		{
			auto result = std::make_unique<WriterType>();
			for (auto [k, v] : ConfigPairs) {
				result->addConfigPair(k, v);
			}
			return result;
		}

		std::vector<std::pair<std::string, std::string>> ConfigPairs;
	};

	template <typename WriterType> Herald::ILogWriterBuilderPtr createWriterBuilder()
	{
		return std::make_unique<LogWriterBuilder<WriterType>>();
	}
} // namespace Herald
#endif //_INCLUDED_WriterBuilder_hpp
