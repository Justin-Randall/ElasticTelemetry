// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_from_json_pair_hpp
#define _INCLUDED_from_json_pair_hpp

#include "rapidjsoncpp/from_json.hpp"

namespace rapidjson
{
	template <typename First, typename Second>
	void read(const Value & v, std::pair<First, Second> & values)
	{
		const auto &               obj = v.GetObject();
		Value::ConstMemberIterator v1  = obj.FindMember("key");
		if (v1 == obj.MemberEnd())
			return;

		Value::ConstMemberIterator v2 = obj.FindMember("value");
		if (v2 == obj.MemberEnd())
			return;

		First first;
		read(v1->value, first);

		Second second;
		read(v2->value, second);

		values.first  = first;
		values.second = second;
	}
} // namespace rapidjson

#endif //_INCLUDED_from_json_pair_hpp
