// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#pragma once

#include "CoreMinimal.h"

/// <summary>
/// FileNameFriendly
///
/// The index name is used to programmatically instance new indices
/// in ElasticSearch. Internally, these names are used as filenames,
/// so only characters that are valid for filenames should be used.
///
/// </summary>
/// <param name="IndexName"></param>
/// <returns>
/// A normalized, lower-case FString suitable for sending to
/// ElasticSearch for use as an index.
/// </returns>
inline FString FileNameFriendly(const FString& IndexName)
{
	FString Result = IndexName.ToLower();
	for (TCHAR& i : Result)
	{
		if ((i >= 'a' && i <= 'z') || (i >= '0' && i <= '9') || (i == '-') || (i == '_') || (i == '.'))
			continue;
		i = '_';
	}
	return Result;
}

