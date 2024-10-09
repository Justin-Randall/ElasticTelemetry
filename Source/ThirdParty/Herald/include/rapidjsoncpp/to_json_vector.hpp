// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_to_json_vector_hpp
#define _INCLUDED_to_json_vector_hpp

#include "rapidjsoncpp/to_json.hpp"
#include "rapidjsoncpp/to_json_sequence.hpp"
#include <vector>

namespace rapidjson
{
	template <typename WriterType, typename ValueType>
	void write(WriterType & w, const std::vector<ValueType> & values)
	{
		writeSequence(w, values);
	}
} // namespace rapidjson

#endif //_INCLUDED_to_json_vector_hpp
