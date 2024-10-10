// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#ifndef _INCLUDED_BaseLogTransformerBuilder_hpp
#define _INCLUDED_BaseLogTransformerBuilder_hpp

#include "Herald/ILogTransformer.hpp"
#include "Herald/ILogTransformerBuilder.hpp"

namespace Herald
{
	class BaseLogTransformerBuilder : public ILogTransformerBuilder
	{
	  public:
		BaseLogTransformerBuilder()                   = default;
		virtual ~BaseLogTransformerBuilder() override = default;

		virtual ILogTransformerBuilder & addHeader(const std::string & key, const std::string & value) override;
		virtual ILogTransformerBuilder & attachLogWriter(const ILogWriterPtr writer) override;
		virtual ILogTransformerBuilder & attachLogWriterCallback(const std::function<void(const std::string &)> & callback) override;

		// build() will be actually allocating and assigning builder parameters to the transformer
	  protected:
		virtual void applyBuildConfigurations(ILogTransformerPtr logTransformer);

	  protected:
		std::vector<std::function<void(const std::string &)>> callbacks;
		std::vector<std::weak_ptr<ILogWriter>>                writers;
		std::vector<std::pair<std::string, std::string>>      headers;
	};
} // namespace Herald

#endif //_INCLUDED_BaseLogTransformerBuilder_hpp
