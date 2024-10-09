// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_to_json_associative_map_hpp
#define _INCLUDED_to_json_associative_map_hpp

namespace rapidjson
{
	template <typename WriterType, typename AssociativeMapType>
	void writeMap(WriterType & w, const AssociativeMapType & container)
	{
		w.StartObject();
		for (const auto & i : container) {
			write(w, i.first);
			write(w, i.second);
		}
		w.EndObject();
	}
} // namespace rapidjson

#endif //_INCLUDED_to_json_associative_map_hpp
