// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "ElasticTelemetrySettings.h"
#include "Logging/LogVerbosity.h"

FElasticTelemetrySettings::FElasticTelemetrySettings()
{
	// Set default values
	Enabled = false; // Disabled by default. Requires configuration to work.

	EndpointURL    = TEXT("https://localhost:9200");
	IndexName      = TEXT("UELog");
	EventIndexName = TEXT("testing_game_events");
	Username       = TEXT("DefaultUser");
	Password       = TEXT("ChangeMe");

	EnableFatal       = true;
	EnableError       = true;
	EnableWarning     = true;
	EnableDisplay     = true;
	EnableLog         = true;
	EnableVerbose     = false;
	EnableVeryVerbose = false;

	IncludeCallstacksOnFatal       = true;
	IncludeCallstacksOnError       = true;
	IncludeCallstacksOnWarning     = false;
	IncludeCallstacksOnDisplay     = false;
	IncludeCallstacksOnLog         = false;
	IncludeCallstacksOnVerbose     = false;
	IncludeCallstacksOnVeryVerbose = false;
}

bool FElasticTelemetrySettings::IsLogLevelEnabled(const ELogVerbosity::Type Level) const
{
	switch (Level)
	{
	case ELogVerbosity::NoLogging:
		return false;
	case ELogVerbosity::Fatal:
		return EnableFatal;
	case ELogVerbosity::Error:
		return EnableError;
	case ELogVerbosity::Warning:
		return EnableWarning;
	case ELogVerbosity::Display:
		return EnableDisplay;
	case ELogVerbosity::Log:
		return EnableLog;
	case ELogVerbosity::Verbose:
		return EnableVerbose;
	case ELogVerbosity::VeryVerbose:
		return EnableVeryVerbose;
	default:
		return false;
	}
}
