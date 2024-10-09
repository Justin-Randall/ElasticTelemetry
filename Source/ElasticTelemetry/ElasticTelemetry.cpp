// Copyright Epic Games, Inc. All Rights Reserved.

#include "ElasticTelemetry.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#include "ElasticTelemetryEnvironmentSettings.h"
#include "HttpModule.h"
#include "Herald/LogLevels.hpp"
#include "ElasticTelemetryOutputDevice.h"
#include "ETLogger.h"

#define LOCTEXT_NAMESPACE "FElasticTelemetryModule"

DEFINE_LOG_CATEGORY(TelemetryLog);

FElasticTelemetryModule::FElasticTelemetryModule()
	: Settings(), OutputDevice(nullptr)
{
}

FElasticTelemetryModule::~FElasticTelemetryModule()
{
	if (OutputDevice)
	{
		delete OutputDevice;
	}
}

void FElasticTelemetryModule::StartupModule()
{
	UpdateConfig();

	// Already spawned from the editor most likely, which is
	// re-logging output.
	if (IsRunningCommandlet())
	{
		ShutdownModule();
		return;
	}

	OutputDevice = new FElasticTelemetryOutputDevice(*this);

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Elastic Telemetry",
			LOCTEXT("ElasticTelemetrySettingsName", "Elastic Telemetry Settings"),
			LOCTEXT("ElasticTelemetrySettingsDescription", "Configure Elastic Telemetry"),
			GetMutableDefault<UElasticTelemetryEnvironmentSettings>());
	}

	// ensure the http module is setup from this thread before attempting to use it elsewhere
	// otherwise, the http module may not be ready when the http logger tries to use it
	FHttpModule::Get();

	const auto ComputerName = FPlatformProcess::ComputerName();
	if (nullptr != ComputerName)
	{
		Herald::addHeader("ComputerName", ComputerName);
	}

#if UE_SERVER
	Herald::setJsonLogHeader("UserName", "DedicatedServer");
#else
	// This returns the locally logged in user, not the Steam/XBox/Sony Network user
	const auto UserName = FPlatformProcess::UserName();
	if (nullptr != UserName)
	{
		Herald::addHeader("UserName", UserName);
	}
#endif
}

void FElasticTelemetryModule::UpdateConfig()
{
	auto NewEnvSettings = GetMutableDefault<UElasticTelemetryEnvironmentSettings>();
	if (nullptr == NewEnvSettings)
	{
		UE_LOG(TelemetryLog, Error, TEXT("Failed to get settings."));
		return;
	}
	FElasticTelemetrySettings NewSettings;
	{
		FScopeLock Lock(&SettingsLock);
		auto	   UpdatedSettings = NewEnvSettings->GetSettingsForEnvironment(NewEnvSettings->GetActiveEnvironment());
		if (nullptr == UpdatedSettings)
		{
			UE_LOG(TelemetryLog, Error, TEXT("Failed to get settings for environment %s."), *NewEnvSettings->GetActiveEnvironment());
			return;
		}

		// Make a local copy to work with as the settings are applied
		Settings = *UpdatedSettings;
	}

	// Apply the settings to the Herald log system
	if (!Settings.Enabled)
	{
		UE_LOG(TelemetryLog, Log, TEXT("Telemetry is disabled."));
		Herald::disableAllLogLevels();
	}
	else
	{
		Settings.EnableFatal ? Herald::enableLogLevel(Herald::LogLevels::Fatal) : Herald::disableLogLevel(Herald::LogLevels::Fatal);
		Settings.EnableError ? Herald::enableLogLevel(Herald::LogLevels::Error) : Herald::disableLogLevel(Herald::LogLevels::Error);
		Settings.EnableWarning ? Herald::enableLogLevel(Herald::LogLevels::Warning) : Herald::disableLogLevel(Herald::LogLevels::Warning);
		Settings.EnableDisplay ? Herald::enableLogLevel(Herald::LogLevels::Info) : Herald::disableLogLevel(Herald::LogLevels::Info);
		Settings.EnableLog ? Herald::enableLogLevel(Herald::LogLevels::Debug) : Herald::disableLogLevel(Herald::LogLevels::Debug);
		Settings.EnableVerbose ? Herald::enableLogLevel(Herald::LogLevels::Trace) : Herald::disableLogLevel(Herald::LogLevels::Trace);
		Settings.EnableVeryVerbose ? Herald::enableLogLevel(Herald::LogLevels::Analysis) : Herald::disableLogLevel(Herald::LogLevels::Analysis);

		// Hook up the Output device writer to the transformer

		UE_LOG(TelemetryLog, Log, TEXT("Telemetry is enabled."));
	}
}

FElasticTelemetrySettings FElasticTelemetryModule::GetSettings() const
{
	FScopeLock Lock(&SettingsLock);
	return Settings;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FElasticTelemetryModule, ElasticTelemetry)