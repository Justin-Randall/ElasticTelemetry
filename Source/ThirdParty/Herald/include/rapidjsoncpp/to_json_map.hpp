// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_to_json_map_hpp
#define _INCLUDED_to_json_map_hpp

#include "rapidjsoncpp/to_json.hpp"
#include "rapidjsoncpp/to_json_associative_map.hpp"
#include "rapidjsoncpp/to_json_pair.hpp"
#include "rapidjsoncpp/to_json_sequence.hpp"
#include <map>

namespace rapidjson
{
	// if it is an stl map that uses stl strings as keys, store it as
	// a normal json object
	template <typename WriterType, typename ValueType>
	void write(WriterType & w, const std::map<std::string, ValueType> & values)
	{
		writeMap(w, values);
	}

	// otherwise, store an array of tuples
	template <typename WriterType, typename KeyType, typename ValueType>
	void write(WriterType & w, const std::map<KeyType, ValueType> & values)
	{
		writeSequence(w, values);
	}
} // namespace rapidjson
#endif //
