// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_from_json_set_hpp
#define _INCLUDED_from_json_set_hpp

#include "rapidjsoncpp/from_json.hpp"
#include "rapidjsoncpp/from_json_sequence.hpp"
#include <set>

namespace rapidjson
{
	template <typename ValueType>
	void read(const Document & doc, const std::string & key,
	          std::set<ValueType> & values)
	{
		readSequence<std::set<ValueType>, ValueType>(doc, key, values);
	}

	template <typename ValueType>
	void read(const Value & source, std::set<ValueType> & target)
	{
		readSequence<std::set<ValueType>, ValueType>(source, target);
	}
} // namespace rapidjson

#endif //_INCLUDED_from_json_set_hpp
