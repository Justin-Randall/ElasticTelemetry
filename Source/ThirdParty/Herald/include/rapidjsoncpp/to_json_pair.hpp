// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_to_json_pair_hpp
#define _INCLUDED_to_json_pair_hpp

#include "rapidjsoncpp/to_json.hpp"
#include <utility>

namespace rapidjson
{
	template <typename WriterType, typename KeyType, typename ValueType>
	void write(WriterType & w, const std::pair<KeyType, ValueType> & values)
	{
		w.StartObject();
		write(w, "key");
		write(w, values.first);
		write(w, "value");
		write(w, values.second);
		w.EndObject();
	}
} // namespace rapidjson

#endif //_INCLUDED_to_json_pair_hpp
