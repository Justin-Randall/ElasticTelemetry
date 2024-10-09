// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_from_json_HPP
#define _INCLUDED_from_json_HPP

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <string>
#include <vector>

/// Retrieve plain old data types from a json string.
/// Users implementing custom serialization should override
//  void read(const rapidjson::Value & source, <YourCustomType> & target)
/// in their own headers in scope when reading from a json encoded
/// string.
namespace rapidjson
{
	inline void read(const Value & source, unsigned char & target)
	{
		target = source.GetUint();
	}

	inline void read(const Value & source, char & target)
	{
		target = source.GetInt();
	}

	inline void read(const Value & source, uint16_t & target)
	{
		target = source.GetUint();
	}

	inline void read(const Value & source, int16_t & target)
	{
		target = source.GetInt();
	}

	inline void read(const Value & source, uint32_t & target)
	{
		target = source.GetUint();
	}

	inline void read(const Value & source, int32_t & target)
	{
		target = source.GetInt();
	}

	inline void read(const Value & source, uint64_t & target)
	{
		target = source.GetUint64();
	}

	inline void read(const Value & source, int64_t & target)
	{
		target = source.GetInt64();
	}

	inline void read(const Value & source, float & target)
	{
		target = static_cast<float>(source.GetDouble());
	}

	inline void read(const Value & source, double & target)
	{
		target = source.GetDouble();
	}

	inline void read(const Value & source, bool & target)
	{
		target = source.GetBool();
	}

	inline void read(const Value & source, std::string & target)
	{
		target = source.GetString();
	}

	template <typename ValueType>
	void read(const Value & source, const std::string & key, ValueType & target)
	{
		const auto & v = source.FindMember(key.c_str());

		if (v != source.MemberEnd()) {
			// If you receive a compilation error here about not finding an
			// overload, you need to either include a header that has a read()
			// function that knows how to deserialize the ValueType passed, or
			// implement one and ensure it is in scope where the object is being
			// deserialized.
			read(v->value, target);
		}
	}

	template <typename ValueType>
	void read(const Document & doc, const std::string & key, ValueType & value)
	{
		if (!doc.IsObject())
			return;

		const auto & i = doc.FindMember(key.c_str());
		if (i == doc.MemberEnd())
			return;

		// If you receive a compilation error here about not finding an
		// overload, you need to either include a header that has a read()
		// function that knows how to deserialize the ValueType passed, or
		// implement one and ensure it is in scope where the object is being
		// deserialized.
		read(i->value, value);
	}

	template <typename ValueType, typename... Args>
	void read(const Document & doc, const std::string & key, ValueType & value,
	          Args &... args)
	{
		const Value & v = doc[key.c_str()];
		read(v, value);
		read(doc, args...);
	}

	template <typename ValueType>
	void from_json(const std::string & jsonSource, ValueType & target)
	{
		Document doc;
		doc.Parse(jsonSource.c_str());
		read(doc, target);
	}

	template <typename... Args>
	void from_json(const std::string & jsonSource, Args &... args)
	{
		Document doc;
		doc.Parse(jsonSource.c_str());
		read(doc, args...);
	}
} // namespace rapidjson

#endif //_INCLUDED_from_json_HPP
