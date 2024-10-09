// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_from_json_map_hpp
#define _INCLUDED_from_json_map_hpp

#include "rapidjsoncpp/from_json.hpp"
#include "rapidjsoncpp/from_json_pair.hpp"
#include "rapidjsoncpp/from_json_sequence.hpp"
#include <map>

namespace rapidjson
{
	// maps of strings to other types can be serialized as normal json
	template <typename ValueType>
	void read(const Value & v, std::map<std::string, ValueType> & values)
	{
		values.clear();
		const auto & obj = v.GetObject();
		for (auto i = obj.MemberBegin(); i != obj.MemberEnd(); i++) {
			std::string k = i->name.GetString();
			ValueType   val;
			read(i->value, val);
			values[k] = val;
		}
	}

	// maps where key types are not strings need to be stored as
	// arrays of pairs to be valid json.
	template <typename KeyType, typename ValueType>
	void read(const Value & v, std::map<KeyType, ValueType> & values)
	{
		readSequence<std::map<KeyType, ValueType>,
		             std::pair<KeyType, ValueType>>(v, values);
	}
} // namespace rapidjson
#endif //_INCLUDED_from_json_map_hpp
