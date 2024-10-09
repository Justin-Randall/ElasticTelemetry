#ifndef _INCLUDED_BaseLogTransformer_hpp
#define _INCLUDED_BaseLogTransformer_hpp

#include "Herald/ILogTransformer.hpp"
#include "Herald/ILogWriter.hpp"
#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace Herald
{
	class BaseLogTransformer : public ILogTransformer
	{
	  public:
		BaseLogTransformer()                   = default;
		virtual ~BaseLogTransformer() override = default;

		virtual ILogTransformer & addHeader(const std::string & key, const std::string & value) override;
		virtual ILogTransformer & attachLogWriterCallback(const std::function<void(const std::string &)> & callback) override;
		virtual ILogTransformer & attachLogWriter(const ILogWriterPtr writer) override;
		virtual void              removeLogCallback(const std::function<void(const std::string &)> & callback) override;
		virtual void              removeHeader(const std::string & key) override;

		// common functionality for simple and json log transformers with callbacks
	  protected:
		std::vector<std::function<void(const std::string &)>> callbacks;
		std::vector<std::weak_ptr<ILogWriter>>                writers;
		std::map<std::string, std::string>                    headers;
	};
} // namespace Herald

#endif //_INCLUDED_BaseLogTransformer_hpp
