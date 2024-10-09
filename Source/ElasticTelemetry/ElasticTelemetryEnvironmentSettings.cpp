#include "ElasticTelemetryEnvironmentSettings.h"
#include "ElasticTelemetrySettings.h"

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
