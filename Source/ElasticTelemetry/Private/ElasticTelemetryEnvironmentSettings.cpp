// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "ElasticTelemetryEnvironmentSettings.h"
#include "ElasticTelemetrySettings.h"
#include "ElasticTelemetry.h"

UElasticTelemetryEnvironmentSettings::UElasticTelemetryEnvironmentSettings()
{
	// Sane defaults. Will be overridden by config if present.
	ActiveEnvironment = "Development";
	Environments = {
		{ "Development", FElasticTelemetrySettings() },
		{ "Staging", FElasticTelemetrySettings() },
		{ "Production", FElasticTelemetrySettings() }
	};
}

const FElasticTelemetrySettings* UElasticTelemetryEnvironmentSettings::GetSettingsForEnvironment(const FString& Key) const
{
	auto Result = Environments.Find(Key);
	return Result;
}

void UElasticTelemetryEnvironmentSettings::SetActiveEnvironment(const FString& NewEnvironment)
{
	if (Environments.Contains(NewEnvironment))
	{
		ActiveEnvironment = NewEnvironment;
		TryUpdateDefaultConfigFile(); // Updated to the new API
	}
	else
	{
		UE_LOG(TelemetryLog, Warning, TEXT("Environment %s does not exist."), *NewEnvironment);
	}
}

const FElasticTelemetrySettings UElasticTelemetryEnvironmentSettings::GetActiveSettings() const
{
	static const FElasticTelemetrySettings DefaultSettings;
	auto								   Active = GetSettingsForEnvironment(ActiveEnvironment);
	if (!Active)
	{
		return DefaultSettings;
	}
	return *Active;
}

FName UElasticTelemetryEnvironmentSettings::GetCategoryName() const
{
	return "Plugins";
}

FName UElasticTelemetryEnvironmentSettings::GetSectionName() const
{
	return "Elastic Telemetry";
}
