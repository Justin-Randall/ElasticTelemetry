#pragma once

#include "CoreMinimal.h"
#include <string>

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
} // namespace std