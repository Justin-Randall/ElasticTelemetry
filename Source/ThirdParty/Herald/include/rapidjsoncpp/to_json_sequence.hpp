// Copyright (c) 2020 Justin Randall / Playscale PTE LTD
// Released under the MIT license. See LICENSE included with this
// source package for details.

#ifndef _INCLUDED_to_json_sequence_hpp
#define _INCLUDED_to_json_sequence_hpp

namespace rapidjson
{
	template <typename WriterType, typename SequenceContainerType>
	void writeSequence(WriterType & w, const SequenceContainerType & container)
	{
		w.StartArray();
		for (const auto & i : container) {
			write(w, i);
		}
		w.EndArray();
	}
} // namespace rapidjson
#endif //_INCLUDED_to_json_sequence_hpp
