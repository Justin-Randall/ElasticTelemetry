// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_to_json_hpp
#define _INCLUDED_to_json_hpp

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include <sstream>
#include <string>

namespace rapidjson
{
	template <typename WriterType>
	void write(WriterType & w, const unsigned char & value)
	{
		w.Uint(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const char & value)
	{
		w.Int(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const uint16_t & value)
	{
		w.Uint(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const int16_t & value)
	{
		w.Int(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const uint32_t & value)
	{
		w.Uint(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const int32_t & value)
	{
		w.Int(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const int64_t & value)
	{
		w.Int64(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const uint64_t & value)
	{
		w.Uint64(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const float & value)
	{
		int maxDecimalPlaces = w.GetMaxDecimalPlaces();
		w.SetMaxDecimalPlaces(8);
		// going to burn for this hack, but
		// upcasting a float to a double reveals
		// extra precision that is often unexpected.
		std::stringstream ss;
		ss << value;
		double d = std::stod(ss.str());
		w.Double(d);
		w.SetMaxDecimalPlaces(maxDecimalPlaces);
	}

	template <typename WriterType>
	void write(WriterType & w, const double & value)
	{
		w.Double(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const bool & value)
	{
		w.Bool(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const char * const value)
	{
		w.String(value);
	}

	template <typename WriterType>
	void write(WriterType & w, const std::string & value)
	{
		w.String(value.c_str());
	}
	template <typename WriterType, typename ValueType>
	void write(WriterType & w, const std::string & key, const ValueType & value)
	{
		write(w, key);

		// If you reach this line with an error about 'write' not taking 2
		// arguments, there may be a write overload missing for the type you are
		// attempting to serialize. It should be placed in the
		// rapidjson namespace.
		write(w, value);
	}

	template <typename WriterType, typename ValueType, typename... Args>
	void write(WriterType & w, const std::string & key, const ValueType & value,
	           const Args &... args)
	{
		write(w, key, value);
		write(w, args...);
	}

	template <typename ValueType, typename... Args>
	std::string to_json(const std::string & key, const ValueType & value,
	                    const Args &... args)
	{
		StringBuffer               sb;
		PrettyWriter<StringBuffer> writer(sb);
		writer.StartObject();
		write(writer, key, value, args...);
		writer.EndObject();
		return sb.GetString();
	}

	template <typename ValueType> std::string to_json(const ValueType & value)
	{
		StringBuffer               sb;
		PrettyWriter<StringBuffer> writer(sb);
		write(writer, value);
		return sb.GetString();
	}

} // namespace rapidjson
#endif //_INCLUDED_to_json_hpp
