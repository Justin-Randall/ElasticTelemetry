// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_from_json_sequence_hpp
#define _INCLUDED_from_json_sequence_hpp

#include "rapidjsoncpp/from_json.hpp"

namespace rapidjson
{
	template <typename ContainerType, typename ValueType>
	auto addToContainer(ContainerType & container, const ValueType & value, int)
	    -> decltype(container.push_back(value), void())
	{
		container.push_back(value);
	}

	template <typename ContainerType, typename ValueType>
	void addToContainer(ContainerType & container, const ValueType & value, ...)
	{
		container.insert(value);
	}

	template <typename SequenceContainerType, typename ValueType>
	void readSequence(const Value & ar, SequenceContainerType & values)
	{
		if (!ar.IsArray())
			return;

		for (SizeType i = 0; i < ar.Size(); ++i) {
			ValueType target;
			read(ar[i], target);
			addToContainer(values, target, 0);
		}
	}

	template <typename SequenceContainerType, typename ValueType>
	void readSequence(const Document & doc, SequenceContainerType & values)
	{
		if (!doc.IsArray())
			return;
		const Value & ar = doc.GetArray();

		values.clear();
		readSequence<SequenceContainerType, ValueType>(ar, values);
	}

	template <typename SequenceContainerType, typename ValueType>
	void readSequence(const Document & doc, const std::string & key,
	                  SequenceContainerType & values)
	{
		if (!doc.IsObject())
			return;

		const auto & i = doc.FindMember(key.c_str());
		if (i == doc.MemberEnd())
			return;

		values.clear();

		const Value & ar = i->value;
		readSequence<SequenceContainerType, ValueType>(ar, values);
	}
} // namespace rapidjson
#endif //_INCLUDED_from_json_sequence_hpp