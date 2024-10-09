// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#pragma once

#include "CoreMinimal.h"
#include <string>

/// <summary>
/// to_string overloads for converting various types to std::string. These should be in scope for the Herald::LogEntry struct, meaning
/// include order where this file is included is important. When in doubt, include this file first.
/// </summary>
namespace std
{
	inline std::string to_string(const TCHAR* value)
	{
		return std::string(TCHAR_TO_UTF8(value));
	}

	inline std::string to_string(const FString& value)
	{
		return TCHAR_TO_UTF8(*value);
	}

	inline std::string to_string(const FVector& value)
	{
		std::string result = std::string("(") + std::to_string(value.X) + ", " + std::to_string(value.Y) + ", " + std::to_string(value.Z) + std::string(")");
		return result;
	}
} // namespace std
