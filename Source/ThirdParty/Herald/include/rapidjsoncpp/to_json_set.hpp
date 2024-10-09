// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_to_json_set_hpp
#define _INCLUDED_to_json_set_hpp

#include "rapidjsoncpp/to_json.hpp"
#include "rapidjsoncpp/to_json_sequence.hpp"
#include <set>

namespace rapidjson
{
	template <typename WriterType, typename ValueType>
	void write(WriterType & w, const std::set<ValueType> & values)
	{
		writeSequence(w, values);
	}
} // namespace rapidjson

#endif //_INCLUDED_to_json_set_hpp
