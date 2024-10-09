// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#pragma once

#include "CoreMinimal.h"
#include "ElasticTelemetry.h"
#include "Herald/LogLevels.hpp"
#include "Herald/Logger.hpp"
#include "StringConversions.h"

// ----------------------------------------------------------------------------
// Helpers for custom logging.
// - GetJsonTransformer()
// - addHeader()
// - removeHeader()
// - log() with variadic arguments

namespace Herald
{
	/// <summary>
	/// Primarily an internal function to facilitate helper functions, though it can be used directly to configure the behavior of the JsonTransformer.
	/// </summary>
	/// <returns>An interface the active LogTransformer</returns>
	inline ILogTransformerPtr GetJsonTransformer()
	{
		FElasticTelemetryModule& ElasticTelemetryModule = FModuleManager::GetModuleChecked<FElasticTelemetryModule>("ElasticTelemetry");

		// Get the Transformer from the OutputDevice
		// FElasticTelemetryOutputDevice* OutputDevice = ElasticTelemetryModule.GetOutputDevice();
		// if (!OutputDevice)
		//	return nullptr;

		return ElasticTelemetryModule.GetJsonTransformer();
	}
	/// <summary>
	/// addHeader will insert a key-value pair into the headers of the JsonTransformer.
	/// These are included with each log message transformed. Use cases include:
	/// Current user name or machine name to isolate logs aggregated in ElasticSearch.
	/// </summary>
	/// <typeparam name="KeyType"></typeparam>
	/// <typeparam name="ValueType"></typeparam>
	/// <param name="Key"></param>
	/// <param name="Value"></param>
	template <typename KeyType, typename ValueType>
	void addHeader(const KeyType& Key, const ValueType& Value)
	{
		auto JsonTransformer = GetJsonTransformer();
		if (!JsonTransformer)
			return;

		// convert to strings
		std::string KeyStr = std::to_string(Key);
		std::string ValueStr = std::to_string(Value);

		JsonTransformer->addHeader(KeyStr, ValueStr);
	}

	/// <summary>
	/// Removes any previously added header from the JsonTransformer.
	/// </summary>
	/// <typeparam name="KeyType"></typeparam>
	/// <typeparam name="ValueType"></typeparam>
	/// <param name="Key"></param>
	template <typename KeyType, typename ValueType>
	void removeHeader(const KeyType& Key)
	{
		auto JsonTransformer = GetJsonTransformer();
		if (!JsonTransformer)
			return;

		// convert to strings
		std::string KeyStr = std::to_string(Key);

		JsonTransformer->removeHeader(KeyStr);
	}

	/// <summary>
	/// Custom log function that will log a message with the specified LogLevel.
	/// For most cases, simply using UE_LOG is sufficient. If there are special
	/// log messages that need to be transformed into JSON, this function can be used.
	/// The variadic arguments are passed to the JsonTransformer to be included in the
	/// transformed message and must be in Key, Value pairs for everything after the message.
	/// </summary>
	/// <typeparam name="...Args">Key, Value types</typeparam>
	/// <param name="LogLevel"></param>
	/// <param name="Message"></param>
	/// <param name="...args">Key, Value pairs</param>
	template <typename... Args>
	void log(const LogLevels LogLevel, const FString& Message, Args... args)
	{
		if (!isLogLevelEnabled(LogLevel))
			return;

		auto JsonTransformer = GetJsonTransformer();

		if (!JsonTransformer)
			return;

		Herald::log(*JsonTransformer, LogLevel, std::string(TCHAR_TO_UTF8(*Message)), args...);
	}

	inline void log(LogLevels Level, const FString& Message)
	{
		if (!isLogLevelEnabled(Level))
			return;

		auto JsonTransformer = GetJsonTransformer();

		if (!JsonTransformer)
			return;

		Herald::log(*JsonTransformer, Level, std::string(TCHAR_TO_UTF8(*Message)));
	}
} // namespace Herald
